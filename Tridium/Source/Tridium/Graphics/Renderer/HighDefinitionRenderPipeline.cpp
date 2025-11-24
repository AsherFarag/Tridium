#include "tripch.h"
#include "HighDefinitionRenderPipeline.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Shaders/Families/GBuffer_ShaderInterop.h>
#include <Tridium/Shaders/Families/LitDefault_ShaderInterop.h>
#include <Tridium/Shaders/Families/SkyBox_ShaderInterop.h>

namespace Tridium {

	using Passes = HighDefinitionRenderPipeline::Passes;

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

		for ( const RenderView& view : a_Views.Shadows() )
		{
			RenderShadowMap( a_Context, view );
		}

		for ( const RenderView& view : a_Views.Cameras() )
		{
			m_RenderGraph.Execute( *m_GfxCmdList, a_Context, view );
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


			a_Builder.Execute( [=, this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, const RenderView& a_View )
			{
				PROFILE_SCOPE( "RenderPass: Lighting Pass", ProfilerCategory::Rendering );

				RHITextureRef directionalShadowMap;

				// Render directional shadow map
				if ( !a_Context.Lighting().DirectionalLightShadowMaps.empty() )
				{
					struct ShadowVertex
					{
						Vector3 POSITION;
					};

					auto shadowPSODesc = RHIGraphicsPipelineStateDesc{}
						.SetName( "Directional Light Shadow Map" )
						.SetVertexLayout( RHIVertexLayout::From<Vertex>() )
						.SetTopology( ERHITopology::Triangle )
						.SetRasterizerState( RHIRasterizerState{}.SetCullMode( ERHICullMode::Front ) )
						.SetDepthState( RHIDepthState{}.SetComparison( ERHIComparison::Less ) )
						.SetFramebufferInfo( RHIFramebufferInfo{}.SetDepthStencilFormat( ERHIFormat::D32_FLOAT ) );

					ShaderLibrary::GetOrCreateVariant( "Depth"_H )->Apply( shadowPSODesc );
					auto PSO = PipelineStateCache::GetOrCreatePSO( shadowPSODesc );

					a_CommandList.PushDebugGroup( "Render Directional Light Shadow Maps" );

					for ( const auto& [lightId, shadowMap] : a_Context.Lighting().DirectionalLightShadowMaps )
					{
						const DirectionalLight* dirLight = a_Context.GetDirectionalLight( lightId );
						if ( !dirLight || shadowMap == nullptr )
							continue;

						if ( directionalShadowMap )
							break;

						directionalShadowMap = shadowMap;

						auto graphicsState = RHIGraphicsState{}
							.SetFramebuffer( RHIFramebuffer{}.SetDepthStencilAttachment( shadowMap )  );

						const auto& shadowDesc = shadowMap->Desc();

						// 5. Clear depth
						a_CommandList.ClearTexture( *shadowMap, RHITextureSubresourceSet::All(), RHIClearValue{} );

						// 6. Draw shadow casters
						for ( const DrawPacket& drawPacket : a_Context.DrawPackets() )
						{
							for ( const DrawItem& drawItem : drawPacket.Items )
							{
								if ( !drawItem.PassTags.test( RenderPassTags::Shadow ) )
									continue;

								a_CommandList.SetGraphicsState( graphicsState
									.SetPipelineState( PSO.get() )
									.SetIndexBuffer( drawItem.IndexBuffer )
									.SetVertexBuffer( drawItem.VertexBuffer )
								);

								a_CommandList.SetViewportState( RHIViewportState{}
									.AddViewportAndScissor( RHIViewport{
										0, 0,
										(float)shadowDesc.Width,
										(float)shadowDesc.Height
									} )
								);

								// Simple orthographic bounds; in a real engine, calculate bounds from camera frustum
								const float orthoSize = 50.0f;
								const float nearPlane = -1000.f;
								const float farPlane = 1000.0f;

								Matrix4 lightView = glm::lookAtRH( glm::vec3( -dirLight->Direction * 50.0f ), glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) );
								Matrix4 lightProj = glm::orthoRH_ZO( -orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane );

								a_CommandList.SetInlinedConstants( ( lightProj* lightView ) * drawItem.Transform, 0 );

								a_CommandList.Draw( drawItem.DrawArgs );
							}
						}
					}

					a_CommandList.PopDebugGroup();
				}

				TODO( "This" );
				RHIBufferRef pointLightBuffer;
				if ( a_Context.Lighting().PointLights.Size() > 0 )
				{
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

				RHIBufferRef spotLightBuffer;
				if ( a_Context.Lighting().SpotLights.Size() > 0 )
				{
					RHIBufferDesc spotLightBufferDesc = RHIBufferDesc{}
						.SetName( "SceneRenderer Spot Light Buffer" )
						.SetType( ERHIBufferType::Structured )
						.SetHeapType( ERHIHeapType::Dynamic )
						.SetBindFlags( ERHIBindFlags::ShaderResource )
						.SetSize( sizeof( SpotLight ) * a_Context.Lighting().SpotLights.Size() )
						.SetStride( sizeof( SpotLight ) );
					spotLightBuffer = RHI::CreateBuffer( spotLightBufferDesc,
														 AsBytes( Span{ a_Context.Lighting().SpotLights.Data(),
																  a_Context.Lighting().SpotLights.Size() } ) );
				}

				auto lightingPassPSODesc = RHIGraphicsPipelineStateDesc{}
					.SetName( "SceneRenderer Lighting Pass Pipeline State" )
					.SetVertexLayout( RHIVertexLayout::From<ViewportQuadVertex>() )
					.SetTopology( ERHITopology::Triangle )
					.SetFramebufferInfo( RHIFramebufferInfo{}
						.SetColorFormats( { a_View.OutputTexture->Desc().Format } ) 
					);
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

				constexpr auto shadowMapSampler = RHISampler{}
					.SetFilter( ERHISamplerFilter::MinMagLinearMipPoint )
					.SetAddressU( ERHISamplerAddressMode::Border )
					.SetAddressV( ERHISamplerAddressMode::Border )
					.SetAddressW( ERHISamplerAddressMode::Border )
					.SetBorderColor( Color4::White() );

				auto bindingSetDesc = RHIBindingSetDesc{ PSO->Desc().BindingLayouts[0] }
					.AddConstantBuffer( "u_RenderView"_H, a_View.ConstantsBuffer.get() )
					.AddTexture( "PositionMap"_H, positionTex.get() )
					.AddTexture( "AlbedoMap"_H, albedoTex.get() )
					.AddTexture( "NormalMap"_H, normalTex.get() )
					.AddTexture( "MetalRoughAOMap"_H, metalRoughAOTex.get() )
					.AddTexture( "EmissionMap"_H, emissionTex.get() )
					.AddTexture( "DirectionalShadowMap"_H,
								 directionalShadowMap ? directionalShadowMap.get() : RenderResourceManager::GetWhiteTexture2D().get(),
								 &shadowMapSampler );

				if ( pointLightBuffer && pointLightBuffer->Valid() )
					bindingSetDesc.AddStructuredBuffer( "PointLights"_H, pointLightBuffer.get() );

				if ( spotLightBuffer && spotLightBuffer->Valid() )
					bindingSetDesc.AddStructuredBuffer( "SpotLights"_H, spotLightBuffer.get() );

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
				constants.NumPointLights = Cast<uint32_t>( a_Context.Lighting().PointLights.Size());
				constants.NumSpotLights = Cast<uint32_t>( a_Context.Lighting().SpotLights.Size() );

				// Directional Light
				if ( !a_Context.Lighting().DirectionalLights.Empty() )
				{
					constants.DirectionalLight = a_Context.Lighting().DirectionalLights[0];
				}
				else
				{
					constants.DirectionalLight.Intensity = 0.0f;
				}

				a_CommandList.SetInlinedConstants( constants, 0 );

				// Draw fullscreen quad
				a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 4 ).SetIndexCount( 6 ) );

			} );
		} );
	}

}