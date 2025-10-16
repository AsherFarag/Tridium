#include "tripch.h"
#include "HighDefinitionRenderPipeline.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Shaders/Families/GBuffer_ShaderInterop.h>
#include <Tridium/Shaders/Families/LitDefault_ShaderInterop.h>
#include <Tridium/Shaders/Families/SkyBox_ShaderInterop.h>

// TEMP!
#include <Tridium/Shaders/Families/DebugLightCaster_ShaderInterop.h>

namespace Tridium {



	using Passes = HighDefinitionRenderPipeline::Passes;

	// TEMP!
	bool g_TestLightEnable = false;
	bool g_TestDrawLights = false;

	class TestLightDrawPipelinePass : public IRenderPipelinePass
	{
	public:
		RHIBufferRef m_CubeVertexBuffer;
		RHIBufferRef m_CubeIndexBuffer;
		RenderPassTextureID m_Output = RenderPassTextureID::Invalid;


		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override
		{

			if ( !m_CubeVertexBuffer )
			{
				constexpr Vector3 verts[] = {
					{ -1.0f, -1.0f,  1.0f }, // 0
					{  1.0f, -1.0f,  1.0f }, // 1
					{  1.0f,  1.0f,  1.0f }, // 2
					{ -1.0f,  1.0f,  1.0f }, // 3
					{ -1.0f, -1.0f, -1.0f }, // 4
					{  1.0f, -1.0f, -1.0f }, // 5
					{  1.0f,  1.0f, -1.0f }, // 6
					{ -1.0f,  1.0f, -1.0f }, // 7
				};

				constexpr uint16_t indices[] = {
					0, 1, 2, 2, 3, 0, // Front face
					1, 5, 6, 6, 2, 1, // Right face
					5, 4, 7, 7, 6, 5, // Back face
					4, 0, 3, 3, 7, 4, // Left face
					3, 2, 6, 6, 7, 3, // Top face
					4, 5, 1, 1, 0, 4  // Bottom face
				};

				m_CubeVertexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
														   .SetName( "Test Light Caster Cube Vertex Buffer" )
														   .SetType( ERHIBufferType::Structured )
														   .SetHeapType( ERHIHeapType::Default )
														   .SetBindFlags( ERHIBindFlags::VertexBuffer )
														   .SetSize( sizeof( verts ) )
														   .SetStride( sizeof( Vector3 ) ),
														   AsBytes( Span{ verts } ) );

				m_CubeIndexBuffer = RHI::CreateBuffer( RHIBufferDesc{}
													   .SetName( "Test Light Caster Cube Index Buffer" )
														  .SetType( ERHIBufferType::Formatted )
														  .SetHeapType( ERHIHeapType::Default )
														  .SetBindFlags( ERHIBindFlags::IndexBuffer )
														  .SetSize( sizeof( indices ) )
														  .SetFormat( ERHIFormat::R16_UINT ),
														  AsBytes( Span{ indices } ) );
			}



			a_RenderGraph.AddPass( "Debug Lights", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
			{
				GBufferPipelinePass* gbuffer = GetRenderPipeline().GetRenderPass<GBufferPipelinePass>( Passes::GBuffer );
				auto* rootPass = GetRenderPipeline().GetRenderPass<HDLightingPipelinePass>( Passes::Lighting );
				ASSERT( gbuffer && rootPass, "GBuffer and Root passes must be present before Lighting pass." );

				// Import GBuffer textures
				const auto depthID = gbuffer->GetDepthID();

				// Declare resource usage
				a_Builder.Read( depthID, ERHIResourceStates::ShaderResource );

				m_Output = rootPass->GetOutputID();
				a_Builder.Write( m_Output, ERHIResourceStates::RenderTarget );

				// Random lights for testing

				a_Builder.Execute( [=, this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, RenderViewID a_ViewID, const RenderView& a_View )
				{
					if ( !g_TestLightEnable || !g_TestDrawLights )
						return;
					PROFILE_SCOPE( "RenderPass: Lighting Pass", ProfilerCategory::Rendering );

					struct DebugLightCasterVertex
					{
						Vector3 Position;
					};

					const auto& depthTex = a_Graph.GetTexture( depthID );
					const auto& lightingOutputTex = a_Graph.GetTexture( m_Output );

					auto lightingPassPSODesc = RHIGraphicsPipelineStateDesc{}
						.SetName( "SceneRenderer Lighting Pass Pipeline State" )
						.SetVertexLayout( RHIVertexLayout::From<DebugLightCasterVertex>() )
						.SetTopology( ERHITopology::Triangle )
						.SetDepthState( RHIDepthState{}
							.SetDepthTestEnabled( true )
							.SetDepthWriteEnabled( true )
							.SetComparison( ERHIComparison::LessEqual ) )
						.SetFramebufferInfo( RHIFramebufferInfo{}
											 .SetColorFormats( { a_View.Camera.OutputFormat } )
											 .SetDepthStencilFormat( depthTex->Desc().Format ) );
					ShaderLibrary::GetOrCreateVariant( "DebugLightCaster"_H )->Apply( lightingPassPSODesc );

					auto PSO = PipelineStateCache::GetOrCreatePSO(
						lightingPassPSODesc
					);

					auto graphicsState = RHIGraphicsState{}
						.SetPipelineState( PSO.get() )
						.SetVertexBuffer( this->m_CubeVertexBuffer.get() )
						.SetIndexBuffer( this->m_CubeIndexBuffer.get() )
						.SetFramebuffer( RHIFramebuffer{}
							.AddColorAttachment( lightingOutputTex )
							.SetDepthStencilAttachment( depthTex, true )
						);

					a_CommandList.SetGraphicsState( graphicsState );
					a_CommandList.SetViewportState( RHIViewportState{}
						.AddViewportAndScissor( RHIViewport{ 0, 0,
												a_View.Constants.ViewportSize.X,
												a_View.Constants.ViewportSize.Y } )
					);

					for ( uint32_t i = 0; i < a_Context.Lighting().PointLights.Size(); i++ )
					{
						auto& light = a_Context.Lighting().PointLights[i];
						InlinedConstants_DebugLightCaster constants;
						constants.PVM = a_View.Constants.ViewProjectionMatrix * 
							Math::Translate( light.Position ) *
							Math::Scale( Vector3( 0.025f * light.Intensity ) );
						constants.Color = light.Color * light.Intensity;

						a_CommandList.SetInlinedConstants( constants, 0 );
						a_CommandList.Draw( RHIDrawArgs{}
							.SetIndexCount( 36 )
							.SetBaseIndex( 0 )
							.SetBaseVertex( 0 )
						);
					}

				} );
			} );
		}
	};

	// TEMP!
	struct ViewportQuadVertex
	{
		Vector3 Position;
		Vector2 TexCoord;
	};

	bool HighDefinitionRenderPipeline::Setup()
	{
		// Setup command list
		{
			const auto cmdListDesc = RHICommandListDesc{}
				.SetQueueType( ERHICommandQueueType::Graphics )
				.SetEnableImmediateExecution( true )
				.SetName( "HighDefinitionRenderPipeline Graphics Command List" );
			m_GfxCmdList = RHI::CreateCommandList( cmdListDesc );

			if ( !m_GfxCmdList || !m_GfxCmdList->Valid() )
			{
				LOG( LogCategory::Rendering, Error, "Failed to create HighDefinitionRenderPipeline graphics command list." );
				return false;
			}
		}

		// Setup render passes
		{
			auto* rootPass = AddRenderPass<RootPipelinePass>( String{ Passes::Root.String() } );
			auto* gbufferPass = AddRenderPass<GBufferPipelinePass>( String{ Passes::GBuffer.String() } );
			auto* lightingPass = AddRenderPass<HDLightingPipelinePass>( String{ Passes::Lighting.String() } );
			auto* skyboxPass = AddRenderPass<SkyboxPipelinePass>( String{ Passes::Skybox.String() } );
			
			// TEMP!
			auto* testLightPass = AddRenderPass<TestLightDrawPipelinePass>( "TestLightCaster" );
		}

		return true;
	}

	RHIFenceValue HighDefinitionRenderPipeline::Render( const RenderContext& a_Context, RenderViewList a_Views )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( m_RenderGraphRequiresRebuild )
		{
			BuildRenderGraph();
		}

		m_GfxCmdList->Open();
		m_GfxCmdList->PushDebugGroup( "HighDefinitionRenderPipeline Render" );

		for ( const auto& [id, view] : a_Views.Shadows() )
		{
			RenderShadowMap( a_Context, view );
		}

		for ( const auto& [id, view] : a_Views.Cameras() )
		{
			m_RenderGraph.Execute( *m_GfxCmdList, a_Context, id, view );
		}

		m_GfxCmdList->PopDebugGroup();
		m_GfxCmdList->Close();

		IRHICommandList* cmdLists[] = { m_GfxCmdList.get() };
		const RHIFenceValue fence = RHI::ExecuteCommandLists( cmdLists, 1, ERHICommandQueueType::Graphics );
		return fence;
	}

	//=============================================================================================
	// High-Definition Lighting Pass Implementation
	//=============================================================================================

	void HDLightingPipelinePass::Setup( RenderGraph& a_RenderGraph )
	{

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

		ASSERT( m_QuadVertexBuffer && m_QuadVertexBuffer->Valid(), "Lighting pass quad vertex buffer is not valid!" );
		ASSERT( m_QuadIndexBuffer && m_QuadIndexBuffer->Valid(), "Lighting pass quad index buffer is not valid!" );

		a_RenderGraph.AddPass( "Lighting", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			GBufferPipelinePass* gbuffer = GetRenderPipeline().GetRenderPass<GBufferPipelinePass>( Passes::GBuffer );
			RootPipelinePass* rootPass = GetRenderPipeline().GetRenderPass<RootPipelinePass>( Passes::Root );
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

			a_Builder.Execute( [=, this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, RenderViewID a_ViewID, const RenderView& a_View )
			{
				PROFILE_SCOPE( "RenderPass: Lighting Pass", ProfilerCategory::Rendering );

				static RHIBufferRef pointLightBuffer;
				if ( !pointLightBuffer || !pointLightBuffer->Valid() ||
					 pointLightBuffer->Desc().Size < sizeof( PointLight ) * a_Context.Lighting().PointLights.Size() )
				{
												// Create structured buffer for point lights
					RHIBufferDesc pointLightBufferDesc = RHIBufferDesc{}
						.SetName( "SceneRenderer Point Light Buffer" )
						.SetType( ERHIBufferType::Structured )
						.SetHeapType( ERHIHeapType::Dynamic )
						.SetBindFlags( ERHIBindFlags::ShaderResource )
						.SetSize( sizeof( PointLight ) * a_Context.Lighting().PointLights.Size() )
						.SetStride( sizeof( PointLight ) );

					pointLightBuffer = RHI::CreateBuffer( pointLightBufferDesc,
														  AsBytes( Span{ a_Context.Lighting().PointLights.Data(),
																   a_Context.Lighting().PointLights.Size() } ) );
				}

				auto lightingPassPSODesc = RHIGraphicsPipelineStateDesc{}
					.SetName( "SceneRenderer Lighting Pass Pipeline State" )
					.SetVertexLayout( RHIVertexLayout::From<ViewportQuadVertex>() )
					.SetTopology( ERHITopology::Triangle )
					.SetFramebufferInfo( RHIFramebufferInfo{}.SetColorFormats( { a_View.Camera.OutputFormat } ) );
				ShaderLibrary::GetOrCreateVariant( "LitDefault"_H, { "HIGH_QUALITY" } )->Apply( lightingPassPSODesc );

				auto PSO = PipelineStateCache::GetOrCreatePSO(
					lightingPassPSODesc
				);

				const auto& positionTex = a_Graph.GetTexture( positionID );
				const auto& albedoTex = a_Graph.GetTexture( albedoID );
				const auto& normalTex = a_Graph.GetTexture( normalID );
				const auto& metalRoughAOTex = a_Graph.GetTexture( metalRoughAOID );
				const auto& emissionTex = a_Graph.GetTexture( emissionID );
				const auto& depthTex = a_Graph.GetTexture( depthID );

				auto bindingSetDesc = RHIBindingSetDesc{ PSO->Desc().BindingLayouts[0] }
					.AddConstantBuffer( "u_RenderView"_H, a_View.ConstantsBuffer.get() )
					.AddTexture( "PositionMap"_H, positionTex.get() )
					.AddTexture( "AlbedoMap"_H, albedoTex.get() )
					.AddTexture( "NormalMap"_H, normalTex.get() )
					.AddTexture( "MetalRoughAOMap"_H, metalRoughAOTex.get() )
					.AddTexture( "EmissionMap"_H, emissionTex.get() )
					.AddStructuredBuffer( "PointLights"_H, pointLightBuffer.get() );

				if ( const auto& environmentMap = a_Context.Lighting().Sky.EnvironmentMap; environmentMap.Valid() )
				{
					bindingSetDesc.AddTexture( "IrradianceMap"_H, environmentMap.IrradianceMap.get() );
					bindingSetDesc.AddTexture( "RadianceMap"_H, environmentMap.RadianceMap.get() );
				}

				const auto bindingSet = RHI::CreateBindingSet( bindingSetDesc );

				const auto& lightingOutputTex = a_Graph.GetTexture( m_Output );

				auto graphicsState = RHIGraphicsState{}
					.SetPipelineState( PSO.get() )
					.SetVertexBuffer( this->m_QuadVertexBuffer.get() )
					.SetIndexBuffer( this->m_QuadIndexBuffer.get() )
					.SetBindingSet( 0, bindingSet.get() )
					.SetFramebuffer( RHIFramebuffer{}
						.AddColorAttachment( lightingOutputTex )
					);

				a_CommandList.SetGraphicsState( graphicsState );
				a_CommandList.SetViewportState( RHIViewportState{}
					.AddViewportAndScissor( RHIViewport{ 0, 0,
											a_View.Constants.ViewportSize.X,
											a_View.Constants.ViewportSize.Y } )
				);

				a_CommandList.ClearRenderTargets( ERHIClearFlags::Color, RHIClearValue{} );

				InlinedConstants_LitDefault constants;
				constants.NumPointLights = g_TestLightEnable * Cast<uint32_t>( a_Context.Lighting().PointLights.Size());

				// Directional Light
				{
					constants.DirectionalLight.Direction = Vector3{ -0.5f, -1.0f, -0.5f }.Normalized();
					constants.DirectionalLight.Color = Vector3{ 1.0f, 1.0f, 1.0f };
					constants.DirectionalLight.Intensity = 0.0f;
				}

				a_CommandList.SetInlinedConstants( constants, 0 );

				a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 4 ).SetIndexCount( 6 ) );

			} );
		} );
	}

}