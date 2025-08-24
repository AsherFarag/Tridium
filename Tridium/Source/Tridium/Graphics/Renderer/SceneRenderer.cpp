#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Graphics/RHI/RHI.h>
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
				.SetClearValue( RHIClearValue{}.SetColor( Color::Blue() ) )
				.SetUseClearValue( true )
				.SetName( "SceneRenderer Output Texture" );
			m_OutputTexture = RHI::CreateTexture( outputTextureDesc );
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

	void SceneRenderer::SubmitStaticMesh( SharedPtr<T::StaticMesh> a_StaticMesh, const Matrix4& a_Transform )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( !ASSERT( IsOpen(), "Cannot submit static mesh while renderer is not open. Call Open() first." ) )
		{
			return;
		}

		// Go through each submesh in the static mesh and create a draw call for it
		for ( const auto& subMesh : a_StaticMesh->SubMeshes() )
		{
			// Use the override material if it exists, otherwise use the material from the source mesh
			SharedPtr<T::Material> material = subMesh.OverrideMaterial 
				? subMesh.OverrideMaterial 
				: a_StaticMesh->SourceMesh()->Materials().At( subMesh.SubMeshIndex );

			const MeshKey key{ 
				.MeshAsset = a_StaticMesh->ID(),
				.MaterialAsset = subMesh.OverrideMaterial ? subMesh.OverrideMaterial->ID() : ( AssetID )AssetID::InvalidID,
				.SubMeshIndex = subMesh.SubMeshIndex 
			};

			// Add the submesh to the geometry draw list
			{
				StaticDrawList& drawList = material->Transparent() ? m_StaticDrawLists.Transparent : m_StaticDrawLists.Opaque;
				auto& drawCall = drawList[ key ];
				drawCall.StaticMesh = a_StaticMesh;
				drawCall.InstanceCount++;
				drawCall.InstanceTransforms.PushBack( a_Transform );
			}

			// If the material casts shadows, add it to the shadow draw list
			if ( material->CastsShadows() )
			{
				auto& drawCall = m_StaticDrawLists.Shadow[ key ];
				drawCall.StaticMesh = a_StaticMesh;
				drawCall.InstanceCount++;
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

		if ( m_Viewport.Width > 0 && m_Viewport.Height > 0 )
		{
			auto gBufferPass = m_RenderGraph.AddPass( "GBuffer", ERHICommandQueueType::Graphics, [this]( RenderPassBuilder& a_Builder )
			{
				auto albedo = a_Builder.Create( "Albedo", RHITextureDesc{}
					.SetDimension( ERHITextureDimension::Texture2D )
					.SetWidth( m_Viewport.Width )
					.SetHeight( m_Viewport.Height )
					.SetFormat( ERHIFormat::RGBA8_UNORM )
					.SetBindFlags( ERHIBindFlags::RenderTarget )
					.SetClearValue( RHIClearValue{ Color::Green() } )
					.SetUseClearValue( true )
					.SetName( "GBuffer Albedo" ) );

				auto normal = a_Builder.Create( "Normal", RHITextureDesc{}
					.SetDimension( ERHITextureDimension::Texture2D )
					.SetWidth( m_Viewport.Width )
					.SetHeight( m_Viewport.Height )
					.SetFormat( ERHIFormat::RGBA8_UNORM )
					.SetBindFlags( ERHIBindFlags::RenderTarget )
					.SetName( "GBuffer Normal" ) );

				auto depth = a_Builder.Create( "Depth", RHITextureDesc{}
					.SetDimension( ERHITextureDimension::Texture2D )
					.SetWidth( m_Viewport.Width )
					.SetHeight( m_Viewport.Height )
					.SetFormat( ERHIFormat::D32_FLOAT )
					.SetBindFlags( ERHIBindFlags::DepthStencil )
					.SetName( "GBuffer Depth" ) );

				a_Builder.Write( albedo, ERHIResourceStates::RenderTarget );
				a_Builder.Write( normal, ERHIResourceStates::RenderTarget );
				a_Builder.Write( depth, ERHIResourceStates::DepthStencilWrite );

				a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
					{
						a_CommandList.PushDebugGroup( "Render Opaque Static Mesh" );

						// Do fullscreen copy/quad: sceneColor → backBuffer
						//static Array<uint8_t> whiteTexData;
						//whiteTexData.Resize( m_OutputTexture->Desc().Width * m_OutputTexture->Desc().Height * 4 );
						//static uint8_t color = 255; // White color (RGBA)
						//std::memset( whiteTexData.Data(), color++, whiteTexData.Size() ); // Fill with white color (RGBA)

						//auto data = RHITextureSubresourceData{
						//	.Data = whiteTexData.Data(),
						//	.RowStride = m_OutputTexture->Desc().Width * sizeof( uint32_t ) * 4, // 4 channels (RGBA)
						//	.DepthStride = 0
						//};

						//a_CommandList.UpdateTexture( *m_OutputTexture, RHITextureSlice::EntireTexture(), data );

						//auto sceneColor = a_Graph.GetTexture( SceneColorID );
						//printf( "SceneColor Texture ID: %s\n", sceneColor->Desc().Name.c_str() );

						StringView VertShaderCode = R"(
#include "Globals.hlsli"

struct Vertex
{
	float3 Position : Position;
};

struct VSOutput
{
    float4 pos : SV_Position;  
	float3 worldPos : WORLD_POSITION;
};

VSOutput VSMain( Vertex a_Vertex ) 
{
	VSOutput output;
	output.pos = float4( a_Vertex.Position.xy, 0.0, 1.0 ); // Simple pass-through position
	return output;
}
					)";

						StringView pixelCode = R"(
#include "Globals.hlsli"
struct PSInput
{
	float4 pos : SV_Position;
	float3 worldPos : WORLD_POSITION;
};

float4 PSMain( PSInput input ) : SV_Target
{
	return float4( 0.0, 0.0, 1.0, 1.0 ); // Simple world position to color
}
					)";

						static RHIShaderModuleRef vertShader = ShaderLibrary::Get()->LoadShader( VertShaderCode, "My vert shader", ERHIShaderType::Vertex );
						static RHIShaderModuleRef pixelShader = ShaderLibrary::Get()->LoadShader( pixelCode, "My pixel shader", ERHIShaderType::Pixel );

						struct Vertex
						{
							Vector3 Position;
						};
						constexpr RHIVertexLayout layout = RHIVertexLayout::From<Vertex>();

						RHIGraphicsPipelineStateDesc psd{};
						psd.Topology = ERHITopology::Triangle;
						psd.VertexShader = vertShader;
						psd.PixelShader = pixelShader;
						psd.FramebufferInfo = RHIFramebufferInfo{}.SetColorFormats( { ERHIFormat::RGBA8_UNORM } );
						psd.RasterizerState.CullMode = ERHICullMode::None;
						psd.RasterizerState.AnitaliasedLinesEnabled = false;
						psd.VertexLayout = layout;
						psd.Name = "My pipeline state";
						static RHIGraphicsPipelineStateRef pso = RHI::CreateGraphicsPipelineState( psd );

						static Array<Vertex> vertices = {
							{ { -0.5f, -0.5f, 0.0f } },
							{ {  0.5f, -0.5f, 0.0f } },
							{ {  0.0f,  0.5f, 0.0f } }
						};

						RHIBufferDesc vbDesc = RHIBufferDesc{}
							.SetSize( sizeof( Vertex ) * vertices.Size() )
							.SetBindFlags( ERHIBindFlags::VertexBuffer )
							.SetUsage( ERHIUsage::Static )
							.SetName( "Fullscreen Triangle VB" );
						static RHIBufferRef vertexBuffer = RHI::CreateBuffer( vbDesc, { ( Byte* )vertices.Data(), sizeof( Vertex ) * vertices.Size() } );

						auto graphicsState = RHIGraphicsState{}.SetPipelineState( pso.get() ).SetVertexBuffer( vertexBuffer.get() ).SetFramebuffer( RHIFramebuffer{}.AddColorAttachment( a_Graph.GetTexture( albedo ) ) );
						a_CommandList.SetGraphicsState( graphicsState );
						a_CommandList.ClearRenderTargets( ERHIClearFlags::Color, RHIClearValue{}.SetColor( { 0.0f, 1.0f, 0.0f, 1.0f } ) );

						// Set the viewport
						RHIViewportState viewportState{};
						viewportState.AddViewportAndScissor( RHIViewport( 0, 0, m_Viewport.Width, m_Viewport.Height ) );
						a_CommandList.SetViewportState( viewportState );

						a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 3 ) );

						a_CommandList.PopDebugGroup();
					}
				);
			} );

			m_RenderGraph.AddPass( "Lighting", ERHICommandQueueType::Graphics, [this]( RenderPassBuilder& a_Builder )
			{
				RenderPass* pass = a_Builder.GetPass( "GBuffer"_H );
				ASSERT( pass != nullptr, "GBuffer pass not found!" );

				// Create the lighting output (SceneColor)
				auto sceneColor = a_Builder.Create( "SceneColor",
											   RHITextureDesc{}
											   .SetDimension( ERHITextureDimension::Texture2D )
											   .SetWidth( m_Viewport.Width )
											   .SetHeight( m_Viewport.Height )
											   .SetFormat( ERHIFormat::RGBA16_FLOAT )
											   .SetBindFlags( ERHIBindFlags::RenderTarget )
											   .SetName( "Scene Color" ) );

				// Reads
				a_Builder.Read( pass->GetTextureID( "Albedo" ), ERHIResourceStates::ShaderResource);
				a_Builder.Read( pass->GetTextureID( "Normal" ), ERHIResourceStates::ShaderResource);
				a_Builder.Read( pass->GetTextureID( "Depth" ), ERHIResourceStates::DepthStencilRead );

				// Write the result into SceneColor
				a_Builder.Write( sceneColor, ERHIResourceStates::RenderTarget );

				// Execute
				a_Builder.Execute( [ = ]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
				{
					a_CommandList.PushDebugGroup( "Lighting" );
					// Bind gBuffer textures + do fullscreen pass into SceneColor
					a_CommandList.PopDebugGroup();
				} );
			} );

			m_RenderGraph.AddPass( "Present", ERHICommandQueueType::Graphics,
			[ this ]( RenderPassBuilder& a_Builder )
			{
				RenderPass* pass = a_Builder.GetPass( "GBuffer"_H );
				ASSERT( pass != nullptr, "GBuffer pass not found!" );

				// BackBuffer handle from swapchain
				auto backBuffer = a_Builder.Import( m_OutputTexture );

				// Read sceneColor (produced by Lighting)
				const auto albedoID = pass->GetTextureID( "Albedo" );
				a_Builder.Read( albedoID, ERHIResourceStates::ShaderResource );

				// Write to backbuffer
				a_Builder.Write( backBuffer, ERHIResourceStates::RenderTarget );

				// Blit / fullscreen draw
				a_Builder.Execute( [ = ]( IRHICommandList& a_CommandList, RenderGraph& a_Graph )
				{
					a_CommandList.PushDebugGroup( "Present" );

					// Do fullscreen copy/quad: sceneColor → backBuffer
					//static Array<uint8_t> whiteTexData;
					//whiteTexData.Resize( m_OutputTexture->Desc().Width * m_OutputTexture->Desc().Height * 4 );
					//static uint8_t color = 255; // White color (RGBA)
					//std::memset( whiteTexData.Data(), color++, whiteTexData.Size() ); // Fill with white color (RGBA)

					//auto data = RHITextureSubresourceData{
					//	.Data = whiteTexData.Data(),
					//	.RowStride = m_OutputTexture->Desc().Width * sizeof( uint32_t ) * 4, // 4 channels (RGBA)
					//	.DepthStride = 0
					//};

					//a_CommandList.UpdateTexture( *m_OutputTexture, RHITextureSlice::EntireTexture(), data );

					//auto sceneColor = a_Graph.GetTexture( SceneColorID );
					//printf( "SceneColor Texture ID: %s\n", sceneColor->Desc().Name.c_str() );

					StringView VertShaderCode = R"(
#include "Globals.hlsli"

struct InlinedConstants
{
	float4x4 PVM;
	float4x4 Model;
};

INLINED_CONSTANTS( inlinedConstants, InlinedConstants );

struct Vertex
{
	float3 Position : Position;
	float2 UV : UV;
};

struct VSOutput
{
    float4 pos : SV_Position;  
	float2 uv : TexCoord0;
};

VSOutput VSMain( Vertex a_Vertex ) 
{
	VSOutput output;
	output.pos = a_Vertex.Position.xyzz; // Simple pass-through position
	output.uv = a_Vertex.UV;
	return output;
}
					)";

					StringView pixelCode = R"(
#include "Globals.hlsli"
struct PSInput
{
	float4 pos : SV_Position;
	float2 uv : TexCoord0;
};

Texture2D Texture : register( t0 );
SamplerState TextureSampler : register( s0 );

float4 PSMain( PSInput input ) : SV_Target
{
	return Texture.Sample( TextureSampler, input.uv ).gbra; // Simple texture sample
}
					)";

					static RHIShaderModuleRef vertShader = ShaderLibrary::Get()->LoadShader( VertShaderCode, "dd", ERHIShaderType::Vertex );
					static RHIShaderModuleRef pixelShader = ShaderLibrary::Get()->LoadShader( pixelCode, "ddf", ERHIShaderType::Pixel );
					
					struct Vertex
					{
						Vector3 Position;
						Vector2 UV;
					};
					constexpr RHIVertexLayout layout = RHIVertexLayout::From<Vertex>();

					RHIBindingLayoutDesc sblDesc;
					sblDesc.Name = "My shader binding layout";
					sblDesc.Visibility = ERHIShaderVisibility::All;
					sblDesc.AddBinding( "Texture"_H ).AsTexture( 0 );
					static RHIBindingLayoutRef sbl = RHI::CreateBindingLayout( sblDesc );

					RHIGraphicsPipelineStateDesc psd{};
					psd.Topology = ERHITopology::Triangle;
					psd.VertexShader = vertShader;
					psd.PixelShader = pixelShader;
					psd.FramebufferInfo = RHIFramebufferInfo{}.SetColorFormats( { ERHIFormat::RGBA8_UNORM } );
					psd.RasterizerState.CullMode = ERHICullMode::None;
					psd.RasterizerState.AnitaliasedLinesEnabled = false;
					psd.VertexLayout = layout;
					psd.BindingLayouts.EmplaceBack( sbl );
					psd.Name = "My pipeline state";
					static RHIGraphicsPipelineStateRef pso = RHI::CreateGraphicsPipelineState( psd );

					// Quad
					static Array<Vertex> vertices = {
						{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f } },
						{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
						{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } },
						{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
						{ { 0.5f, 0.5f,   0.5f }, { 1.0f, 0.0f } },
						{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } }
					};

					RHIBufferDesc vbDesc = RHIBufferDesc{}
						.SetSize( sizeof( Vertex ) * vertices.Size() )
						.SetBindFlags( ERHIBindFlags::VertexBuffer )
						.SetUsage( ERHIUsage::Static )
						.SetName( "Fullscreen Triangle VB" );
					static RHIBufferRef vertexBuffer = RHI::CreateBuffer( vbDesc, { ( Byte* )vertices.Data(), sizeof( Vertex ) * vertices.Size() } );

					RHIBindingSetDesc bindingSetDesc{ sbl };
					auto sampler = RHISampler{}.SetAddressU( ERHISamplerAddressMode::Border )
						.SetAddressV( ERHISamplerAddressMode::Border )
						.SetAddressW( ERHISamplerAddressMode::Border )
						.SetFilter( ERHISamplerFilter::Anisotropic )
						.SetMaxAnisotropy( 16 )
						.SetBorderColor( Color( 1, 1, 0, 1.0f ) );

					bindingSetDesc.AddTexture( "Texture"_H, a_Graph.GetTexture( albedoID ).get(), &sampler );
					RHIBindingSetRef bindingSet = RHI::CreateBindingSet( bindingSetDesc );

					auto graphicsState = RHIGraphicsState{}
						.SetPipelineState( pso.get() )
						.SetVertexBuffer( vertexBuffer.get() )
						.AddBindingSet( bindingSet.get() )
						.SetFramebuffer( RHIFramebuffer{}.AddColorAttachment( m_OutputTexture ) );

					a_CommandList.SetGraphicsState( graphicsState );
					a_CommandList.ClearRenderTargets( ERHIClearFlags::Color, RHIClearValue{}.SetColor( { 0.0f, 0.0f, 1.0f, 1.0f } ) );

					// Set the viewport
					RHIViewportState viewportState{};
					viewportState.AddViewportAndScissor( RHIViewport( 0, 0, m_Viewport.Width, m_Viewport.Height ) );
					a_CommandList.SetViewportState( viewportState );

					a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 6 ) );

					a_CommandList.PopDebugGroup();
				} );
			} );

			m_RenderGraph.Compile();

			m_CommandList->Open();
			m_CommandList->PushDebugGroup( "SceneRenderer::FlushDrawLists" );
			{
				m_RenderGraph.Execute( *m_CommandList );
			}
			m_CommandList->PopDebugGroup();
			m_CommandList->Close();

			// Submit the command list to the graphics queue
			IRHICommandList* cmdList = m_CommandList.get();
			RHI::WaitForFence( ERHICommandQueueType::Graphics, RHI::ExecuteCommandLists( { &cmdList, 1 }, ERHICommandQueueType::Graphics ) );

			m_RenderGraph.ResetFrame();
		}

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