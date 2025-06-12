#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include <Tridium/Graphics/RHI/RHIUtil.h>
#include <pix.h>

#undef RHI_DEBUG_CMD_PARAM

#if RHI_DEBUG_ENABLE_CMD_RECORDING
	#define RHI_DEBUG_SRC_LOC_PARAM const SourceLocation& RHI_DEBUG_SRC_LOC
#else
	#define RHI_DEBUG_CMD_PARAM
#endif // RHI_DEBUG_ENABLE_CMD_RECORDING

namespace Tridium::D3D12 {

	inline constexpr size_t AlignUp( size_t a_Value, size_t a_Alignment )
	{
		return ( a_Value + a_Alignment - 1 ) & ~( a_Alignment - 1 );
	}

	RHICommandList_D3D12Impl::RHICommandList_D3D12Impl( const RHICommandListDesc& a_Desc )
		: IRHICommandList( a_Desc )
	{
		const HRESULT hr = GetD3D12RHI()->GetD3D12Device4()->CreateCommandList1( 
			0, D3D12::Translate( m_Desc.QueueType ), D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( CommandList.GetAddressOf() )
		);

		ENSURE( SUCCEEDED( hr ), "Failed to create command list!" );
	}

	bool RHICommandList_D3D12Impl::Release()
	{
		CommandList.Reset();
		return true;
	}

	void RHICommandList_D3D12Impl::CommitBarriers()
	{
		if ( m_ResourceStateTracker.ResourceBarriers.Size() == 0 )
		{
			return;
		}

		m_D3D12Barriers.Clear();
		m_D3D12Barriers.Reserve( m_ResourceStateTracker.ResourceBarriers.Size() );

		// Commit the resource barriers
		for ( const auto& barrier : m_ResourceStateTracker.ResourceBarriers )
		{
			const D3D12_RESOURCE_STATES before = D3D12::Translate( barrier.Before );
			const D3D12_RESOURCE_STATES after = D3D12::Translate( barrier.After );
			if ( before == after )
			{
				continue;
			}

			auto& d3d12Barrier = m_D3D12Barriers.EmplaceBack();
			d3d12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			d3d12Barrier.Transition.pResource = barrier.Resource->NativePtrAs<ID3D12Resource>();
			d3d12Barrier.Transition.StateBefore = before;
			d3d12Barrier.Transition.StateAfter = after;
			d3d12Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		}

		if ( m_D3D12Barriers.Size() > 0 )
			GraphicsCommandList()->ResourceBarrier( m_D3D12Barriers.Size(), m_D3D12Barriers.Data() );

		m_ResourceStateTracker.ResourceBarriers.Clear();
	}

	bool RHICommandList_D3D12Impl::IsCompleted() const
	{
		return GetD3D12RHI()->GetCommandContext( m_Desc.QueueType ).IsFenceComplete( FenceValue() );
	}

	void RHICommandList_D3D12Impl::WaitUntilCompleted()
	{
		GetD3D12RHI()->GetCommandContext( m_Desc.QueueType ).Wait( FenceValue() );
	}

	
	// --- Commands ---

	bool RHICommandList_D3D12Impl::Open() 
	{
		DynamicRHI_D3D12Impl* rhi = GetD3D12RHI();
		CHECK( rhi );

		const auto& cmdAllocator = rhi->GetCommandContext( ERHICommandQueueType::Graphics ).CmdAllocator;
		if ( FAILED( cmdAllocator->Reset() ) )
		{
			ASSERT( false, "Failed to reset command allocator!" );
			return false;
		}

		if ( FAILED( GraphicsCommandList()->Reset( cmdAllocator.Get(), nullptr ) ) )
		{
			ASSERT( false, "Failed to reset command list!" );
			return false;
		}

		return true;
	}

	bool RHICommandList_D3D12Impl::Close()
	{
		return true;
	}

	void RHICommandList_D3D12Impl::ClearState()
	{
		m_CurrentGraphicsState = {};
		m_GraphicsStateValid = false;
		m_ResourceStateTracker.Clear();
		m_ReferencedResources.Clear();
		m_D3D12Barriers.Clear();
	}

	void RHICommandList_D3D12Impl::ResourceBarrier( const RHIResourceBarrier& a_Barrier, RHI_DEBUG_SRC_LOC_PARAM )
	{
		m_ResourceStateTracker.ResourceBarriers.EmplaceBack( a_Barrier );
	}

	void RHICommandList_D3D12Impl::UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
	}

	void RHICommandList_D3D12Impl::CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
	}

	void RHICommandList_D3D12Impl::UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
	}

	void RHICommandList_D3D12Impl::CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
	}

	void RHICommandList_D3D12Impl::SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		const RootSignature* rootSig = nullptr;
		bool isGraphics = false;
		if ( m_CurrentGraphicsState.PipelineState )
		{
			rootSig = m_CurrentGraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>()->RootSignature.get();
			isGraphics = true;
		}

		if ( !rootSig || rootSig->InlinedConstantsSize == 0 )
		{
			RHI_DEV_CHECK( false, "Attempting to set inlined constants without a valid root signature or no inlined constants defined!" );
			return;
		}

		RHI_DEV_CHECK( a_SizeBytes <= (rootSig->InlinedConstantsSize - a_DstOffsetBytes), "Size of inlined constants exceeds the root signature size!" );

		if ( isGraphics )
		{
			GraphicsCommandList()->SetGraphicsRoot32BitConstants( 
				rootSig->RootParamInlinedConstants, 
				NumDWORDsFromBytes( a_SizeBytes ), 
				a_Data, 
				NumDWORDsFromBytes( a_DstOffsetBytes ) 
			);
		}
		else
		{
			GraphicsCommandList()->SetComputeRoot32BitConstants(
				rootSig->RootParamInlinedConstants, 
				NumDWORDsFromBytes( a_SizeBytes ), 
				a_Data, 
				NumDWORDsFromBytes( a_DstOffsetBytes ) 
			);
		}
	}

	void RHICommandList_D3D12Impl::SetGraphicsState( const RHIGraphicsState& a_GraphicsState, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::SetGraphicsState( a_GraphicsState, RHI_DEBUG_SRC_LOC );

		auto* pso = a_GraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>();
		RHI_DEV_CHECK( pso, "Invalid graphics pipeline state!" );

		const bool updateRootSig = !m_GraphicsStateValid 
			|| m_CurrentGraphicsState.PipelineState == nullptr
			|| m_CurrentGraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>()->RootSignature != pso->RootSignature;

		const bool updateFramebuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.Framebuffer != a_GraphicsState.Framebuffer;
		const bool updatePipelineState = !m_GraphicsStateValid || m_CurrentGraphicsState.PipelineState != a_GraphicsState.PipelineState;
		const bool updateIndexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.IndexBuffer != a_GraphicsState.IndexBuffer;
		const bool updateVertexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.VertexBuffer != a_GraphicsState.VertexBuffer;

		uint32_t bindingsUpdateMask = 0;
		if ( updateRootSig )
			bindingsUpdateMask = ~0;

		// Commit descriptor heaps TODO

		if ( bindingsUpdateMask == 0 )
			bindingsUpdateMask = RHIUtil::ArrayDifferenceMask( m_CurrentGraphicsState.BindingSets, a_GraphicsState.BindingSets );

		if ( updatePipelineState )
		{
			BindGraphicsPipelineState( pso, updateRootSig );
			m_ReferencedResources.EmplaceBack( pso->Shared() );
		}

		if ( updateFramebuffer )
		{
			BindFramebuffer( a_GraphicsState.Framebuffer );
			for ( IRHITexture* attachment : a_GraphicsState.Framebuffer.ColorAttachments )
				m_ReferencedResources.EmplaceBack( attachment->SharedFromThis() );
			if ( a_GraphicsState.Framebuffer.DepthStencilAttachment )
				m_ReferencedResources.EmplaceBack( a_GraphicsState.Framebuffer.DepthStencilAttachment->SharedFromThis() );
		}

		BindGraphicsBindings( a_GraphicsState.BindingSets, bindingsUpdateMask, pso->RootSignature );

		if ( updateIndexBuffer )
		{
			D3D12_INDEX_BUFFER_VIEW ibv{};
			if ( a_GraphicsState.IndexBuffer )
			{
				RHIBuffer_D3D12Impl* indexBuffer = a_GraphicsState.IndexBuffer->As<RHIBuffer_D3D12Impl>();

				if ( IsAutomaticResourceStateTransitionEnabled() )
				{
					m_ResourceStateTracker.RequireBufferState( *indexBuffer, ERHIResourceStates::IndexBuffer );
				}

				TODO( "Add support for index buffer offset" );
				ibv.BufferLocation = indexBuffer->ManagedBuffer.Resource->GetGPUVirtualAddress() /* + a_GraphicsState.IndexBufferOffset */;
				//ibv.SizeInBytes = indexBuffer->GetSizeInBytes() /* - a_GraphicsState.IndexBufferOffset */;
				ibv.Format = D3D12::Translate( indexBuffer->Desc().Format );
				RHI_DEV_CHECK( ibv.Format == DXGI_FORMAT_R16_UINT || ibv.Format == DXGI_FORMAT_R32_UINT, "Invalid index buffer format!" );
			}

			GraphicsCommandList()->IASetIndexBuffer( &ibv );
		}


		if ( updateVertexBuffer )
		{
			D3D12_VERTEX_BUFFER_VIEW vbv{};
			if ( a_GraphicsState.VertexBuffer )
			{
				RHIBuffer_D3D12Impl* vertexBuffer = a_GraphicsState.VertexBuffer->As<RHIBuffer_D3D12Impl>();
				if ( IsAutomaticResourceStateTransitionEnabled() )
				{
					m_ResourceStateTracker.RequireBufferState( *vertexBuffer, ERHIResourceStates::VertexBuffer );
				}

				TODO( "Add support for vertex buffer offset" );
				vbv.BufferLocation = vertexBuffer->ManagedBuffer.Resource->GetGPUVirtualAddress();
				//vbv.SizeInBytes = vertexBuffer->GetSizeInBytes();
				vbv.StrideInBytes = vertexBuffer->Desc().Stride;
			}

			GraphicsCommandList()->IASetVertexBuffers( 0, 1, &vbv );
		}

		CommitBarriers();

		m_CurrentGraphicsState = a_GraphicsState;
		m_GraphicsStateValid = true;
	}

	void RHICommandList_D3D12Impl::ClearRenderTargets( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue, uint8_t a_StencilValue, int32_t a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::ClearRenderTargets( a_Flags, a_ClearColor, a_DepthValue, a_StencilValue, a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for clearing render targets!" );
	}

	void RHICommandList_D3D12Impl::SetScissors( Span<const RHIScissorRect> a_Scissors, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetScissors( a_Scissors, RHI_DEBUG_SRC_LOC );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for setting scissors!" );
		RHI_DEV_CHECK( a_Scissors.size() <= RHIConstants::MaxScissorRects, "Too many scissor rectangles! - Only '{0}' are allowed.", RHIConstants::MaxScissorRects );

		D3D12_RECT scissors[RHIConstants::MaxScissorRects];
		for ( size_t i = 0; i < a_Scissors.size(); ++i )
		{
			const auto& scissor = a_Scissors[i];
			scissors[i] = D3D12_RECT{ 
				.left = scissor.Left,
				.top = scissor.Top,
				.right = scissor.Right,
				.bottom = scissor.Bottom
			};
		}

		GraphicsCommandList()->RSSetScissorRects( a_Scissors.size(), scissors );
	}

	void RHICommandList_D3D12Impl::SetViewports( Span<const RHIViewport> a_Viewports, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::SetViewports( a_Viewports, RHI_DEBUG_SRC_LOC );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for setting viewports!" );
		RHI_DEV_CHECK( a_Viewports.size() <= RHIConstants::MaxViewports, "Too many viewports! - Only '{0}' are allowed.", RHIConstants::MaxViewports );

		D3D12_VIEWPORT viewports[RHIConstants::MaxViewports];
		for ( size_t i = 0; i < a_Viewports.size(); ++i )
		{
			const auto& viewport = a_Viewports[i];
			viewports[i] = D3D12_VIEWPORT{ 
				.TopLeftX = viewport.X,
				.TopLeftY = viewport.Y,
				.Width = viewport.Width,
				.Height = viewport.Height,
				.MinDepth = viewport.MinDepth,
				.MaxDepth = viewport.MaxDepth
			};
		}

		GraphicsCommandList()->RSSetViewports( a_Viewports.size(), viewports );
	}

	void RHICommandList_D3D12Impl::Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::Draw( a_DrawArgs, RHI_DEBUG_SRC_LOC );
		TODO( "Validation" );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for drawing!" );
		RHI_DEV_CHECK( !a_DrawArgs.IsIndexed() || m_CurrentGraphicsState.IndexBuffer,
			"Cannot draw indexed without a valid index buffer!" );

		if ( a_DrawArgs.IsIndexed() )
		{
			GraphicsCommandList()->DrawIndexedInstanced( 
				a_DrawArgs.IndexCount, 
				a_DrawArgs.InstanceCount, 
				a_DrawArgs.BaseIndex, 
				a_DrawArgs.BaseVertex, 
				a_DrawArgs.BaseInstance
			);
		}
		else
		{
			// Draw non-indexed
			GraphicsCommandList()->DrawInstanced( 
				a_DrawArgs.VertexCount,
				a_DrawArgs.InstanceCount,
				a_DrawArgs.BaseVertex,
				a_DrawArgs.BaseInstance
			);
		}
	}

	void RHICommandList_D3D12Impl::PushDebugGroup( StringView a_Name )
	{
		PIXBeginEvent( GraphicsCommandList(), 0, a_Name.data() );
	}

	void RHICommandList_D3D12Impl::PopDebugGroup()
	{
		PIXEndEvent( GraphicsCommandList() );
	}

	void RHICommandList_D3D12Impl::InsertDebugMarker( StringView a_Name )
	{
		PIXSetMarker( GraphicsCommandList(), 0, a_Name.data() );
	}

	void RHICommandList_D3D12Impl::BindGraphicsPipelineState( RHIGraphicsPipelineState_D3D12Impl* a_PSO, bool a_UpdateRootSignature )
	{
		RHI_DEV_CHECK( a_PSO, "Invalid graphics pipeline state!" );

		if ( a_UpdateRootSignature )
		{
			ASSERT( a_PSO->RootSignature, "Graphics pipeline state does not have a valid root signature!" );
			GraphicsCommandList()->SetGraphicsRootSignature( a_PSO->RootSignature->D3D12Signature.Get() );
		}

		GraphicsCommandList()->SetPipelineState( a_PSO->PSO.Get() );
		GraphicsCommandList()->IASetPrimitiveTopology( D3D12::Translate( a_PSO->Desc().Topology ) );
	}

	void RHICommandList_D3D12Impl::BindFramebuffer( const RHIFramebuffer& a_Framebuffer )
	{

	}

	void RHICommandList_D3D12Impl::BindGraphicsBindings( 
		Span<IRHIBindingSet const* const> a_BindingSets, uint32_t a_UpdateMask, const SharedPtr<RootSignature>& a_RootSignature )
	{

	}


#if 0 // TODO: Remove this if not needed
	//////////////////////////////////////////////////////////////////////////

	void RHICommandList_D3D12Impl::SetShaderBindings( const RHICommand::SetShaderBindings& a_Cmd )
	{
		RHIBindingSet_D3D12Impl* bindingSet = a_Cmd.BindingSet->As<RHIBindingSet_D3D12Impl>();
		RHIBindingLayout_D3D12Impl* bindingLayout = bindingSet->Desc().Layout->As<RHIBindingLayout_D3D12Impl>();

		// Validate State
		for ( auto& binding : bindingSet->Desc().Bindings )
		{
			if ( binding.StateTransitionMode == ERHIStateTransition::None )
			{
				continue;
			}

			if ( binding.StateTransitionMode == ERHIStateTransition::Transition )
			{
				switch ( binding.Type )
				{
				case ERHIBindingType::InlinedConstants:
					break;
				case ERHIBindingType::ConstantBuffer:
					m_ResourceStateTracker.RequireBufferState(
						*binding.Resource->As<IRHIBuffer>(),
						ERHIResourceStates::ConstantBuffer );
					break;
				case ERHIBindingType::StructuredBuffer:
					m_ResourceStateTracker.RequireBufferState(
						*binding.Resource->As<IRHIBuffer>(),
						ERHIResourceStates::UnorderedAccess );
					break;
				case ERHIBindingType::StorageBuffer:
					m_ResourceStateTracker.RequireBufferState(
						*binding.Resource->As<IRHIBuffer>(),
						ERHIResourceStates::UnorderedAccess );
					break;
				case ERHIBindingType::Texture:
					m_ResourceStateTracker.RequireTextureState(
						*binding.Resource->As<IRHITexture>(),
						ERHIResourceStates::ShaderResource );
					break;
				case ERHIBindingType::StorageTexture:
					m_ResourceStateTracker.RequireTextureState(
						*binding.Resource->As<IRHITexture>(),
						ERHIResourceStates::UnorderedAccess );
					break;
				case ERHIBindingType::Sampler:
					// Samplers do not require a state transition
					break;
				case ERHIBindingType::CombinedSampler:
					m_ResourceStateTracker.RequireTextureState(
						*binding.Resource->As<IRHITexture>(),
						ERHIResourceStates::ShaderResource );
					break;
				default:
					ASSERT( false, "Unknown binding type!" );
					break;
				}
			}
		}
		CommitBarriers();

		for ( auto& binding : bindingSet->Desc().Bindings )
		{
			switch ( binding.Type )
			{
			case ERHIBindingType::InlinedConstants:
			{
				// This is handled by SetInlinedConstants
				break;
			}
			case ERHIBindingType::ConstantBuffer:
			{
				RHIBuffer_D3D12Impl* buffer = binding.Resource->As<RHIBuffer_D3D12Impl>();
				size_t bufferSize = binding.Range == RHIBufferRange::EntireBuffer() ? buffer->GetSizeInBytes() : binding.Range.Size;

				const D3D12::DescriptorHeapRef& cbvHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
					ERHIDescriptorHeapType::RenderResource,
					8,
					EDescriptorHeapFlags::Poolable | EDescriptorHeapFlags::GPUVisible,
					"CBV Heap" ) );

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
				cbvDesc.BufferLocation = buffer->ManagedBuffer.Resource->GetGPUVirtualAddress();
				cbvDesc.SizeInBytes = AlignUp( bufferSize, 256 ); // Constant buffers must be 256-byte aligned
				GetD3D12RHI()->GetD3D12Device()->CreateConstantBufferView(
					&cbvDesc,
					cbvHeap->GetCPUHandle( 0 )
				);

				break;
			}
			case ERHIBindingType::StructuredBuffer:
			{
				NOT_IMPLEMENTED;
				break;
			}
			case ERHIBindingType::StorageBuffer:
			{
				NOT_IMPLEMENTED;
				break;
			}
			case ERHIBindingType::Texture:
			{
				const DescriptorHeapRef& srvHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
					ERHIDescriptorHeapType::RenderResource,
					8,
					EDescriptorHeapFlags::Poolable,
					"SRV Heap" ) );

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				if ( false )
				{
					// Texture Array
					NOT_IMPLEMENTED;
				}
				else
				{
					// Single Texture
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = 0;
					srvDesc.Texture2D.MipLevels = 1;
					srvDesc.Texture2D.PlaneSlice = 0;
					srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

					const auto* tex = Cast<const RHITexture_D3D12Impl*>( binding.Resource );

					GetD3D12RHI()->GetD3D12Device()->CreateShaderResourceView(
						tex->Texture.Resource,
						&srvDesc,
						srvHeap->GetCPUHandle( 0 )
					);
				}

				//ID3D12DescriptorHeap* d3d12Heap = srvHeap->Heap();
				//GraphicsCommandList()->SetDescriptorHeaps( 1, &d3d12Heap );
				//GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Textures, srvHeap->GetGPUHandle( 0 ) );

				break;
			}
			case ERHIBindingType::StorageTexture:
			{
				NOT_IMPLEMENTED;
				break;
			}
			case ERHIBindingType::Sampler:
			{
				auto* sampler = binding.Resource->As<RHISampler_D3D12Impl>();
				NOT_IMPLEMENTED; // TODO: Handle sampler state
				//GraphicsCommandList()->SetDescriptorHeaps( 1, &sampler->SamplerHeap );
				//GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Samplers, sampler->SamplerHeap->GetGPUDescriptorHandleForHeapStart() );
				break;
			}
			case ERHIBindingType::CombinedSampler:
			{
				// Create the SRV Heap
				D3D12::DescriptorHeapRef srvHeap = m_State.Heaps.EmplaceBack( 
					GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
						ERHIDescriptorHeapType::RenderResource,
						8,
						EDescriptorHeapFlags::Poolable | EDescriptorHeapFlags::GPUVisible,
						"SRV Heap" ) 
				);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				auto* tex = binding.Resource->As<RHITexture_D3D12Impl>();

				if ( /*a_Cmd.Payload.Count > 1 */ false)
				{
					NOT_IMPLEMENTED;
				}
				else
				{
					// Single Texture
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = 0;
					srvDesc.Texture2D.MipLevels = 1;
					srvDesc.Texture2D.PlaneSlice = 0;
					srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

					GetD3D12RHI()->GetD3D12Device()->CreateShaderResourceView(
						tex->Texture.Resource,
						&srvDesc,
						srvHeap->GetCPUHandle( 0 )
					);

					// Retrieve the sampler from the texture
					const auto& sampler = tex->Sampler->Desc();

					// Create the Sampler Descriptor
					D3D12_SAMPLER_DESC samplerDesc{};
					samplerDesc.Filter = D3D12::Translate( sampler.Filter );
					samplerDesc.AddressU = D3D12::Translate( sampler.AddressU );
					samplerDesc.AddressV = D3D12::Translate( sampler.AddressV );
					samplerDesc.AddressW = D3D12::Translate( sampler.AddressW );
					samplerDesc.MipLODBias = sampler.MipLODBias;
					samplerDesc.MaxAnisotropy = sampler.MaxAnisotropy;
					samplerDesc.ComparisonFunc = D3D12::Translate( sampler.ComparisonFunc );
					samplerDesc.BorderColor[0] = sampler.BorderColor.r;
					samplerDesc.BorderColor[1] = sampler.BorderColor.g;
					samplerDesc.BorderColor[2] = sampler.BorderColor.b;
					samplerDesc.BorderColor[3] = sampler.BorderColor.a;
					samplerDesc.MinLOD = sampler.MinLOD;
					samplerDesc.MaxLOD = sampler.MaxLOD;

					// Create the Sampler Heap
					D3D12::DescriptorHeapRef samplerHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
						ERHIDescriptorHeapType::Sampler,
						1,
						EDescriptorHeapFlags::Poolable | EDescriptorHeapFlags::GPUVisible,
						"Sampler Heap" ) );

					GetD3D12RHI()->GetD3D12Device()->CreateSampler(
						&samplerDesc,
						samplerHeap->GetCPUHandle( 0 )
					);



					// Bind both SRV and Sampler heaps
					//ID3D12DescriptorHeap* heaps[] = { srvHeap->Heap(), samplerHeap->Heap() };
					//GraphicsCommandList()->SetDescriptorHeaps( 2, heaps );

					//GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Textures /*2*/, srvHeap->GetGPUHandle(0));
					//GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Samplers /*3*/, samplerHeap->GetGPUHandle(0));
				}

				break;
			}
			default:
			{
				ASSERT( false, "Unknown Shader Binding Type!" );
				break;
			}
			}
		}
	}

	void RHICommandList_D3D12Impl::SetInlinedConstants( const RHICommand::SetInlinedConstants& a_Cmd )
	{
	#if RHI_ENABLE_DEV_CHECKS
		// Check if the currently bound root signature matches supports the inlined constants
		TODO( "Check if the currently bound root signature supports inlined constants" );
	#endif

		GraphicsCommandList()->SetGraphicsRoot32BitConstants(
			RootParameters::Constants,
			NumDWORDsFromBytes( a_Cmd.Range.Size ),
			Cast<const void*>( a_Cmd.Data.Data() ),
			NumDWORDsFromBytes( a_Cmd.Range.Offset )
		);
	}

	void RHICommandList_D3D12Impl::ResourceBarrier( const RHICommand::ResourceBarrier& a_Cmd )
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = a_Cmd.Barrier.Resource->NativePtrAs<ID3D12Resource>();
		barrier.Transition.StateBefore = D3D12::Translate( a_Cmd.Barrier.Before );
		barrier.Transition.StateAfter = D3D12::Translate( a_Cmd.Barrier.After );
		barrier.Transition.Subresource = 0;
		GraphicsCommandList()->ResourceBarrier( 1, &barrier );

		// Update the resource state
		a_Cmd.Barrier.Resource->Type() == ERHIObjectType::Texture ?
			Cast<RHITexture_D3D12Impl*>( a_Cmd.Barrier.Resource )->SetState( a_Cmd.Barrier.After ) :
			Cast<RHIBuffer_D3D12Impl*>( a_Cmd.Barrier.Resource )->SetState( a_Cmd.Barrier.After );
	}

	void RHICommandList_D3D12Impl::UpdateBuffer( const RHICommand::UpdateBuffer& a_Cmd )
	{
		RHIBuffer_D3D12Impl* buffer = a_Cmd.Buffer->As<RHIBuffer_D3D12Impl>();

		TODO( "Reusing upload buffers" );
		auto& uploadBuffer = m_State.D3D12Resources.EmplaceBack();
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = a_Cmd.Data.size();
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			return;

		// Copy data to upload buffer
		char* uploadBufferAddress;
		D3D12_RANGE uploadRange = { 0, a_Cmd.Data.size() };
		uploadBuffer.Resource->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		std::memcpy( uploadBufferAddress, a_Cmd.Data.data(), a_Cmd.Data.size() );
		uploadBuffer.Resource->Unmap( 0, &uploadRange );

		// Check the buffer state
		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Buffer, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Buffer->State() == ERHIResourceStates::CopyDest, "Buffer state is not CopyDest!" );
		}

		CommitBarriers();

		GraphicsCommandList()->CopyBufferRegion(
			buffer->ManagedBuffer.Resource,
			a_Cmd.Offset,
			uploadBuffer.Resource,
			0,
			a_Cmd.Data.size()
		);
	}

	void RHICommandList_D3D12Impl::CopyBuffer( const RHICommand::CopyBuffer& a_Cmd )
	{
		// Check the source buffer state
		if ( a_Cmd.SrcStateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Source, ERHIResourceStates::CopySource );
		}
		else if ( a_Cmd.SrcStateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Source->State() == ERHIResourceStates::CopySource, "Source buffer state is not CopySource!" );
		}
		// Check the destination buffer state
		if ( a_Cmd.DstStateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Destination, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.DstStateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Destination->State() == ERHIResourceStates::CopyDest, "Destination buffer state is not CopyDest!" );
		}

		CommitBarriers();

		GraphicsCommandList()->CopyBufferRegion(
			a_Cmd.Destination->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource,
			a_Cmd.DestinationOffset,
			a_Cmd.Source->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource,
			a_Cmd.SourceOffset,
			a_Cmd.Size
		);
	}

	void RHICommandList_D3D12Impl::UpdateTexture( const RHICommand::UpdateTexture& a_Cmd )
	{
		TODO( "Handle different texture sizes" );
		if ( a_Cmd.Data.Data.empty() )
		{
			RHI_DEV_CHECK( false, "No data provided for texture update!" );
			return;
		}

		// Transition or validate resource state
		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.Texture, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Texture->State() == ERHIResourceStates::CopyDest, "Texture state is not CopyDest!" );
		}

		CommitBarriers();

		// Create an upload buffer
		D3D12::ManagedResource& uploadBuffer = m_State.D3D12Resources.EmplaceBack();
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC textureDesc = a_Cmd.Texture->As<RHITexture_D3D12Impl>()->Texture.Resource->GetDesc();

		UINT64 requiredSize = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;

		GetD3D12RHI()->GetD3D12Device()->GetCopyableFootprints(
			&textureDesc,
			a_Cmd.MipLevel,
			1,
			0,
			&footprint,
			&numRows,
			&rowSizeInBytes,
			&requiredSize
		);

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = requiredSize;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			return;

		// Copy data to upload buffer
		{
			char* uploadBufferAddress = nullptr;
			D3D12_RANGE mapRange = { 0, Cast<SIZE_T>(requiredSize) };
			uploadBuffer.Resource->Map( 0, &mapRange, ReinterpretCast<void**>(&uploadBufferAddress) );

			const uint8_t* srcData = a_Cmd.Data.Data.data();
			uint8_t* dstData = ReinterpretCast<uint8_t*>(uploadBufferAddress);
			// We copy row by row, slice by slice here because the GPU layout may be different from the CPU layout
			for ( uint32_t z = 0; z < footprint.Footprint.Depth; ++z )
			{
				for ( uint32_t y = 0; y < numRows; ++y )
				{
					std::memcpy(
						dstData + z * footprint.Footprint.RowPitch * numRows + y * footprint.Footprint.RowPitch,
						srcData + z * a_Cmd.Data.DepthStride + y * a_Cmd.Data.RowStride,
						std::min<size_t>( a_Cmd.Data.RowStride, footprint.Footprint.RowPitch )
					);
				}
			}

			uploadBuffer.Resource->Unmap( 0, nullptr );
		}

		// Setup copy locations
		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = a_Cmd.Texture->As<RHITexture_D3D12Impl>()->Texture.Resource;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = CalcSubresource( a_Cmd.MipLevel, a_Cmd.ArraySlice, 0, textureDesc.MipLevels, textureDesc.DepthOrArraySize );

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer.Resource;
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint = footprint;

		// Perform the copy
		GraphicsCommandList()->CopyTextureRegion(
			&dstLocation,
			a_Cmd.Region.MinX,
			a_Cmd.Region.MinY,
			a_Cmd.Region.MinZ,
			&srcLocation,
			nullptr // use full source footprint
		);
	}


	void RHICommandList_D3D12Impl::CopyTexture( const RHICommand::CopyTexture& a_Cmd )
	{
		// Check the source texture state
		if ( a_Cmd.SrcStateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.SrcTexture, ERHIResourceStates::CopySource );
		}
		else if ( a_Cmd.SrcStateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.SrcTexture->State() == ERHIResourceStates::CopySource, "Source texture state is not CopySource!" );
		}
		// Check the destination texture state
		if ( a_Cmd.DstStateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.DstTexture, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.DstStateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.DstTexture->State() == ERHIResourceStates::CopyDest, "Destination texture state is not CopyDest!" );
		}

		const bool success = a_Cmd.DstTexture->As<RHITexture_D3D12Impl>()->CopyTexture(
			*GraphicsCommandList(),
			*a_Cmd.SrcTexture->As<RHITexture_D3D12Impl>(),
			a_Cmd.SrcMipLevel,
			a_Cmd.SrcArraySlice,
			a_Cmd.SrcRegion,
			a_Cmd.DstMipLevel,
			a_Cmd.DstArraySlice,
			a_Cmd.DstRegion
		);

		ASSERT( success, "Failed to copy texture {0} to {1}!", a_Cmd.SrcTexture->Desc().Name, a_Cmd.DstTexture->Desc().Name );
	}

	void RHICommandList_D3D12Impl::SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Cmd )
	{
		m_State.Graphics.PSO = SharedPtrCast<RHIGraphicsPipelineState_D3D12Impl>( a_Cmd.PSO->shared_from_this() );
		GraphicsCommandList()->SetPipelineState( m_State.Graphics.PSO->PSO.Get() );
	}

	void RHICommandList_D3D12Impl::SetBindingLayout( const RHICommand::SetBindingLayout& a_Cmd )
	{
		CurrentSBL = a_Cmd.SBL;
		GraphicsCommandList()->SetGraphicsRootSignature( a_Cmd.SBL->As<RHIBindingLayout_D3D12Impl>()->m_RootSignature.Get() );
	}

	void RHICommandList_D3D12Impl::SetRenderTargets( const RHICommand::SetRenderTargets& a_Cmd )
	{
		if ( a_Cmd.RTV.Size() == 0 )
		{
			TODO( "Dodgy, bad logic" );
			GraphicsCommandList()->OMSetRenderTargets( 0, nullptr, FALSE, nullptr );
			m_State.LastRTVHeap = nullptr;
			m_State.LastDSVHeap = nullptr;
			return;
		}

		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			// Set the render target state for all RTVs and DSVs
			for ( IRHITexture* rtv : a_Cmd.RTV )
			{
				m_ResourceStateTracker.RequireTextureState( *rtv, ERHIResourceStates::RenderTarget );
			}
			if ( a_Cmd.DSV )
			{
				m_ResourceStateTracker.RequireTextureState( *a_Cmd.DSV, ERHIResourceStates::DepthStencilWrite );
			}
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			// Check the render target state for all RTVs and DSVs
			for ( IRHITexture* rtv : a_Cmd.RTV )
			{
				RHI_DEV_CHECK( rtv->State() == ERHIResourceStates::RenderTarget, "Render target state is not RenderTarget!" );
			}
			if ( a_Cmd.DSV )
			{
				RHI_DEV_CHECK( a_Cmd.DSV->State() == ERHIResourceStates::DepthStencilWrite, "Depth stencil state is not DepthStencilWrite!" );
			}
		}

		CommitBarriers();

		// Set the render targets
		m_State.Graphics.CurrentRTs.Clear();
		for ( IRHITexture* rtv : a_Cmd.RTV )
		{
			m_State.Graphics.CurrentRTs.EmplaceBack( rtv );
		}
		m_State.Graphics.CurrentDSV = a_Cmd.DSV;

		const auto& device = GetD3D12RHI()->GetD3D12Device();
		m_State.LastRTVHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
			ERHIDescriptorHeapType::RenderTarget,
			a_Cmd.RTV.Size(),
			EDescriptorHeapFlags::Poolable,
			"RTV Heap" ) 
		);

		// Add the RTVs to the rtv heap
		D3D12_CPU_DESCRIPTOR_HANDLE rtvs[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_Cmd.RTV.Size(); ++i )
		{
			device->CreateRenderTargetView( a_Cmd.RTV[i]->As<RHITexture_D3D12Impl>()->Texture.Resource, nullptr, m_State.LastRTVHeap->GetCPUHandle( i ) );
			rtvs[i] = m_State.LastRTVHeap->GetCPUHandle( i );
		}

		if ( a_Cmd.DSV )
		{
			if ( m_State.Graphics.PSO )
			{
				TODO( "Move this to a validation function before drawing?" );
				ASSERT( 
					m_State.Graphics.PSO->Desc().FramebufferInfo.DepthStencilFormat == a_Cmd.DSV->Desc().Format,
					"Depth stencil format of the set DSV does not match the currently bound PSO!"
				);
			}

			m_State.LastDSVHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
				ERHIDescriptorHeapType::DepthStencil,
				1,
				EDescriptorHeapFlags::Poolable,
				"DSV Heap" ) );

			device->CreateDepthStencilView( a_Cmd.DSV->As<RHITexture_D3D12Impl>()->Texture.Resource, nullptr, m_State.LastDSVHeap->GetCPUHandle( 0 ) );
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_State.LastDSVHeap->GetCPUHandle( 0 );
			GraphicsCommandList()->OMSetRenderTargets( a_Cmd.RTV.Size(), rtvs, false, &dsv );
		}
		else
		{
			GraphicsCommandList()->OMSetRenderTargets( a_Cmd.RTV.Size(), rtvs, false, nullptr );
			m_State.LastDSVHeap = nullptr;
		}
	}

	void RHICommandList_D3D12Impl::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			// Set the render target state for all RTVs and DSVs
			for ( IRHITexture* rtv : m_State.Graphics.CurrentRTs )
			{
				m_ResourceStateTracker.RequireTextureState( *rtv, ERHIResourceStates::RenderTarget );
			}
			if ( m_State.Graphics.CurrentDSV )
			{
				m_ResourceStateTracker.RequireTextureState( *m_State.Graphics.CurrentDSV, ERHIResourceStates::DepthStencilWrite );
			}
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			// Check the render target state for all RTVs and DSVs
			for ( IRHITexture* rtv : m_State.Graphics.CurrentRTs )
			{
				RHI_DEV_CHECK( rtv->State() == ERHIResourceStates::RenderTarget, "Render target state is not RenderTarget!" );
			}
			if ( m_State.Graphics.CurrentDSV )
			{
				RHI_DEV_CHECK( m_State.Graphics.CurrentDSV->State() == ERHIResourceStates::DepthStencilWrite, "Depth stencil state is not DepthStencilWrite!" );
			}
		}

		CommitBarriers();

		if ( a_Cmd.ClearFlags.HasFlag( ERHIClearFlags::Color ) && m_State.LastRTVHeap )
		{
			for ( size_t i = 0; i < m_State.LastRTVHeap->NumDescriptors(); ++i )
			{
				GraphicsCommandList()->ClearRenderTargetView(
					m_State.LastRTVHeap->GetCPUHandle( i ),
					&a_Cmd.ClearColorValues[i].r, 0, nullptr );
			}
		}

		// Clear the depth stencil view

		D3D12_CLEAR_FLAGS clearFlags = D3D12::Translate( a_Cmd.ClearFlags );
		if ( clearFlags != 0 && m_State.LastDSVHeap )
		{
			GraphicsCommandList()->ClearDepthStencilView(
				m_State.LastDSVHeap->GetCPUHandle( 0 ),
				clearFlags,
				a_Cmd.DepthValue, a_Cmd.StencilValue,
				0, nullptr );
		}
	}

	void RHICommandList_D3D12Impl::SetScissors( const RHICommand::SetScissors& a_Cmd )
	{
		TODO( "I just slapped a thread_local on this as I'm not sure if D3D12 needs me to keep the data around" );
		thread_local RECT rects[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_Cmd.Rects.Size(); ++i )
		{
			const auto& rect = a_Cmd.Rects[i];
			rects[i].left = rect.Left;
			rects[i].top = rect.Top;
			rects[i].right = rect.Right;
			rects[i].bottom = rect.Bottom;
		}

		GraphicsCommandList()->RSSetScissorRects( a_Cmd.Rects.Size(), rects );
	}

	void RHICommandList_D3D12Impl::SetViewports( const RHICommand::SetViewports& a_Cmd )
	{
		// Check the offsets of the Tridium RHIViewport and D3D12_VIEWPORT variables are the same so we can use a cast
		static_assert( sizeof( RHIViewport ) == sizeof( D3D12_VIEWPORT ) );
		static_assert( offsetof( RHIViewport, X ) == offsetof( D3D12_VIEWPORT, TopLeftX ) );
		static_assert( offsetof( RHIViewport, Y ) == offsetof( D3D12_VIEWPORT, TopLeftY ) );
		static_assert( offsetof( RHIViewport, Width ) == offsetof( D3D12_VIEWPORT, Width ) );
		static_assert( offsetof( RHIViewport, Height ) == offsetof( D3D12_VIEWPORT, Height ) );
		static_assert( offsetof( RHIViewport, MinDepth ) == offsetof( D3D12_VIEWPORT, MinDepth ) );
		static_assert( offsetof( RHIViewport, MaxDepth ) == offsetof( D3D12_VIEWPORT, MaxDepth ) );

		GraphicsCommandList()->RSSetViewports( a_Cmd.Viewports.Size(), ReinterpretCast<const D3D12_VIEWPORT*>( a_Cmd.Viewports.Data() ) );
	}

	void RHICommandList_D3D12Impl::SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.IBO, ERHIResourceStates::IndexBuffer );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.IBO->State() == ERHIResourceStates::IndexBuffer, "Index buffer state is not IndexBuffer!" );
		}

		CommitBarriers();

		D3D12_INDEX_BUFFER_VIEW ibv{};
		ibv.BufferLocation = a_Cmd.IBO->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource->GetGPUVirtualAddress();
		ibv.SizeInBytes = Cast<UINT>( a_Cmd.IBO->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource->GetDesc().Width );
		ibv.Format = D3D12::Translate( a_Cmd.IBO->Desc().Format );
		GraphicsCommandList()->IASetIndexBuffer( &ibv );
	}

	void RHICommandList_D3D12Impl::SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.VBO, ERHIResourceStates::VertexBuffer );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIStateTransition::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.VBO->State() == ERHIResourceStates::VertexBuffer, "Vertex buffer state is not VertexBuffer!" );
		}

		CommitBarriers();

		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = a_Cmd.VBO->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource->GetGPUVirtualAddress();
		vbv.SizeInBytes = Cast<UINT>( a_Cmd.VBO->As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource->GetDesc().Width );
		vbv.StrideInBytes = m_State.Graphics.PSO->Desc().VertexLayout.Stride;
		GraphicsCommandList()->IASetVertexBuffers( 0, 1, &vbv );
	}

	void RHICommandList_D3D12Impl::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Cmd )
	{
		GraphicsCommandList()->IASetPrimitiveTopology( D3D12::Translate( a_Cmd.Topology ) );
	}

	void RHICommandList_D3D12Impl::Draw( const RHICommand::Draw& a_Cmd )
	{
		// Set the descriptor heaps
		if ( m_State.Heaps.Size() > 0 )
		{
			TODO( "Make this a global constant" );
			constexpr size_t MaxHeaps = 32;
			ID3D12DescriptorHeap* heaps[MaxHeaps];
			for ( size_t i = 0; i < m_State.Heaps.Size(); ++i )
			{
				heaps[i] = m_State.Heaps[i]->Heap();
			}
			GraphicsCommandList()->SetDescriptorHeaps( Cast<UINT>( m_State.Heaps.Size() ), heaps );

			for ( size_t i = 0; i < m_State.Heaps.Size(); ++i )
			{
				const D3D12::DescriptorHeapRef& heapRef = m_State.Heaps[i];
				if ( heapRef->Type() == ERHIDescriptorHeapType::RenderResource
					|| heapRef->Type() == ERHIDescriptorHeapType::Sampler )
				{
				}
			}
		}

		GraphicsCommandList()->DrawInstanced( a_Cmd.VertexCount, 1, a_Cmd.VertexStart, 0 );
	}

	void RHICommandList_D3D12Impl::DrawIndexed( const RHICommand::DrawIndexed& a_Cmd )
	{
		NOT_IMPLEMENTED; // TODO: Handle indexed drawing
		GraphicsCommandList()->DrawIndexedInstanced( a_Cmd.IndexCount, 1, a_Cmd.IndexStart, 0, 0 );
	}

	void RHICommandList_D3D12Impl::SetComputePipelineState( const RHICommand::SetComputePipelineState& a_Cmd )
	{
	}

	void RHICommandList_D3D12Impl::DispatchCompute( const RHICommand::DispatchCompute& a_Cmd )
	{
	}

	void RHICommandList_D3D12Impl::DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect& a_Cmd )
	{
	}

#endif

} // namespace Tridium