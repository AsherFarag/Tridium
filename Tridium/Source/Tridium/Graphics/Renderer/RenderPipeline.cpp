#include "tripch.h"
#include "RenderPipeline.h"
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/PipelineStateCache.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>

namespace Tridium {

	//=============================================================================================
	// IRenderPipeline Implementation
	//=============================================================================================

	void IRenderPipeline::BuildRenderGraph()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		m_RenderGraph.ResetFrame();

		for ( auto& [name, pass] : m_RenderPasses )
		{
			pass->Setup( m_RenderGraph );
		}

		m_RenderGraph.Compile();

		m_RenderGraphRequiresRebuild = false;
	}

	void IRenderPipeline::RenderShadowMap( const RenderContext& a_Context, const RenderView& a_View )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		Culler culler{ a_Context, a_View.Constants };
	}

	//=============================================================================================
	// RenderPipelineManager Implementation
	//=============================================================================================

	IRenderPipeline* RenderPipelineManager::GetRenderPipeline( uint32_t a_FrameIndex )
	{
		if ( a_FrameIndex == ~0u )
		{
			a_FrameIndex = RendererModule::GetFrameIndex();
		}

		if ( !ASSERT( a_FrameIndex < m_RenderPipelines.Size(), "Frame index {} is out of bounds (max {})",
					  a_FrameIndex, m_RenderPipelines.Size() - 1 ) )
		{
			return nullptr;
		}

		return m_RenderPipelines.At( a_FrameIndex ).Pipeline.get();
	}

	bool RenderPipelineManager::SetRenderPipeline( UniquePtr<IRenderPipeline> a_Pipeline )
	{
		if ( a_Pipeline == nullptr )
		{
			return false;
		}

		// Wait for all previous frames to complete before changing the pipeline.
		RHI::WaitForIdle();

		Reset();

		// Create a render pipeline instance for each frame in flight.
		const uint32_t maxFramesInFlight = RHI::GetDynamicRHI()->MaxFramesInFlight();
		m_RenderPipelines.Resize( maxFramesInFlight );
		for ( uint32_t i = 0; i < maxFramesInFlight; ++i )
		{
			m_RenderPipelines[i].Pipeline = std::move( a_Pipeline->Create() );
			a_Pipeline = m_RenderPipelines[i].Pipeline->Create();
		}

		for ( RenderPipelineInFlight& pipelineInFlight : m_RenderPipelines )
		{
			if ( pipelineInFlight.Pipeline == nullptr || !pipelineInFlight.Pipeline->Setup() )
			{
				LOG( LogCategory::Rendering, Error, "Failed to set up render pipeline" );
				Reset();
				return false;
			}
		}

		return true;
	}

	RenderViewID RenderPipelineManager::AddView( RenderView a_View )
	{
		RenderViewID id{ m_NextViewID++ };
		m_ViewsNextFrame.EmplaceBack( id, std::move( a_View ) );
		return id;
	}

	RHIFenceValue RenderPipelineManager::Render()
	{
		RenderPipelineInFlight& pipelineInFlight = m_RenderPipelines[RendererModule::GetFrameIndex()];

		// We want to ensure this pipeline is free before rendering.
		RHI::WaitForFence( ERHICommandQueueType::Graphics, pipelineInFlight.FenceValue );
		
		const RenderViewList viewList{ .RawViews = m_Views };

		for ( auto& [viewID, view] : m_Views | std::views::filter( []( const auto& a_View ) { return a_View.second.Enabled; } ) )
		{
			// Create the constant buffer for this view if it doesn't already exist.
			if ( view.ConstantsBuffer == nullptr )
			{
				view.ConstantsBuffer = RHI::CreateBuffer( RHIBufferDesc{}
					.SetName( std::format( "ViewConstants_{}", view.Name ) )
					.SetSize( sizeof( RenderViewConstants ) )
					.SetHeapType( ERHIHeapType::Dynamic )
					.SetBindFlags( ERHIBindFlags::ConstantBuffer ),
					Span{ (const byte_t*)&view.Constants, sizeof( RenderViewConstants ) } );
			}

			// Create the output texture for this view if it doesn't already exist.
			auto texDesc = RHITextureDesc{}.SetName( std::format( "ViewOutput_{}", view.Name ) )
				.SetDimension( ERHITextureDimension::Texture2D )
				.SetWidth( view.Constants.ViewportSize.X )
				.SetHeight( view.Constants.ViewportSize.Y )
				.SetClearValue( RHIClearValue{} )
				.SetUseClearValue( true )
				.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource );

			if ( view.Type == ERenderViewType::Camera )
			{
				texDesc.SetFormat( view.Camera.OutputFormat );
			}
			else if ( view.Type == ERenderViewType::Shadow )
			{
				if ( view.Shadow.LightType == ELightType::Point )
				{
					texDesc.SetDimension( ERHITextureDimension::TextureCube );
					texDesc.SetArraySize( 6 );
				}

				texDesc.SetFormat( ERHIFormat::D32_FLOAT );
				texDesc.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource );
			}
			else
			{
				texDesc.SetFormat( ERHIFormat::RGBA8_UNORM );
			}

			m_RenderContext.m_ViewOutputs[viewID] = RHI::CreateTexture( texDesc );
		}

		pipelineInFlight.FenceValue = pipelineInFlight.Pipeline->Render( m_RenderContext, viewList );

		m_ViewOutputs = std::move( m_RenderContext.m_ViewOutputs );

		return pipelineInFlight.FenceValue;
	}

	void RenderPipelineManager::SubmitStaticMesh( AssetRef<StaticMesh> a_Mesh, const Matrix4& a_Transform )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		const RenderResourceStaticMesh meshResource = RenderResourceManager::GetOrCreateStaticMesh( a_Mesh );

		m_DrawPacketBuilder.Reserve( a_Mesh->SubMeshes().Size() );

		// Go through each submesh in the static mesh and create a draw call for it
		for ( size_t i = 0; i < a_Mesh->SubMeshes().Size(); i++ )
		{
			const SubMesh& subMesh = a_Mesh->SubMeshes()[i];

			// Use the override material if it exists, otherwise use the material from the source mesh
			AssetRef<Material> material = a_Mesh->GetMaterial( subMesh );
			const RenderResourceMaterial materialResource = RenderResourceManager::GetOrCreateMaterial( material );
			if ( !CHECK( materialResource.Valid(), "Material render resource is not valid for material '{}'", material->ID() ) )
			{
				continue;
			}

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

			RenderPassTagMask passTags{};
			passTags.set( material->Flags().HasFlag( EMaterialFlags::Transparent ) ? RenderPassTags::Transparent : RenderPassTags::Opaque );
			passTags.set( material->Flags().HasFlag( EMaterialFlags::DisableShadowCasting ) ? ( RenderPassTag )0 : RenderPassTags::Shadow );

			m_DrawPacketBuilder.AddDrawItem()
				.SetPassTags( passTags )
				.SetPipelineState( PipelineStateCache::GetOrCreatePSO( pipelineDesc ) )
				.SetBindingSet( materialResource.BindingSet )
				.SetVertexBuffer( meshResource.VertexBuffer )
				.SetIndexBuffer( meshResource.IndexBuffer )
				.SetTransform( a_Transform * subMesh.Transform )
				.SetDrawArgs( RHIDrawArgs{}
							  .SetBaseIndex( subMesh.BaseIndex )
							  .SetIndexCount( subMesh.IndexCount )
							  .SetBaseVertex( subMesh.BaseVertexIndex )
							  .SetVertexCount( subMesh.VertexCount ) );
		}

		SubmitDrawPacket( m_DrawPacketBuilder.Build() );
	}

	void RenderPipelineManager::BeginFrame()
	{
		// We are now rendering the views that between last frames 'BeginFrame' and this frames 'BeginFrame'.
		std::swap( m_Views, m_ViewsNextFrame );
		m_ViewsNextFrame.Clear();
	}

	void RenderPipelineManager::EndFrame()
	{
	}

	void RenderPipelineManager::Reset()
	{
		m_RenderPipelines.Clear();
		m_Views.Clear();
		m_RenderContext.m_DrawPackets.Clear();
		m_RenderContext.m_ViewOutputs.clear();
	}

} // namespace Tridium