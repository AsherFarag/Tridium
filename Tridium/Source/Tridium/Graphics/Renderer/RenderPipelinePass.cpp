#include "tripch.h"
#include "RenderPipelinePass.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Graphics/Renderer/RenderPipeline.h>
#include <Tridium/Shaders/Families/GBuffer_ShaderInterop.h>
#include <Tridium/Shaders/Families/SkyBox_ShaderInterop.h>

// TEMP
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Math/Random.h>
#include <Tridium/Asset/Importers/EnvironmentMapImporter.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>

namespace Tridium {

	// TEMP!
	struct ViewportQuadVertex
	{
		Vector3 Position;
		Vector2 TexCoord;
	};

	//=================================================================================================
	// RootPass
	//=================================================================================================

	void RootPipelinePass::Setup( RenderGraph& a_RenderGraph )
	{
		a_RenderGraph.AddPass( "Root", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			//m_Output.first = a_Builder.Import( "Output", m_Output.second );
			//m_Depth.first = a_Builder.Create( "Depth", RHITextureDesc{}
			//	.SetDimension( ERHITextureDimension::Texture2D )
			//	.SetWidth( m_Output.second->Desc().Width )
			//	.SetHeight( m_Output.second->Desc().Height )
			//	.SetFormat( ERHIFormat::D32_FLOAT )
			//	.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
			//	.SetClearValue( RHIClearValue{} )
			//	.SetUseClearValue( true )
			//	.SetName( "Root-Pass Depth" ) );

			m_Output.first = a_Builder.Import( "Output", RHITextureRef{} );
			m_Depth.first = a_Builder.Import( "Depth", RHITextureRef{} );

			a_Builder.Write( m_Output.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Depth.first, ERHIResourceStates::DepthStencilWrite );

			a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, RenderViewID a_ViewID, const RenderView& a_View )
			{
				PROFILE_SCOPE( "RenderPass: Root", ProfilerCategory::Rendering );

				TODO( "Add clearing textures without graphics state" );

				m_Output.second = a_Context.GetViewOutput( a_ViewID );
				a_Graph.UpdateTexture( m_Output.first, m_Output.second );

				m_Depth.second = RHI::CreateTexture( RHITextureDesc{}
					.SetDimension( ERHITextureDimension::Texture2D )
					.SetWidth( m_Output.second->Desc().Width )
					.SetHeight( m_Output.second->Desc().Height )
					.SetFormat( ERHIFormat::D32_FLOAT )
					.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
					.SetClearValue( RHIClearValue{} )
					.SetUseClearValue( true )
					.SetName( "SceneRenderer Depth Texture" ) );

				a_Graph.UpdateTexture( m_Depth.first, m_Depth.second );

				a_CommandList.ClearTexture( *m_Output.second, {}, m_Output.second->Desc().ClearValue );
				a_CommandList.ClearTexture( *m_Depth.second, {}, m_Depth.second->Desc().ClearValue );
			} );
		} );
	}

	void RootPipelinePass::OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height )
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
		a_RenderGraph.UpdateTexture( m_Output.first, m_Output.second );

		const RHITextureDesc depthTextureDesc = RHITextureDesc{}
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( a_Width )
			.SetHeight( a_Height )
			.SetFormat( ERHIFormat::D32_FLOAT )
			.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
			.SetClearValue( RHIClearValue{} )
			.SetUseClearValue( true )
			.SetName( "SceneRenderer Depth Texture" );

		m_Depth.second = RHI::CreateTexture( depthTextureDesc );
		a_RenderGraph.UpdateTexture( m_Depth.first, m_Depth.second );
	}

	//=================================================================================================
	// GBufferPass
	//=================================================================================================

	void GBufferPipelinePass::Setup( RenderGraph& a_RenderGraph )
	{
		a_RenderGraph.AddPass( "GBuffer", ERHICommandQueueType::Graphics, [&]( RenderPassBuilder& a_Builder )
		{
			// Import GBuffer textures
			m_Position.first = a_Builder.Import( "Position", m_Position.second );
			m_Albedo.first = a_Builder.Import( "Albedo", m_Albedo.second );
			m_Normal.first = a_Builder.Import( "Normal", m_Normal.second );
			m_MetallicRoughnessAO.first = a_Builder.Import( "MetalRoughAO", m_MetallicRoughnessAO.second );
			m_Emission.first = a_Builder.Import( "Emission", m_Emission.second );
			m_Depth.first = a_Builder.GetPass( "Root"_H )->GetTextureID( "Depth" );

			// Declare resource usage
			a_Builder.Write( m_Position.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Albedo.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Normal.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_MetallicRoughnessAO.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Emission.first, ERHIResourceStates::RenderTarget );
			a_Builder.Write( m_Depth.first, ERHIResourceStates::DepthStencilWrite );

			a_Builder.Execute( [this]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, RenderViewID a_ViewID, const RenderView& a_View )
			{
				PROFILE_SCOPE( "RenderPass: Geometry Pass - StaticMesh", ProfilerCategory::Rendering );

				OnViewportResize( a_Graph, a_View.Constants.ViewportSize.X, a_View.Constants.ViewportSize.Y );

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

				const size_t materialBindingSetIndex = 0;

				// Clear GBuffer textures

				a_CommandList.ClearTexture( *positionTex, {}, positionTex->Desc().ClearValue );
				a_CommandList.ClearTexture( *albedoTex, {}, albedoTex->Desc().ClearValue );
				a_CommandList.ClearTexture( *normalTex, {}, normalTex->Desc().ClearValue );
				a_CommandList.ClearTexture( *metalRoughAmbientTex, {}, metalRoughAmbientTex->Desc().ClearValue );
				a_CommandList.ClearTexture( *emissionTex, {}, emissionTex->Desc().ClearValue );

				for ( const DrawPacket& drawPacket : a_Context.DrawPackets() )
				{
					for ( const DrawItem& drawItem : drawPacket.Items )
					{
						a_CommandList.SetGraphicsState( graphicsState
							.SetPipelineState( pipelineState.get() )
							.SetIndexBuffer( drawItem.IndexBuffer )
							.SetVertexBuffer( drawItem.VertexBuffer )
							.SetBindingSet( materialBindingSetIndex, drawItem.BindingSet )
						);

						a_CommandList.SetViewportState( RHIViewportState{}
							.AddViewportAndScissor( RHIViewport{ 0, 0, a_View.Constants.ViewportSize.X, a_View.Constants.ViewportSize.Y } )
						);

						InlinedConstants_GBuffer inlinedConstants{};
						inlinedConstants.PVM = a_View.Constants.ViewProjectionMatrix * drawItem.Transform;
						inlinedConstants.Model = drawItem.Transform;
						a_CommandList.SetInlinedConstants( inlinedConstants, 0 );

						a_CommandList.Draw( drawItem.DrawArgs );
					}
				}
			} );
		} );
	}

	void GBufferPipelinePass::OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height )
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

		// The resources have been recreated, so we need to update the render graph
		a_RenderGraph.UpdateTexture( m_Position.first, m_Position.second );
		a_RenderGraph.UpdateTexture( m_Albedo.first, m_Albedo.second );
		a_RenderGraph.UpdateTexture( m_Normal.first, m_Normal.second );
		a_RenderGraph.UpdateTexture( m_MetallicRoughnessAO.first, m_MetallicRoughnessAO.second );
		a_RenderGraph.UpdateTexture( m_Emission.first, m_Emission.second );
	}

	//=================================================================================================
	// SkyboxPass
	//=================================================================================================

	struct SkyboxVertex
	{
		Vector3 Position;
	};

	void SkyboxPipelinePass::Setup( RenderGraph& a_RenderGraph )
	{
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
			const auto* rootPass = GetRenderPipeline().GetRenderPass<RootPipelinePass>( IRenderPipeline::Passes::Root );
			ASSERT( rootPass, "Root pass must be present before Skybox pass." );

			m_Output = rootPass->GetOutputID();
			m_Depth = rootPass->GetDepthID();
			a_Builder.Read( m_Depth, ERHIResourceStates::DepthStencilRead );
			a_Builder.Write( m_Output, ERHIResourceStates::RenderTarget );

			a_Builder.Execute( [=]( IRHICommandList& a_CommandList, RenderGraph& a_Graph, const RenderContext& a_Context, RenderViewID a_ViewID, const RenderView& a_View )
			{
				PROFILE_SCOPE( "RenderPass: Skybox", ProfilerCategory::Rendering );

				const auto& lightingOutputTex = a_Graph.GetTexture( m_Output );
				const auto& depthTex = a_Graph.GetTexture( m_Depth );

				ASSERT( lightingOutputTex, "Lighting output texture is null in Skybox pass!" );
				ASSERT( depthTex, "Depth texture is null in Skybox pass!" );

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
						.SetColorFormats( { lightingOutputTex->Desc().Format } )
						.SetDepthStencilFormat( ERHIFormat::D32_FLOAT )
					);

				ShaderLibrary::GetOrCreateVariant( "SkyBox"_H )->Apply( psoDesc );

				m_PipelineState = PipelineStateCache::GetOrCreatePSO(
					psoDesc
				);

				RHIBindingSetRef s_BindingSet = [&]()
				{
					// Disable mipmap filtering for skybox
					const auto sampler = RHISampler{}
						.SetFilter( ERHISamplerFilter::MinMagMipLinear )
						.SetAddressU( ERHISamplerAddressMode::Clamp )
						.SetAddressV( ERHISamplerAddressMode::Clamp )
						.SetAddressW( ERHISamplerAddressMode::Clamp );

					auto bindingSetDesc = RHIBindingSetDesc{ this->m_PipelineState->Desc().BindingLayouts[0] };
					bindingSetDesc.AddTexture( "SkyboxMap"_H, a_Context.Lighting().Sky.EnvironmentMap.RadianceMap.get(), &sampler);
					return RHI::CreateBindingSet( bindingSetDesc );
				}( );

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
					.AddViewportAndScissor( RHIViewport{ 0, 0, a_View.Constants.ViewportSize.X, a_View.Constants.ViewportSize.Y } )
				);

				InlinedConstants_SkyBox constants;
				constants.Projection = a_View.Constants.ProjectionMatrix;
				constants.View = a_View.Constants.ViewMatrix;
				a_CommandList.SetInlinedConstants( constants, 0 );

				a_CommandList.Draw( RHIDrawArgs{}.SetVertexCount( 36 ).SetIndexCount( 36 ) );
			} );
		} );
	}

} // namespace Tridium