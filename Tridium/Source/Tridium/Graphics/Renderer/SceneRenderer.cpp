#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

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
				.SetEnableImmediateExecution( false )
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

			m_OutputTexture = RHI::CreateTexture( RHITextureDesc{ m_OutputTexture->Desc() }
													.SetWidth( m_Viewport.Width )
													.SetHeight( m_Viewport.Height )
													.SetName( "SceneRenderer Output Texture" ) );

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
			m_RenderGraph.AddPass( "Geometry Pass - StaticMesh", ERHICommandQueueType::Graphics, [ & ]( RenderPassBuilder& a_Builder )
			{
					auto backBuffer = a_Builder.Import( m_OutputTexture );
					auto albedo = a_Builder.Create( "Albedo", RHITextureDesc{}
						.SetDimension( ERHITextureDimension::Texture2D )
						.SetWidth( m_Viewport.Width )
						.SetHeight( m_Viewport.Height )
						.SetFormat( ERHIFormat::RGBA8_UNORM )
						.SetBindFlags( ERHIBindFlags::RenderTarget )
						.SetClearValue( RHIClearValue{} )
						.SetUseClearValue( true )
						.SetName( "GBuffer Albedo" ) );

					auto normal = a_Builder.Create( "Normal", RHITextureDesc{}
						.SetDimension( ERHITextureDimension::Texture2D )
						.SetWidth( m_Viewport.Width )
						.SetHeight( m_Viewport.Height )
						.SetFormat( ERHIFormat::RGBA8_UNORM )
						.SetBindFlags( ERHIBindFlags::RenderTarget )
						.SetClearValue( RHIClearValue{} )
						.SetUseClearValue( true )
						.SetName( "GBuffer Normal" ) );

					auto depth = a_Builder.Create( "Depth", RHITextureDesc{}
						.SetDimension( ERHITextureDimension::Texture2D )
						.SetWidth( m_Viewport.Width )
						.SetHeight( m_Viewport.Height )
						.SetFormat( ERHIFormat::D32_FLOAT )
						.SetBindFlags( ERHIBindFlags::DepthStencil )
						.SetClearValue( RHIClearValue{} )
						.SetUseClearValue( true )
						.SetName( "GBuffer Depth" ) );

					a_Builder.Write( albedo, ERHIResourceStates::RenderTarget );
					a_Builder.Write( normal, ERHIResourceStates::RenderTarget );
					a_Builder.Write( depth, ERHIResourceStates::DepthStencilWrite );

					a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
						{
							PROFILE_SCOPE( "RenderPass: Geometry Pass - StaticMesh", ProfilerCategory::Rendering );

							const RHITextureRef& albedoTex = a_Graph.GetTexture( backBuffer );
							const RHITextureRef& normalTex = a_Graph.GetTexture( normal );
							const RHITextureRef& depthTex = a_Graph.GetTexture( depth );

							auto graphicsState = RHIGraphicsState{}
								.SetFramebuffer( 
									RHIFramebuffer{}
									.AddColorAttachment( albedoTex )
									.AddColorAttachment( normalTex )
									.SetDepthStencilAttachment( depthTex ) 
								);

							bool cleared = false;
							static float time = 0.0f;
							time += 0.016f;
							float zPos = Math::Sin( time ) * -10.0f;

							const Matrix4 ProjectionView = m_CameraData.Projection * m_CameraData.View;
							for ( const auto& [key, draw] : m_StaticDrawLists.Opaque )
							{
								graphicsState.SetIndexBuffer( draw.IndexBuffer.get() )
									.SetVertexBuffer( draw.VertexBuffer.get() )
									.SetPipelineState( draw.PipelineState.get() )
									.SetBindingSet( 0, draw.BindingSet.get() );

								a_CommandList.SetGraphicsState( graphicsState );
								a_CommandList.SetViewportState( RHIViewportState{}
									.AddViewportAndScissor( RHIViewport{ 0, 0, (float)m_Viewport.Width, (float)m_Viewport.Height } )
								);

								if ( !cleared )
								{
									a_CommandList.ClearRenderTargets( ERHIClearFlags::All, RHIClearValue{} );
									cleared = true;
								}

								Matrix4 model = draw.InstanceTransforms.Front();
								Matrix4 PVM = ProjectionView * model;
								a_CommandList.SetInlinedConstants( PVM );

								const auto drawArgs = RHIDrawArgs{}
									.SetBaseVertex( draw.VertexBufferRange.Offset )
									.SetVertexCount( draw.VertexBufferRange.Size )
									.SetBaseIndex( draw.VertexBufferRange.Offset )
									.SetIndexCount( draw.IndexBufferRange.Size )
									.SetInstanceCount( Cast<uint32_t>( 1 ) );

								a_CommandList.Draw( drawArgs );
							}
						} );

			} );

			// Present pass
			m_RenderGraph.AddPass( "Present Pass", ERHICommandQueueType::Graphics, [ & ]( RenderPassBuilder& a_Builder )
			{
				//auto backBuffer = a_Builder.Import( m_OutputTexture );
				//a_Builder.Read( backBuffer, ERHIResourceStates::Present );
				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
					{
						//PROFILE_SCOPE( "RenderPass: Present Pass", ProfilerCategory::Rendering );
						//const RHITextureRef& backBufferTex = a_Graph.GetTexture( backBuffer );
						//auto graphicsState = RHIGraphicsState{}
						//	.SetFramebuffer(
						//		RHIFramebuffer{}
						//		.AddColorAttachment( backBufferTex )
						//	);
						//a_CommandList.SetGraphicsState( graphicsState );
						//a_CommandList.SetViewportState( RHIViewportState{}
						//	.AddViewportAndScissor( RHIViewport{ 0, 0, (float)m_Viewport.Width, (float)m_Viewport.Height } ) );
						//a_CommandList.ClearRenderTargets( ERHIClearFlags::All, RHIClearValue{}.SetColor( Color::Blue() ) );

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