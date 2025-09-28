#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Shaders/GBuffer_ShaderInterop.h>

namespace Tridium {

	struct ViewportQuadVertex
	{
		Vector3 Position;
		Vector2 TexCoord;
	};

	SceneRenderer::SceneRenderer( SharedPtr<Scene> a_Scene, const SceneRendererOptions& a_Options )
		: m_Scene( std::move( a_Scene ) ),
		  m_Options( a_Options )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		ASSERT( m_Scene != nullptr, "Scene cannot be null." );

		// Create graphics command list
		{
			const auto cmdListDesc = RHICommandListDesc{}
				.SetQueueType( ERHICommandQueueType::Graphics )
				.SetEnableImmediateExecution( true )
				.SetName( "SceneRenderer Graphics Command List" );

			m_CommandList = RHI::CreateCommandList( cmdListDesc );
		}

		// Create output texture
		{
			const RHITextureDesc outputTextureDesc = RHITextureDesc{}
				.SetDimension( ERHITextureDimension::Texture2D )
				.SetWidth( 1024 ) // Will be set later
				.SetHeight( 1024 ) // Will be set later
				.SetFormat( ERHIFormat::RGBA8_UNORM )
				.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
				.SetClearValue( RHIClearValue{} )
				.SetUseClearValue( true )
				.SetName( "SceneRenderer Output Texture" );

		#if !WITH_EDITOR
			m_OutputTexture = RHI::GetSwapChain()->GetBackBuffer();
		#else
			m_OutputTexture = RHI::CreateTexture( outputTextureDesc );
		#endif
		}

		static constexpr ViewportQuadVertex quadVertices[] = {
			{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
			{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
			{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
		};

		static constexpr uint16_t quadIndices[] = {
			0, 1, 2,
			2, 3, 0
		};

		auto lightingPassPSODesc = RHIGraphicsPipelineStateDesc{}
			.SetName( "SceneRenderer Lighting Pass Pipeline State" )
			.SetVertexLayout( RHIVertexLayout::From<ViewportQuadVertex>() )
			.SetTopology( ERHITopology::Triangle )
			.SetFramebufferInfo( RHIFramebufferInfo{}.SetColorFormats( { m_OutputTexture->Desc().Format } ) );
		ShaderLibrary::GetOrCreateVariant( "LitDefault"_H )->Apply( lightingPassPSODesc );

		m_Passes.LightingPass.PipelineState = PipelineStateCache::GetOrCreatePSO(
			lightingPassPSODesc
		);

		m_Passes.LightingPass.QuadVertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
			.SetName( "SceneRenderer Fullscreen Quad Vertex Buffer" )
			.SetSize( sizeof( quadVertices ) )
			.SetBindFlags( ERHIBindFlags::VertexBuffer )
			.SetUsage( ERHIUsage::Static )
			.SetType( ERHIBufferType::Structured )
			.SetStride( sizeof( ViewportQuadVertex ) ),
			AsBytes( Span{ quadVertices } )
		);

		m_Passes.LightingPass.QuadIndexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
			.SetName( "SceneRenderer Fullscreen Quad Index Buffer" )
			.SetSize( sizeof( quadIndices ) )
			.SetBindFlags( ERHIBindFlags::IndexBuffer )
			.SetUsage( ERHIUsage::Static )
			.SetType( ERHIBufferType::Formatted )
			.SetFormat( ERHIFormat::R16_UINT ),
			AsBytes( Span{ quadIndices } )
		);
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Open( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( !ASSERT( !IsOpen(), "Cannot open renderer while it is already open. Call Close() first." ) )
		{
			return;
		}

		m_Open = true;

		m_CameraData.Camera = a_Camera;
		m_CameraData.View = a_View;
		m_CameraData.Position = a_CameraPosition;
		m_CameraData.Projection = a_Camera.GetProjection();

		if ( m_Viewport.NeedsResize )
		{
			m_Viewport.NeedsResize = false; // Reset resize flag

			const uint32_t texWidth = (uint32_t)( (float)m_Viewport.Width * m_Options.TieringSettings.RenderScale );
			const uint32_t texHeight = (uint32_t)( (float)m_Viewport.Height * m_Options.TieringSettings.RenderScale );

			m_OutputTexture = RHI::CreateTexture( RHITextureDesc{ m_OutputTexture->Desc() }
													.SetWidth( texWidth )
													.SetHeight( texHeight )
													.SetName( "SceneRenderer Output Texture" ) );

			// Geometry Pass
			{
				auto texDesc = RHITextureDesc{}
					.SetDimension( ERHITextureDimension::Texture2D )
					.SetWidth( texWidth )
					.SetHeight( texHeight )
					.SetFormat( ERHIFormat::RGBA16_FLOAT )
					.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
					.SetClearValue( RHIClearValue{} )
					.SetUseClearValue( true );

				m_Passes.GeometryPass.Position = RHI::CreateTexture( texDesc.SetName( "GBuffer Position" ) );
				m_Passes.GeometryPass.Albedo = RHI::CreateTexture( texDesc.SetName( "GBuffer Albedo" ) );
				m_Passes.GeometryPass.Normal = RHI::CreateTexture( texDesc.SetName( "GBuffer Normal" ) );
				m_Passes.GeometryPass.MetallicRoughnessAO = RHI::CreateTexture( texDesc.SetName( "GBuffer MetallicRoughnessAO" ) );
				m_Passes.GeometryPass.Emission = RHI::CreateTexture( texDesc.SetName( "GBuffer Emission" ) );
				m_Passes.GeometryPass.Depth = RHI::CreateTexture( texDesc
					.SetFormat( ERHIFormat::D32_FLOAT )
					.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
					.SetName( "GBuffer Depth" ) );
			}

			TODO( "Handle viewport resize logic here" );
		}
	}

	void SceneRenderer::Close()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( !ASSERT( IsOpen(), "Cannot close renderer while it is not open. Call Open() first." ) )
		{
			return;
		}

		// Flush any pending draw lists
		FlushDrawLists();

		m_Open = false;
	}

	void SceneRenderer::SubmitStaticMesh( AssetRef<StaticMesh> a_StaticMesh, const Matrix4& a_Transform )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( !ASSERT( IsOpen(), "Cannot submit static mesh while renderer is not open. Call Open() first." ) )
		{
			return;
		}

		if ( !ASSERT( a_StaticMesh != nullptr ) )
		{
			return;
		}

		const RenderResourceStaticMesh meshResource = RenderResourceManager::GetOrCreateStaticMesh( a_StaticMesh );

		// Go through each submesh in the static mesh and create a draw call for it
		for ( size_t i = 0; i < a_StaticMesh->SubMeshes().Size(); i++ )
		{
			const SubMesh& subMesh = a_StaticMesh->SubMeshes()[ i ];

			// Use the override material if it exists, otherwise use the material from the source mesh
			AssetRef<Material> material = a_StaticMesh->GetMaterial( subMesh );

			const MeshKey key{ 
				.MeshAsset = a_StaticMesh->ID(),
				.MaterialAsset = material ? material->ID() : InvalidAssetID,
				.SubMeshIndex = Cast<uint32_t>( i )
			};

			// Add the submesh to the geometry draw list
			{
				StaticDrawList& drawList = material->Transparent() ? m_StaticDrawLists.Transparent : m_StaticDrawLists.Opaque;
				auto& drawCall = drawList[ key ];

				if ( drawCall.Empty() )
				{
					// We're creating this draw call for the first time, so set it up
					drawCall.VertexBuffer = meshResource.VertexBuffer;
					drawCall.IndexBuffer = meshResource.IndexBuffer;
					drawCall.VertexBufferRange = { subMesh.BaseVertexIndex, subMesh.VertexCount };
					drawCall.IndexBufferRange = { subMesh.BaseIndex, subMesh.IndexCount };

					const RenderResourceMaterial materialResource = RenderResourceManager::GetOrCreateMaterial( material );
					if ( CHECK( materialResource.Valid(), "Material render resource is not valid for material '{}'", material->ID() ) )
					{
						// Build the pipeline state for this draw call
						auto pipelineDesc = RHIGraphicsPipelineStateDesc{}
							.SetTopology( materialResource.Pipeline.Topology )
							.SetVertexLayout( meshResource.VertexLayout )
							.SetFramebufferInfo( RHIFramebufferInfo{}
								.SetColorFormats( { ERHIFormat::RGBA8_UNORM, ERHIFormat::RGBA8_UNORM } )
								.SetDepthStencilFormat( ERHIFormat::D32_FLOAT )
								.SetSampleCount( 1 ) )
							.SetRasterizerState( materialResource.Pipeline.RasterizerState )
							.SetDepthState( materialResource.Pipeline.DepthState )
							.SetBlendState( RHIBlendState{}
								.SetRenderTarget( 0, materialResource.Pipeline.BlendState )
								.SetIndependentBlendEnabled( false ) );

						materialResource.Pipeline.ShaderVariant->Apply( pipelineDesc );

						drawCall.PipelineState = PipelineStateCache::GetOrCreatePSO( pipelineDesc );
						drawCall.BindingSet = materialResource.BindingSet;
					}
				}

				drawCall.InstanceTransforms.PushBack( a_Transform * subMesh.Transform );
			}

			// If the material casts shadows, add it to the shadow draw list
			if ( material->CastsShadows() )
			{
				auto& drawCall = m_StaticDrawLists.Shadow[ key ];

				if ( drawCall.Empty() )
				{
					drawCall.VertexBuffer = meshResource.VertexBuffer;
					drawCall.IndexBuffer = meshResource.IndexBuffer;
					drawCall.VertexBufferRange = { subMesh.BaseVertexIndex, subMesh.VertexCount };
					drawCall.IndexBufferRange = { subMesh.BaseIndex, subMesh.IndexCount };
				}

				drawCall.InstanceTransforms.PushBack( a_Transform );
			}
		}

	}

	RHITextureRef SceneRenderer::GetOutputTexture() const
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		return m_OutputTexture;
	}

	void SceneRenderer::SetScene( SharedPtr<Scene> a_Scene )
	{
		if ( !ASSERT( IsOpen(), "Cannot set scene while renderer is open. Call Close() first." ) )
		{
			return;
		}

		m_Scene = std::move( a_Scene );
	}

	void SceneRenderer::SetViewportSize( uint32_t a_Width, uint32_t a_Height )
	{
		const uint32_t width = ( uint32_t )( ( float )a_Width * m_Options.TieringSettings.RenderScale );
		const uint32_t height = ( uint32_t )( ( float )a_Height * m_Options.TieringSettings.RenderScale );

		if ( m_Viewport.Width != width || m_Viewport.Height != height )
		{
			m_Viewport.Width = width;
			m_Viewport.Height = height;
			m_Viewport.InvWidth = ( m_Viewport.Width > 0 ) ? 1.0f / ( float )m_Viewport.Width : 0.0f;
			m_Viewport.InvHeight = ( m_Viewport.Height > 0 ) ? 1.0f / ( float )m_Viewport.Height : 0.0f;
			m_Viewport.NeedsResize = true; // Mark viewport as needing resize
		}
	}

	void SceneRenderer::FlushDrawLists()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( m_Viewport.Width == 0 || m_Viewport.Height == 0 )
		{
			return; // Cannot render with zero-sized viewport
		}

		// Build the render graph
		{
			RenderPassTextureID outputID;
			m_RenderGraph.AddPass( "Root", ERHICommandQueueType::Graphics, [&](RenderPassBuilder& a_Builder)
			{
				outputID = a_Builder.Import( "Output", m_OutputTexture);

				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
					{
						PROFILE_SCOPE( "RenderPass: Root", ProfilerCategory::Rendering );

						TODO( "Add clearing textures without graphics state" );
					} );
			} );

			m_RenderGraph.AddPass( "GBuffer", ERHICommandQueueType::Graphics, [&](RenderPassBuilder& a_Builder)
			{
				const auto& gbuffer = m_Passes.GeometryPass;

				// Import GBuffer textures
				const auto positionID = a_Builder.Import( "Position", gbuffer.Position );
				const auto albedoID = a_Builder.Import( "Albedo", gbuffer.Albedo );
				const auto normalID = a_Builder.Import( "Normal", gbuffer.Normal );
				const auto metalRoughAmbientID = a_Builder.Import( "MetalRoughAO", gbuffer.MetallicRoughnessAO);
				const auto emissionID = a_Builder.Import( "Emission", gbuffer.Emission );
				const auto depthID = a_Builder.Import( "Depth", gbuffer.Depth);

				// Declare resource usage
				a_Builder.Write( positionID, ERHIResourceStates::RenderTarget );
				a_Builder.Write( albedoID, ERHIResourceStates::RenderTarget );
				a_Builder.Write( normalID, ERHIResourceStates::RenderTarget );
				a_Builder.Write( metalRoughAmbientID, ERHIResourceStates::RenderTarget );
				a_Builder.Write( emissionID, ERHIResourceStates::RenderTarget );
				a_Builder.Write( depthID, ERHIResourceStates::DepthStencilWrite );

				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
				{
					PROFILE_SCOPE( "RenderPass: Geometry Pass - StaticMesh", ProfilerCategory::Rendering );

					const auto& positionTex = a_Graph.GetTexture( positionID );
					const auto& albedoTex = a_Graph.GetTexture( albedoID );
					const auto& normalTex = a_Graph.GetTexture( normalID );
					const auto& metalRoughAmbientTex = a_Graph.GetTexture( metalRoughAmbientID );
					const auto& emissionTex = a_Graph.GetTexture( emissionID );
					const auto& depthTex = a_Graph.GetTexture( depthID );

					const auto& gbufferShader = ShaderLibrary::GetOrCreateVariant( "GBuffer"_H );
					ENSURE( gbufferShader != nullptr, "GBuffer shader not found!" );

					auto pipelineStateDesc = RHIGraphicsPipelineStateDesc{}
						.SetTopology( ERHITopology::Triangle )
						.SetBlendState( RHIBlendState{} ) // Disable blending
						.SetDepthState( RHIDepthState{}
							.SetDepthTestEnabled( true )
							.SetDepthWriteEnabled( true ) )
						.SetVertexLayout( RHIVertexLayout::From<Vertex>() )
						.SetRasterizerState( RHIRasterizerState{} )
						.SetFramebufferInfo(
							RHIFramebufferInfo{}
								.SetColorFormats( { positionTex->Desc().Format, albedoTex->Desc().Format, normalTex->Desc().Format, metalRoughAmbientTex->Desc().Format, emissionTex->Desc().Format } )
								.SetDepthStencilFormat( depthTex->Desc().Format )
								.SetSampleCount( 1 )
						);
					gbufferShader->Apply( pipelineStateDesc );

					RHIGraphicsPipelineStateRef pipelineState = PipelineStateCache::GetOrCreatePSO( pipelineStateDesc );
					ENSURE( pipelineState != nullptr, "Failed to create pipeline state for GBuffer pass!" );

					auto graphicsState = RHIGraphicsState{}
						.SetPipelineState( pipelineState.get() )
						.SetFramebuffer( RHIFramebuffer{}
							.AddColorAttachment( positionTex )
							.AddColorAttachment( albedoTex )
							.AddColorAttachment( normalTex )
							.AddColorAttachment( metalRoughAmbientTex )
							.AddColorAttachment( emissionTex )
							.SetDepthStencilAttachment( depthTex ) 
						);

					bool cleared = false;

					const size_t materialBindingSetIndex = 0;
					const Matrix4 projectionView = m_CameraData.Projection * m_CameraData.View;

					for ( const auto& [key, draw] : m_StaticDrawLists.Opaque )
					{
						a_CommandList.SetGraphicsState( graphicsState
							.SetPipelineState( pipelineState.get() )
							.SetIndexBuffer( draw.IndexBuffer.get() )
							.SetVertexBuffer( draw.VertexBuffer.get() )
							.SetBindingSet( materialBindingSetIndex, draw.BindingSet.get() )
						);

						a_CommandList.SetViewportState( RHIViewportState{}
							.AddViewportAndScissor( RHIViewport{ 0, 0, (float)m_Viewport.Width, (float)m_Viewport.Height } )
						);

						InlinedConstants_GBuffer constants;
						constants.Model = draw.InstanceTransforms.Back();
						constants.PVM = projectionView * constants.Model;
						a_CommandList.SetInlinedConstants( constants );

						if ( !cleared )
						{
							a_CommandList.ClearRenderTargets( ERHIClearFlags::All, RHIClearValue{} );
							cleared = true;
						}

						a_CommandList.Draw( RHIDrawArgs{}
							.SetBaseVertex( draw.VertexBufferRange.Offset )
							.SetVertexCount( draw.VertexBufferRange.Size )
							.SetBaseIndex( draw.VertexBufferRange.Offset )
							.SetIndexCount( draw.IndexBufferRange.Size )
							.SetInstanceCount( Cast<uint32_t>( 1 ) ) 
						);
					}
				} );
			} );

			m_RenderGraph.AddPass( "Lighting", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
			{
				RenderPass* gbuffer = a_Builder.GetPass( Passes::GBuffer );
				ASSERT( gbuffer, "GBuffer pass not found!" );

				// Import GBuffer textures
				const auto positionID = gbuffer->GetTextureID( "Position" );
				const auto albedoID = gbuffer->GetTextureID( "Albedo" );
				const auto normalID = gbuffer->GetTextureID( "Normal" );
				const auto metalRoughAOID = gbuffer->GetTextureID( "MetalRoughAO" );
				const auto emissionID = gbuffer->GetTextureID( "Emission" );
				const auto depthID = gbuffer->GetTextureID( "Depth" );

				// Declare resource usage
				a_Builder.Read( positionID, ERHIResourceStates::ShaderResource );
				a_Builder.Read( albedoID, ERHIResourceStates::ShaderResource );
				a_Builder.Read( normalID, ERHIResourceStates::ShaderResource );
				a_Builder.Read( metalRoughAOID, ERHIResourceStates::ShaderResource );
				a_Builder.Read( emissionID, ERHIResourceStates::ShaderResource );
				a_Builder.Read( depthID, ERHIResourceStates::ShaderResource );

				// Import lighting output
				const auto lightingOutputID = a_Builder.Import( "Output", m_Passes.LightingPass.Output);

				// Declare resource usage
				a_Builder.Write( outputID, ERHIResourceStates::RenderTarget );

				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
				{
					PROFILE_SCOPE( "RenderPass: Lighting Pass", ProfilerCategory::Rendering );

					const auto& positionTex = a_Graph.GetTexture( positionID );
					const auto& albedoTex = a_Graph.GetTexture( albedoID );
					const auto& normalTex = a_Graph.GetTexture( normalID );
					const auto& metalRoughAOTex = a_Graph.GetTexture( metalRoughAOID );
					const auto& emissionTex = a_Graph.GetTexture( emissionID );
					const auto& depthTex = a_Graph.GetTexture( depthID );

					auto bindingSetDesc = RHIBindingSetDesc{ m_Passes.LightingPass.PipelineState->Desc().BindingLayouts[0] }
						.AddTexture( "PositionMap"_H, positionTex.get() )
						.AddTexture( "AlbedoMap"_H, albedoTex.get() )
						.AddTexture( "NormalMap"_H, normalTex.get() )
						.AddTexture( "MetalRoughAOMap"_H, metalRoughAOTex.get() )
						.AddTexture( "EmissionMap"_H, emissionTex.get() );

					const auto bindingSet = RHI::CreateBindingSet( bindingSetDesc );

					const auto& lightingOutputTex = a_Graph.GetTexture( outputID );

					auto graphicsState = RHIGraphicsState{}
						.SetPipelineState( m_Passes.LightingPass.PipelineState.get() )
						.SetVertexBuffer( m_Passes.LightingPass.QuadVertexBuffer.get() )
						.SetIndexBuffer( m_Passes.LightingPass.QuadIndexBuffer.get() )
						.SetBindingSet( 0, bindingSet.get() )
						.SetFramebuffer( RHIFramebuffer{}
							.AddColorAttachment( lightingOutputTex )
						);

					a_CommandList.SetGraphicsState( graphicsState );
					a_CommandList.SetViewportState( RHIViewportState{}
						.AddViewportAndScissor( RHIViewport{ 0, 0, (float)m_Viewport.Width, (float)m_Viewport.Height } )
					);

					a_CommandList.ClearRenderTargets( ERHIClearFlags::Color, RHIClearValue{} );
					a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 4 ).SetIndexCount( 6 ) );

				} );
			} );

			m_RenderGraph.AddPass( "Present", ERHICommandQueueType::Graphics, [ & ]( RenderPassBuilder& a_Builder )
			{
				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
					{
						PROFILE_SCOPE( "RenderPass: Present Pass", ProfilerCategory::Rendering );
					} );
			} );
		}

		m_RenderGraph.Compile();

		m_CommandList->Open();
		{
			m_CommandList->PushDebugGroup( "SceneRenderer::FlushDrawLists" );
			m_RenderGraph.Execute( *m_CommandList );
			m_CommandList->ResourceBarrier( *m_OutputTexture, ERHIResourceStates::Present );
			m_CommandList->PopDebugGroup();
		}
		m_CommandList->Close();

		// Submit the command list to the graphics queue
		IRHICommandList* cmdList = m_CommandList.get();
		RHI::WaitForFence( ERHICommandQueueType::Graphics, RHI::ExecuteCommandLists( { &cmdList, 1 }, ERHICommandQueueType::Graphics ) );

		m_RenderGraph.ResetFrame();

		// Clear the frame-only state
		ClearFrameData();
	}

	void SceneRenderer::ClearFrameData()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		m_StaticDrawLists.Opaque.clear();
		m_StaticDrawLists.Transparent.clear();
		m_StaticDrawLists.Shadow.clear();
	}

} // namespace Tridium