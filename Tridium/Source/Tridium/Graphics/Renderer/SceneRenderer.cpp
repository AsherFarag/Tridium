#include "tripch.h"
#if 0
#include "SceneRenderer.h"
#include <Tridium/Math/Random.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Shaders/Families/GBuffer_ShaderInterop.h>
#include <Tridium/Shaders/Families/LitDefault_ShaderInterop.h>
#include <Tridium/Shaders/Families/SkyBox_ShaderInterop.h>

// TEMP
#include <Tridium/Asset/Importers/EnvironmentMapImporter.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>

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

		// Add built-in render passes
		{
			AddRenderPass<RootPass>( String{ Passes::Root } );
			AddRenderPass<GBufferPass>( String{ Passes::GBuffer } );
			AddRenderPass<LightingPass>( String{ Passes::Lighting } );
			AddRenderPass<SkyboxPass>( String{ Passes::Skybox } );
		}
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

		m_SceneCamera.Camera = a_Camera;
		m_SceneCamera.View = a_View;
		m_SceneCamera.Position = a_CameraPosition;
		m_SceneCamera.Projection = a_Camera.GetProjection();

		if ( m_Viewport.NeedsResize )
		{
			m_Viewport.NeedsResize = false; // Reset resize flag

			const uint32_t texWidth = (uint32_t)( (float)m_Viewport.Width * m_Options.TieringSettings.RenderScale );
			const uint32_t texHeight = (uint32_t)( (float)m_Viewport.Height * m_Options.TieringSettings.RenderScale );

			for ( auto& [name, pass] : m_RenderPasses )
			{
				pass->OnViewportResize( m_RenderGraph, texWidth, texHeight );
			}
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

		if ( RootPass* rootPass = GetRenderPass<RootPass>( Passes::Root ) )
		{
			return rootPass->GetOutputTexture();
		}

		return nullptr;
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

	void SceneRenderer::BuildRenderGraph()
	{
		m_RenderGraph.ResetFrame();

		for ( auto& [name, pass] : m_RenderPasses )
		{
			pass->Setup( m_RenderGraph );
		}

		m_RenderGraph.Compile();
	}

	void SceneRenderer::FlushDrawLists()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( m_Viewport.Width == 0 || m_Viewport.Height == 0 )
		{
			return; // Cannot render with zero-sized viewport
		}

		// Build the render graph
		if ( m_RequiresRenderGraphRebuild )
		{
			m_RequiresRenderGraphRebuild = false;

			BuildRenderGraph();
		}

		m_CommandList->Open();
		{
			m_CommandList->PushDebugGroup( "SceneRenderer::FlushDrawLists" );
			m_RenderGraph.Execute( *m_CommandList );
			m_CommandList->ResourceBarrier( *GetOutputTexture(), ERHIResourceStates::Present);
			m_CommandList->PopDebugGroup();
		}
		m_CommandList->Close();

		// Submit the command list to the graphics queue
		IRHICommandList* cmdList = m_CommandList.get();
		RHI::WaitForFence( ERHICommandQueueType::Graphics, RHI::ExecuteCommandLists( { &cmdList, 1 }, ERHICommandQueueType::Graphics ) );

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

	//=================================================================================================
	// RootPass
	//=================================================================================================

	void RootPass::Setup( RenderGraph& a_RenderGraph )
	{
		a_RenderGraph.AddPass( "Root", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			m_Output.first = a_Builder.Import( "Output", m_Output.second );

			a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
			{
				PROFILE_SCOPE( "RenderPass: Root", ProfilerCategory::Rendering );

				TODO( "Add clearing textures without graphics state" );
			} );
		} );
	}

	void RootPass::OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height )
	{
		const RHITextureDesc outputTextureDesc = RHITextureDesc{}
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( a_Width )
			.SetHeight( a_Height )
			.SetFormat( ERHIFormat::RGBA8_UNORM )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
			.SetClearValue( RHIClearValue{} )
			.SetUseClearValue( true )
			.SetName( "SceneRenderer Output Texture" );

		m_Output.second = RHI::CreateTexture( outputTextureDesc );

		// The resource has been recreated, so we need to update the render graph
		a_RenderGraph.UpdateTexture( m_Output.first, m_Output.second );
	}

	//=================================================================================================
	// GBufferPass
	//=================================================================================================

	void GBufferPass::Setup( RenderGraph& a_RenderGraph )
	{
		a_RenderGraph.AddPass( "GBuffer", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			// Import GBuffer textures
			m_Position.first = a_Builder.Import( "Position", m_Position.second );
			m_Albedo.first = a_Builder.Import( "Albedo", m_Albedo.second );
			m_Normal.first = a_Builder.Import( "Normal", m_Normal.second );
			m_MetallicRoughnessAO.first = a_Builder.Import( "MetalRoughAO", m_MetallicRoughnessAO.second );
			m_Emission.first = a_Builder.Import( "Emission", m_Emission.second );
			m_Depth.first = a_Builder.Import( "Depth", m_Depth.second );

			// Declare resource usage
			a_Builder.Write( m_Position.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Albedo.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Normal.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_MetallicRoughnessAO.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Emission.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Depth.first, ERHIResourceStates::DepthStencilWrite );

			a_Builder.Execute( [this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
			{
				PROFILE_SCOPE( "RenderPass: Geometry Pass - StaticMesh", ProfilerCategory::Rendering );

				const auto& sceneCamera = GetSceneRenderer().GetSceneCamera();

				const auto& positionTex = a_Graph.GetTexture( m_Position.first );
				const auto& albedoTex = a_Graph.GetTexture( m_Albedo.first );
				const auto& normalTex = a_Graph.GetTexture( m_Normal.first );
				const auto& metalRoughAmbientTex = a_Graph.GetTexture( m_MetallicRoughnessAO.first );
				const auto& emissionTex = a_Graph.GetTexture( m_Emission.first );
				const auto& depthTex = a_Graph.GetTexture( m_Depth.first );

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
				const Matrix4 projectionView = sceneCamera.Projection * sceneCamera.View;

				for ( const auto& [key, draw] : GetSceneRenderer().GetStaticDrawList().Opaque )
				{
					a_CommandList.SetGraphicsState( graphicsState
						.SetPipelineState( pipelineState.get() )
						.SetIndexBuffer( draw.IndexBuffer.get() )
						.SetVertexBuffer( draw.VertexBuffer.get() )
						.SetBindingSet( materialBindingSetIndex, draw.BindingSet.get() )
					);

					a_CommandList.SetViewportState( RHIViewportState{}
						.AddViewportAndScissor( RHIViewport{ 0, 0,
												(float)GetSceneRenderer().GetViewportWidth(),
												(float)GetSceneRenderer().GetViewportHeight()
												} ) 
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
	}

	void GBufferPass::OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height )
	{
		auto texDesc = RHITextureDesc{}
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( a_Width )
			.SetHeight( a_Height )
			.SetFormat( ERHIFormat::RGBA32_FLOAT )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
			.SetClearValue( RHIClearValue{} )
			.SetUseClearValue( true )
			.SetDefaultSampler(
				RHISampler{}
					.SetFilter( ERHISamplerFilter::MinMagMipLinear )
					.SetAddressU( ERHISamplerAddressMode::Clamp )
					.SetAddressV( ERHISamplerAddressMode::Clamp )
					.SetAddressW( ERHISamplerAddressMode::Clamp )
			);

		m_Position.second = RHI::CreateTexture( texDesc.SetName( "GBuffer Position" ) );
		m_Albedo.second = RHI::CreateTexture( texDesc.SetName( "GBuffer Albedo" ) );
		m_Normal.second = RHI::CreateTexture( texDesc.SetName( "GBuffer Normal" ) );
		m_MetallicRoughnessAO.second = RHI::CreateTexture( texDesc.SetName( "GBuffer MetallicRoughnessAO" ).SetFormat( ERHIFormat::RGBA8_UNORM ) );
		m_Emission.second = RHI::CreateTexture( texDesc.SetName( "GBuffer Emission" ) );
		m_Depth.second = RHI::CreateTexture( texDesc
			.SetFormat( ERHIFormat::D32_FLOAT )
			.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
			.SetName( "GBuffer Depth" ) );

		// The resources have been recreated, so we need to update the render graph
		a_RenderGraph.UpdateTexture( m_Position.first, m_Position.second );
		a_RenderGraph.UpdateTexture( m_Albedo.first, m_Albedo.second );
		a_RenderGraph.UpdateTexture( m_Normal.first, m_Normal.second );
		a_RenderGraph.UpdateTexture( m_MetallicRoughnessAO.first, m_MetallicRoughnessAO.second );
		a_RenderGraph.UpdateTexture( m_Emission.first, m_Emission.second );
		a_RenderGraph.UpdateTexture( m_Depth.first, m_Depth.second );
	}

	//=================================================================================================
	// LightingPass
	//=================================================================================================

	static RenderResourceEnvironmentMap s_DefaultEnvironmentMap;


	void LightingPass::Setup( RenderGraph& a_RenderGraph )
	{
		if ( !m_PipelineState || !m_PipelineState->Valid() )
		{
			auto lightingPassPSODesc = RHIGraphicsPipelineStateDesc{}
				.SetName( "SceneRenderer Lighting Pass Pipeline State" )
				.SetVertexLayout( RHIVertexLayout::From<ViewportQuadVertex>() )
				.SetTopology( ERHITopology::Triangle )
				.SetFramebufferInfo( RHIFramebufferInfo{}.SetColorFormats( { GetSceneRenderer().GetOutputTexture()->Desc().Format } ) );
			ShaderLibrary::GetOrCreateVariant( "LitDefault"_H, { "HIGH_QUALITY" } )->Apply(lightingPassPSODesc);

			m_PipelineState = PipelineStateCache::GetOrCreatePSO(
				lightingPassPSODesc
			);
		}


		if ( !m_QuadVertexBuffer || !m_QuadVertexBuffer->Valid() )
		{
			constexpr ViewportQuadVertex quadVertices[] = {
				{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
				{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
				{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
				{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
			};

			m_QuadVertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "SceneRenderer Fullscreen Quad Vertex Buffer" )
				.SetSize( sizeof( quadVertices ) )
				.SetBindFlags( ERHIBindFlags::VertexBuffer )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetType( ERHIBufferType::Structured )
				.SetStride( sizeof( ViewportQuadVertex ) ),
				AsBytes( Span{ quadVertices } )
			);
		}

		if ( !m_QuadIndexBuffer || !m_QuadIndexBuffer->Valid() )
		{
			constexpr uint16_t quadIndices[] = {
				0, 1, 2,
				2, 3, 0
			};

			m_QuadIndexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "SceneRenderer Fullscreen Quad Index Buffer" )
				.SetSize( sizeof( quadIndices ) )
				.SetBindFlags( ERHIBindFlags::IndexBuffer )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetType( ERHIBufferType::Formatted )
				.SetFormat( ERHIFormat::R16_UINT ),
				AsBytes( Span{ quadIndices } )
			);
		}

		ASSERT( m_PipelineState && m_PipelineState->Valid(), "Lighting pass pipeline state is not valid!" );
		ASSERT( m_QuadVertexBuffer && m_QuadVertexBuffer->Valid(), "Lighting pass quad vertex buffer is not valid!" );
		ASSERT( m_QuadIndexBuffer && m_QuadIndexBuffer->Valid(), "Lighting pass quad index buffer is not valid!" );

		a_RenderGraph.AddPass( "Lighting", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			GBufferPass* gbuffer = GetSceneRenderer().GetRenderPass<GBufferPass>( SceneRenderer::Passes::GBuffer );
			RootPass* rootPass = GetSceneRenderer().GetRenderPass<RootPass>( SceneRenderer::Passes::Root );
			ASSERT( gbuffer && rootPass, "GBuffer and Root passes must be present before Lighting pass." );

			// Import GBuffer textures
			const auto positionID = gbuffer->GetPositionID();
			const auto albedoID = gbuffer->GetAlbedoID();
			const auto normalID = gbuffer->GetNormalID();
			const auto metalRoughAOID = gbuffer->GetMRAOID();
			const auto emissionID = gbuffer->GetEmissionID();
			const auto depthID = gbuffer->GetDepthID();

			// Declare resource usage
			a_Builder.Read( positionID, ERHIResourceStates::ShaderResource );
			a_Builder.Read( albedoID, ERHIResourceStates::ShaderResource );
			a_Builder.Read( normalID, ERHIResourceStates::ShaderResource );
			a_Builder.Read( metalRoughAOID, ERHIResourceStates::ShaderResource );
			a_Builder.Read( emissionID, ERHIResourceStates::ShaderResource );
			a_Builder.Read( depthID, ERHIResourceStates::ShaderResource );

			m_Output = rootPass->GetOutputID();
			a_Builder.Write( m_Output, ERHIResourceStates::RenderTarget );

			// Random lights for testing
			static Array<PointLight> s_PointLights = []() -> Array<PointLight>
			{
				Array<PointLight> lights;
				lights.Reserve( 32 );

				for ( int i = 0; i < 32; i++ )
				{
					PointLight light;
					light.Position = Vector3{
						Math::Random::Range( -10.0f, 10.0f ),
						Math::Random::Range( 0.0f, 5.0f ),
						Math::Random::Range( -10.0f, 10.0f )
					};
					light.Color = Vector3{
						Math::Random::Range( 0.0f, 1.0f ),
						Math::Random::Range( 0.0f, 1.0f ),
						Math::Random::Range( 0.0f, 1.0f )
					};
					light.Intensity = Math::Random::Range( 1.0f, 2.5f );
					light.Radius = Math::Random::Range( 5.0f, 15.0f );
					lights.PushBack( light );
				}

				return lights;
			}( );

			// Create structured buffer for point lights
			static RHIBufferDesc pointLightBufferDesc = RHIBufferDesc{}
				.SetName( "SceneRenderer Point Light Buffer" )
				.SetType( ERHIBufferType::Structured )
				.SetHeapType( ERHIHeapType::Dynamic )
				.SetBindFlags( ERHIBindFlags::ShaderResource )
				.SetSize( sizeof( PointLight ) * s_PointLights.Size() )
				.SetStride( sizeof( PointLight ) );

			const auto pointLightBuffer = RHI::CreateBuffer( pointLightBufferDesc, AsBytes( Span{ s_PointLights.Data(), s_PointLights.Size() } ) );

			a_Builder.Execute( [=, this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
			{
				PROFILE_SCOPE( "RenderPass: Lighting Pass", ProfilerCategory::Rendering );

				const auto& positionTex = a_Graph.GetTexture( positionID );
				const auto& albedoTex = a_Graph.GetTexture( albedoID );
				const auto& normalTex = a_Graph.GetTexture( normalID );
				const auto& metalRoughAOTex = a_Graph.GetTexture( metalRoughAOID );
				const auto& emissionTex = a_Graph.GetTexture( emissionID );
				const auto& depthTex = a_Graph.GetTexture( depthID );

				auto bindingSetDesc = RHIBindingSetDesc{ this->m_PipelineState->Desc().BindingLayouts[0] }
					.AddTexture( "PositionMap"_H, positionTex.get() )
					.AddTexture( "AlbedoMap"_H, albedoTex.get() )
					.AddTexture( "NormalMap"_H, normalTex.get() )
					.AddTexture( "MetalRoughAOMap"_H, metalRoughAOTex.get() )
					.AddTexture( "EmissionMap"_H, emissionTex.get() )
					.AddStructuredBuffer( "PointLights"_H, pointLightBuffer.get() );

				if ( s_DefaultEnvironmentMap.IrradianceMap )
				{
					bindingSetDesc.AddTexture( "IrradianceMap"_H, s_DefaultEnvironmentMap.IrradianceMap.get() );
					bindingSetDesc.AddTexture( "RadianceMap"_H, s_DefaultEnvironmentMap.RadianceMap.get() );
				}

				const auto bindingSet = RHI::CreateBindingSet( bindingSetDesc );

				const auto& lightingOutputTex = a_Graph.GetTexture( m_Output );

				auto graphicsState = RHIGraphicsState{}
					.SetPipelineState( this->m_PipelineState.get() )
					.SetVertexBuffer( this->m_QuadVertexBuffer.get() )
					.SetIndexBuffer( this->m_QuadIndexBuffer.get() )
					.SetBindingSet( 0, bindingSet.get() )
					.SetFramebuffer( RHIFramebuffer{}
						.AddColorAttachment( lightingOutputTex )
					);

				a_CommandList.SetGraphicsState( graphicsState );
				a_CommandList.SetViewportState( RHIViewportState{}
					.AddViewportAndScissor( RHIViewport{ 0, 0, 
											(float)GetSceneRenderer().GetViewportWidth(),
											(float)GetSceneRenderer().GetViewportHeight() } )
				);

				a_CommandList.ClearRenderTargets( ERHIClearFlags::Color, RHIClearValue{} );

				InlinedConstants_LitDefault constants;
				constants.CameraPosition = GetSceneRenderer().GetSceneCamera().Position;
				constants.NumPointLights = Cast<uint32_t>( s_PointLights.Size() );

				// Directional Light
				{
					constants.DirectionalLight.Direction = Vector3{ -0.5f, -1.0f, -0.5f }.Normalized();
					constants.DirectionalLight.Color = Vector3{ 1.0f, 1.0f, 1.0f };
					constants.DirectionalLight.Intensity = 0.0f;
				}

				a_CommandList.SetInlinedConstants( constants );

				a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 4 ).SetIndexCount( 6 ) );

			} );
		} );
	}

	//=================================================================================================
	// SkyboxPass
	//=================================================================================================

	struct SkyboxVertex
	{
		Vector3 Position;
	};

	void SkyboxPass::Setup( RenderGraph& a_RenderGraph )
	{
		if ( !s_DefaultEnvironmentMap.Valid() )
		{
			//const FilePath assetFilePath = "TestProject/Content/resting_place_2_4k.hdr";
			const FilePath assetFilePath = "TestProject/Content/studio_small.hdr";
			//const FilePath assetFilePath = "TestProject/Content/park_music_stage_4k.hdr";
			auto envMapImporter = AssetFactory::GetImporter( assetFilePath.GetExtension().ToString() );
			static AssetRef<EnvironmentMap> importedEnvMapAsset;
			AssetImportContext context;
			context.m_AssetPath = assetFilePath;
			envMapImporter->OnImport( context );
			importedEnvMapAsset = SharedPtrCast<EnvironmentMap>( context.m_CreatedAssets.Back().second );

			s_DefaultEnvironmentMap = RenderResourceManager::GetOrCreateEnvironmentMap( importedEnvMapAsset );
		}

		if ( !m_PipelineState || !m_PipelineState->Valid() )
		{
			auto psoDesc = RHIGraphicsPipelineStateDesc{}
				.SetName( "SceneRenderer Skybox Pass Pipeline State" )
				.SetVertexLayout( RHIVertexLayout::From<SkyboxVertex>() )
				.SetTopology( ERHITopology::Triangle )
				.SetDepthState( RHIDepthState{}
					.SetDepthTestEnabled( true )
					.SetDepthWriteEnabled( false )
					.SetComparison( ERHIComparison::LessEqual ) )
				.SetRasterizerState( RHIRasterizerState{}
									 .SetCullMode( ERHICullMode::Front ) ) // Cull front faces to render inside of cube
				.SetFramebufferInfo( RHIFramebufferInfo{}
					.SetColorFormats( { GetSceneRenderer().GetOutputTexture()->Desc().Format } )
					.SetDepthStencilFormat( ERHIFormat::D32_FLOAT )
				);

			ShaderLibrary::GetOrCreateVariant( "SkyBox"_H )->Apply( psoDesc );

			m_PipelineState = PipelineStateCache::GetOrCreatePSO(
				psoDesc
			);
		}

		if ( !m_CubeVertexBuffer || !m_CubeVertexBuffer->Valid() )
		{
			constexpr SkyboxVertex cubeVertices[] = {
				{ { -1.0f, -1.0f,  1.0f } }, // 0
				{ {  1.0f, -1.0f,  1.0f } }, // 1
				{ {  1.0f,  1.0f,  1.0f } }, // 2
				{ { -1.0f,  1.0f,  1.0f } }, // 3
				{ { -1.0f, -1.0f, -1.0f } }, // 4
				{ {  1.0f, -1.0f, -1.0f } }, // 5
				{ {  1.0f,  1.0f, -1.0f } }, // 6
				{ { -1.0f,  1.0f, -1.0f } }, // 7
			};
			m_CubeVertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "SceneRenderer Skybox Cube Vertex Buffer" )
				.SetSize( sizeof( cubeVertices ) )
				.SetBindFlags( ERHIBindFlags::VertexBuffer )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetType( ERHIBufferType::Structured )
				.SetStride( sizeof( SkyboxVertex ) ),
				AsBytes( Span{ cubeVertices } )
			);
		}

		if ( !m_CubeIndexBuffer || !m_CubeIndexBuffer->Valid() )
		{
			constexpr uint16_t cubeIndices[] = {
				0, 1, 2, 2, 3, 0, // Front face
				1, 5, 6, 6, 2, 1, // Right face
				5, 4, 7, 7, 6, 5, // Back face
				4, 0, 3, 3, 7, 4, // Left face
				3, 2, 6, 6, 7, 3, // Top face
				4, 5, 1, 1, 0, 4  // Bottom face
			};
			m_CubeIndexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
				.SetName( "SceneRenderer Skybox Cube Index Buffer" )
				.SetSize( sizeof( cubeIndices ) )
				.SetBindFlags( ERHIBindFlags::IndexBuffer )
				.SetHeapType( ERHIHeapType::Immutable )
				.SetType( ERHIBufferType::Formatted )
				.SetFormat( ERHIFormat::R16_UINT ),
				AsBytes( Span{ cubeIndices } )
			);
		}

		a_RenderGraph.AddPass( "Skybox", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			LightingPass* rootPass = GetSceneRenderer().GetRenderPass<LightingPass>( SceneRenderer::Passes::Lighting );
			ASSERT( rootPass, "Root pass must be present before Skybox pass." );

			m_Output = rootPass->GetOutputID();

			GBufferPass* gbuffer = GetSceneRenderer().GetRenderPass<GBufferPass>( SceneRenderer::Passes::GBuffer );
			ASSERT( gbuffer, "GBuffer pass must be present before Skybox pass." );

			m_Depth = gbuffer->GetDepthID();
			a_Builder.Read( m_Depth, ERHIResourceStates::DepthStencilRead );
			a_Builder.Write( m_Output, ERHIResourceStates::RenderTarget );

			a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
			{
				PROFILE_SCOPE( "RenderPass: Skybox", ProfilerCategory::Rendering );

				const auto& lightingOutputTex = a_Graph.GetTexture( m_Output );
				ASSERT( lightingOutputTex, "Lighting output texture is null in Skybox pass!" );

				const auto& depthTex = a_Graph.GetTexture( m_Depth );
				ASSERT( depthTex, "Depth texture is null in Skybox pass!" );

				RHIBindingSetRef s_BindingSet = [this]()
				{
					// Disable mipmap filtering for skybox
					const auto sampler = RHISampler{}
						.SetFilter( ERHISamplerFilter::MinMagMipLinear )
						.SetAddressU( ERHISamplerAddressMode::Clamp )
						.SetAddressV( ERHISamplerAddressMode::Clamp )
						.SetAddressW( ERHISamplerAddressMode::Clamp );

					auto bindingSetDesc = RHIBindingSetDesc{ this->m_PipelineState->Desc().BindingLayouts[0] };
					bindingSetDesc.AddTexture( "SkyboxMap"_H, s_DefaultEnvironmentMap.RadianceMap.get(), &sampler );
					return RHI::CreateBindingSet( bindingSetDesc );
				}();
				
				auto graphicsState = RHIGraphicsState{}
					.SetPipelineState( this->m_PipelineState.get() )
					.SetVertexBuffer( this->m_CubeVertexBuffer.get() )
					.SetIndexBuffer( this->m_CubeIndexBuffer.get() )
					.SetBindingSet( 0, s_BindingSet.get() )
					.SetFramebuffer( RHIFramebuffer{}
						.AddColorAttachment( lightingOutputTex )
						.SetDepthStencilAttachment( depthTex, true )
					);

				a_CommandList.SetGraphicsState( graphicsState );
				a_CommandList.SetViewportState( RHIViewportState{}
					.AddViewportAndScissor( RHIViewport{ 0, 0,
											(float)GetSceneRenderer().GetViewportWidth(),
											(float)GetSceneRenderer().GetViewportHeight() } )
				);

				InlinedConstants_SkyBox constants;
				constants.Projection = GetSceneRenderer().GetSceneCamera().Projection;
				constants.View = GetSceneRenderer().GetSceneCamera().View;
				a_CommandList.SetInlinedConstants( constants );

				a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 36 ).SetIndexCount( 36 ) );
			} );
		} );
	}

} // namespace Tridium

#endif