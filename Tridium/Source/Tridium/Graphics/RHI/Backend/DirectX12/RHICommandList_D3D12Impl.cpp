#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include <Tridium/Graphics/RHI/RHIUtil.h>
#include <pix.h>

#undef RHI_DEBUG_CMD_PARAM
#define RHI_DEBUG_SRC_LOC_PARAM const SourceLocation& RHI_DEBUG_SRC_LOC

namespace Tridium::D3D12 {

	static void SetViewportAndScissors( ID3D12GraphicsCommandList* a_CmdList, const RHIViewportState& a_Viewports )
	{
		// Set viewports
		{
			D3D12_VIEWPORT viewports[RHIConstants::MaxViewports];
			for ( size_t i = 0; i < a_Viewports.Viewports.Size(); ++i )
			{
				const auto& viewport = a_Viewports.Viewports[i];
				viewports[i] = D3D12_VIEWPORT{
					.TopLeftX = viewport.X,
					.TopLeftY = viewport.Y,
					.Width = viewport.Width,
					.Height = viewport.Height,
					.MinDepth = viewport.MinDepth,
					.MaxDepth = viewport.MaxDepth
				};
			}
			a_CmdList->RSSetViewports( a_Viewports.Viewports.Size(), viewports );
		}
		// Set scissor rectangles
		{
			D3D12_RECT scissors[RHIConstants::MaxViewports];
			for ( size_t i = 0; i < a_Viewports.Scissors.Size(); ++i )
			{
				const auto& scissor = a_Viewports.Scissors[i];
				scissors[i] = D3D12_RECT{
					.left = scissor.Left,
					.top = scissor.Top,
					.right = scissor.Right,
					.bottom = scissor.Bottom
				};
			}
			a_CmdList->RSSetScissorRects( a_Viewports.Scissors.Size(), scissors );
		}
	}

	RHICommandList_D3D12Impl::RHICommandList_D3D12Impl( IDynamicRHI* a_Device, const RHICommandListDesc& a_Desc )
		: IRHICommandList( a_Device, a_Desc )
	{
		m_CmdQueue = Device()->GetCommandQueue( a_Desc.QueueType );
	}

	bool RHICommandList_D3D12Impl::Release()
	{
		m_CmdQueue = nullptr;
		m_ResourceStateTracker.Clear();
		m_D3D12Barriers.Clear();

		m_RTVHeap = nullptr;
		m_DSVHeap = nullptr;
		m_SRVUAVHeap = nullptr;
		m_SamplerHeap = nullptr;

		m_GraphicsStateValid = false;
		m_CurrentGraphicsState = {};

		m_CmdListPool.clear();
		m_ActiveCmdList = {};
		m_CmdContext = {};
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
			m_D3D12Barriers.EmplaceBack( Translate( barrier ) );

			if ( m_D3D12Barriers.Back().Transition.StateBefore == m_D3D12Barriers.Back().Transition.StateAfter )
			{
				m_D3D12Barriers.PopBack();
			}
		}

		if ( m_D3D12Barriers.Size() > 0 )
		{
			m_ActiveCmdList.CmdList->ResourceBarrier( m_D3D12Barriers.Size(), m_D3D12Barriers.Data() );
		}

		m_ResourceStateTracker.ResourceBarriers.Clear();
	}

	// --- Commands ---

	bool RHICommandList_D3D12Impl::Open()
	{
		IRHICommandList::Open();

		RHIFenceValue completedValue = m_CmdQueue->UpdateLastCompletedValue();

		// Set the active command list to an unused one from the pool or create a new one.

		m_ActiveCmdList = {};
		if ( !m_CmdListPool.empty()
			&& m_CmdListPool.front().LastSubmittedValue <= completedValue )
		{
			// Pop a command list from the pool if it is completed
			HRESULT hr = S_OK;
			m_ActiveCmdList = std::move( m_CmdListPool.front() );

			hr = m_ActiveCmdList.CmdAllocator->Reset();
			if ( FAILED( hr ) )
			{
				LOG( LogCategory::RHI, Error, "Failed to reset command allocator" );
				return false;
			}

			hr = m_ActiveCmdList.CmdList->Reset( m_ActiveCmdList.CmdAllocator.Get(), nullptr );
			if ( FAILED( hr ) )
			{
				LOG( LogCategory::RHI, Error, "Failed to reset command list" );
				return false;
			}

			m_CmdListPool.pop_front();
		}
		else
		{
			// Create a new command list
			const D3D12_COMMAND_LIST_TYPE d3dCmdListType = Translate( m_Desc.QueueType );
			RHI_DEV_CHECK( d3dCmdListType != D3D12_COMMAND_LIST_TYPE_NONE, "Invalid command list type!" );

			Device()->GetD3D12Device()->CreateCommandAllocator( d3dCmdListType, IID_PPV_ARGS( m_ActiveCmdList.CmdAllocator.GetAddressOf() ) );
			Device()->GetD3D12Device()->CreateCommandList(
				0, d3dCmdListType, m_ActiveCmdList.CmdAllocator.Get(), nullptr, IID_PPV_ARGS( m_ActiveCmdList.CmdList.GetAddressOf() )
			);
		}

		// Create the command context for this command list

		m_CmdContext = {};
		m_CmdContext.QueueType = m_Desc.QueueType;
		m_CmdContext.CmdAllocator = m_ActiveCmdList.CmdAllocator;
		m_CmdContext.CmdList = m_ActiveCmdList.CmdList;

		D3D12_SET_DEBUG_NAME( m_CmdContext.CmdList, m_Desc.Name, L"Unnamed Command List" );

		return true;
	}

	bool RHICommandList_D3D12Impl::Close()
	{
		IRHICommandList::Close();

		CommitBarriers();

		if ( FAILED( m_ActiveCmdList.CmdList->Close() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to close command list" );
			return false;
		}

		ClearState();

		return true;
	}

	void RHICommandList_D3D12Impl::ClearState()
	{
		m_CurrentGraphicsState = {};
		m_GraphicsStateValid = false;
		m_ResourceStateTracker.Clear();
		m_D3D12Barriers.Clear();
	}

	void RHICommandList_D3D12Impl::ResourceBarriers( Span<const RHIResourceBarrier> a_Barriers, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ResourceBarriers( a_Barriers, RHI_DEBUG_SRC_LOC );

		m_CmdContext.ReferencedResources.Reserve( m_CmdContext.ReferencedResources.Size() + a_Barriers.size() );
		for ( const auto& barrier : a_Barriers )
			m_CmdContext.ReferencedResources.EmplaceBack( barrier.Resource->Shared() );

		m_ResourceStateTracker.AddResourceBarriers( a_Barriers );
		CommitBarriers();
	}

	void RHICommandList_D3D12Impl::ClearTexture( IRHITexture& a_Texture, const RHITextureSubresourceSet& a_Subresources, RHIClearValue a_ClearValue, ERHIClearFlags a_ClearFlags, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ClearTexture( a_Texture, a_Subresources, a_ClearValue, a_ClearFlags, RHI_DEBUG_SRC_LOC );

		RHI_DEV_CHECK( a_Texture.Valid(), "Invalid texture!" );

		auto* texture = a_Texture.As<RHITexture_D3D12Impl>();

		const auto subresources = a_Subresources.Resolve( texture->Desc(), false );
		const bool isRenderTarget = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::RenderTarget );
		const bool isDepthStencil = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::DepthStencil );
		const bool isUAV          = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::UnorderedAccess );

		if ( isRenderTarget )
		{
			if ( IsAutomaticResourceStateTransitionEnabled() )
			{
				m_ResourceStateTracker.RequireTextureState( a_Texture, ERHIResourceStates::RenderTarget );
			}

			CommitBarriers();

			// Allocate a temporary RTV descriptor
			DescriptorHeapRef rtvHeap = AllocateHeap( ERHIDescriptorHeapType::RenderTarget, 1, EDescriptorHeapFlags::Poolable );
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUHandle( 0 );

			for ( uint32_t mipLevel = subresources.BaseMipLevel; mipLevel < subresources.BaseMipLevel + subresources.NumMipLevels; ++mipLevel )
			{
				// Create the RTV view into that descriptor slot
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.Format = GetDXGIFormatMap( texture->Desc().Format ).RTVFormat;
				if ( texture->Desc().IsArray() )
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.MipSlice = subresources.BaseMipLevel;
					rtvDesc.Texture2DArray.FirstArraySlice = subresources.BaseArraySlice;
					rtvDesc.Texture2DArray.ArraySize = subresources.NumArraySlices;
				}
				else
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D.MipSlice = subresources.BaseMipLevel;
					rtvDesc.Texture2D.PlaneSlice = 0;
				}

				auto* resource = texture->NativePtrAs<ID3D12Resource>();
				Device()->GetD3D12Device()->CreateRenderTargetView( resource, &rtvDesc, rtvHandle );

				m_ActiveCmdList.CmdList->ClearRenderTargetView( rtvHandle, &a_ClearValue.Color[0], 0, nullptr );
			}
		}
		else if ( isDepthStencil )
		{
			if ( !EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Depth ) &&
				 !EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Stencil ) )
			{
				RHI_DEV_WARN( false, "ClearTexture called with DepthStencil texture but no Depth or Stencil clear flag set." );
				return;
			}

			if ( IsAutomaticResourceStateTransitionEnabled() )
			{
				m_ResourceStateTracker.RequireTextureState( a_Texture, ERHIResourceStates::DepthStencilWrite );
			}

			CommitBarriers();

			// Allocate a temporary DSV descriptor
			DescriptorHeapRef dsvHeap = AllocateHeap( ERHIDescriptorHeapType::DepthStencil, 1, EDescriptorHeapFlags::Poolable );
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUHandle( 0 );

			// Create the DSV view into that descriptor slot
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = GetDXGIFormatMap( texture->Desc().Format ).RTVFormat;

			if ( texture->Desc().IsArray() )
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.MipSlice = subresources.BaseMipLevel;
				dsvDesc.Texture2DArray.FirstArraySlice = subresources.BaseArraySlice;
				dsvDesc.Texture2DArray.ArraySize = subresources.NumArraySlices;
			}
			else
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = subresources.BaseMipLevel;
			}

			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			if ( !EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Depth ) )
			{
				dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
			}
			if ( !EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Stencil ) )
			{
				dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;
			}

			auto* resource = texture->NativePtrAs<ID3D12Resource>();
			Device()->GetD3D12Device()->CreateDepthStencilView( resource, &dsvDesc, dsvHandle );

			const D3D12_CLEAR_FLAGS clearFlags = Translate( a_ClearFlags );
			m_ActiveCmdList.CmdList->ClearDepthStencilView( dsvHandle, clearFlags, a_ClearValue.Depth, (UINT8)a_ClearValue.Stencil, 0, nullptr );
		}
		else if ( isUAV )
		{
			if ( IsAutomaticResourceStateTransitionEnabled() )
			{
				m_ResourceStateTracker.RequireTextureState( a_Texture, ERHIResourceStates::UnorderedAccess );
			}

			CommitBarriers();

			NOT_IMPLEMENTED;
		}
		else
		{
			ASSERT( false, "Attempting to clear a texture that is not a Render Target, Depth Stencil or UAV!" );
			return;
		}

		// Keep a reference to the texture resource
		m_CmdContext.ReferencedResources.EmplaceBack( a_Texture.Shared() );
	}

	void RHICommandList_D3D12Impl::UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM )
	{
		RHIBuffer_D3D12Impl* buffer = a_Buffer.As<RHIBuffer_D3D12Impl>();

		// Create the upload buffer
		ComPtr<ID3D12Resource> uploadBuffer;
		{
			D3D12MA::Allocator* allocator = Device()->GetAllocator().Get();

			D3D12_RESOURCE_DESC uploadBufferDesc = {};
			uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			uploadBufferDesc.Width = a_DataSizeBytes;
			uploadBufferDesc.Height = 1;
			uploadBufferDesc.DepthOrArraySize = 1;
			uploadBufferDesc.MipLevels = 1;
			uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			uploadBufferDesc.SampleDesc.Count = 1;
			uploadBufferDesc.SampleDesc.Quality = 0;
			uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC allocDesc{};
			allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			ComPtr<D3D12MA::Allocation> uploadBufferAlloc;
			HRESULT hr = allocator->CreateResource(
				&allocDesc,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				uploadBufferAlloc.GetAddressOf(),
				IID_PPV_ARGS( uploadBuffer.GetAddressOf() )
			);

			if ( FAILED( hr ) )
			{
				ASSERT( false, "Failed to create upload buffer resource!" );
				return;
			}

			m_CmdContext.ReferencedUnknowns.EmplaceBack( std::move( uploadBufferAlloc ) );
		}

		// Copy data to upload buffer
		char* uploadBufferAddress;
		const auto uploadRange = D3D12_RANGE{ .Begin = 0, .End = a_DataSizeBytes };
		uploadBuffer->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		std::memcpy( uploadBufferAddress, a_Data, a_DataSizeBytes );
		uploadBuffer->Unmap( 0, &uploadRange );


		const ERHIResourceStates prevState = a_Buffer.State();
		if ( IsAutomaticResourceStateTransitionEnabled() )
			m_ResourceStateTracker.RequireBufferState( a_Buffer, ERHIResourceStates::CopyDest );

		CommitBarriers();

		m_ActiveCmdList.CmdList->CopyBufferRegion(
			buffer->ManagedBuffer.Resource(),
			a_DstOffsetBytes,
			uploadBuffer.Get(),
			0,
			a_DataSizeBytes
		);

		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			m_ResourceStateTracker.RequireBufferState( a_Buffer, prevState );
			CommitBarriers();
		}
	}

	void RHICommandList_D3D12Impl::CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::CopyBuffer( a_DstBuffer, a_DstOffsetBytes, a_SrcBuffer, a_SrcRange, RHI_DEBUG_SRC_LOC );

		const ERHIResourceStates prevDstBufferState = a_DstBuffer.State();
		const ERHIResourceStates prevSrcBufferState = a_SrcBuffer.State();
		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			m_ResourceStateTracker.RequireBufferState( a_DstBuffer, ERHIResourceStates::CopyDest );
			m_ResourceStateTracker.RequireBufferState( a_SrcBuffer, ERHIResourceStates::CopySource );
		}

		CommitBarriers();

		m_ActiveCmdList.CmdList->CopyBufferRegion(
			a_DstBuffer.As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource(),
			a_DstOffsetBytes,
			a_SrcBuffer.As<RHIBuffer_D3D12Impl>()->ManagedBuffer.Resource(),
			a_SrcRange.Offset,
			a_SrcRange.Size
		);

		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			m_ResourceStateTracker.RequireBufferState( a_DstBuffer, prevDstBufferState );
			m_ResourceStateTracker.RequireBufferState( a_SrcBuffer, prevSrcBufferState );
			CommitBarriers();
		}
	}

	void RHICommandList_D3D12Impl::UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::UpdateTexture( a_Texture, a_DstSlice, a_Data, RHI_DEBUG_SRC_LOC );
		TODO( "Handle different texture sizes" );
		RHI_DEV_CHECK( a_Data.Data != nullptr, "No data provided for texture update!" );

		if ( IsAutomaticResourceStateTransitionEnabled() )
			m_ResourceStateTracker.RequireTextureState( a_Texture, ERHIResourceStates::CopyDest );

		CommitBarriers();

		D3D12_RESOURCE_DESC textureDesc = a_Texture.NativePtrAs<ID3D12Resource>()->GetDesc();
		UINT64 requiredSize = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;

		GetD3D12RHI()->GetD3D12Device()->GetCopyableFootprints(
			&textureDesc,
			a_DstSlice.MipLevel,
			1,
			0,
			&footprint,
			&numRows,
			&rowSizeInBytes,
			&requiredSize
		);

		// Create an upload buffer
		ComPtr<ID3D12Resource> uploadBuffer;
		{
			D3D12MA::Allocator* allocator = Device()->GetAllocator().Get();

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

			D3D12MA::ALLOCATION_DESC allocDesc{};
			allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			ComPtr<D3D12MA::Allocation> uploadBufferAlloc;
			HRESULT hr = allocator->CreateResource(
				&allocDesc,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				uploadBufferAlloc.GetAddressOf(),
				IID_PPV_ARGS( uploadBuffer.GetAddressOf() )
			);

			if ( FAILED( hr ) )
			{
				ASSERT( false, "Failed to create upload buffer resource!" );
				return;
			}

			m_CmdContext.ReferencedUnknowns.EmplaceBack( std::move( uploadBufferAlloc ) );
		}

		// Copy data to upload buffer
		{
			char* uploadBufferAddress = nullptr;
			D3D12_RANGE mapRange = { 0, Cast<SIZE_T>( requiredSize ) };
			uploadBuffer->Map( 0, &mapRange, ReinterpretCast<void**>( &uploadBufferAddress ) );

			const uint8_t* srcData = Cast<const uint8_t*>( a_Data.Data );
			uint8_t* dstData = ReinterpretCast<uint8_t*>( uploadBufferAddress );
			// We copy row by row, slice by slice here because the GPU layout may be different from the CPU layout
			for ( uint32_t z = 0; z < footprint.Footprint.Depth; ++z )
			{
				for ( uint32_t y = 0; y < numRows; ++y )
				{
					std::memcpy(
						dstData + z * footprint.Footprint.RowPitch * numRows + y * footprint.Footprint.RowPitch,
						srcData + z * a_Data.DepthStride + y * a_Data.RowStride,
						Math::Min<size_t>( a_Data.RowStride, footprint.Footprint.RowPitch )
					);
				}
			}

			uploadBuffer->Unmap(0, nullptr);
		}

		// Setup copy locations
		D3D12_TEXTURE_COPY_LOCATION dstLocation{};
		dstLocation.pResource = a_Texture.NativePtrAs<ID3D12Resource>();
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = CalcSubresource( a_DstSlice.MipLevel, a_DstSlice.ArraySlice, 0, textureDesc.MipLevels, textureDesc.DepthOrArraySize );

		D3D12_TEXTURE_COPY_LOCATION srcLocation{};
		srcLocation.pResource = uploadBuffer.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint = footprint;

		// Perform the copy
		m_ActiveCmdList.CmdList->CopyTextureRegion(
			&dstLocation,
			a_DstSlice.OffsetX,
			a_DstSlice.OffsetY,
			a_DstSlice.OffsetZ,
			&srcLocation,
			nullptr // use full source footprint
		);
	}

	void RHICommandList_D3D12Impl::CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::CopyTexture( a_DstTexture, a_DstSlice, a_SrcTexture, a_SrcSlice, RHI_DEBUG_SRC_LOC );

		const auto dstSlice = a_DstSlice.Resolve( a_DstTexture.Desc() );
		const auto srcSlice = a_SrcSlice.Resolve( a_SrcTexture.Desc() );

		// State transitions
		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			m_ResourceStateTracker.RequireTextureState( a_DstTexture, ERHIResourceStates::CopyDest );
			m_ResourceStateTracker.RequireTextureState( a_SrcTexture, ERHIResourceStates::CopySource );
			CommitBarriers();
		}

		D3D12_TEXTURE_COPY_LOCATION dstLocation = a_DstTexture.As<RHITexture_D3D12Impl>()->CreateCopyLocation( dstSlice );
		D3D12_TEXTURE_COPY_LOCATION srcLocation = a_SrcTexture.As<RHITexture_D3D12Impl>()->CreateCopyLocation( srcSlice );

		D3D12_BOX srcBox{};
		srcBox.left = srcSlice.OffsetX;
		srcBox.top = srcSlice.OffsetY;
		srcBox.front = srcSlice.OffsetZ;
		srcBox.right = srcSlice.OffsetX + srcSlice.Width;
		srcBox.bottom = srcSlice.OffsetY + srcSlice.Height;
		srcBox.back = srcSlice.OffsetZ + srcSlice.Depth;

		m_ActiveCmdList.CmdList->CopyTextureRegion(
			&dstLocation,
			dstSlice.OffsetX,
			dstSlice.OffsetY,
			dstSlice.OffsetZ,
			&srcLocation,
			&srcBox
		);
	}

	void RHICommandList_D3D12Impl::SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM )
	{
		const RootSignature* rootSig = nullptr;
		bool isGraphics = false;
		if ( m_CurrentGraphicsState.PipelineState )
		{
			rootSig = m_CurrentGraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>()->RootSig.get();
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
			m_ActiveCmdList.CmdList->SetGraphicsRoot32BitConstants(
				rootSig->RootParamInlinedConstants,
				NumDWORDsFromBytes( a_SizeBytes ),
				a_Data,
				NumDWORDsFromBytes( a_DstOffsetBytes )
			);
		}
		else
		{
			m_ActiveCmdList.CmdList->SetComputeRoot32BitConstants(
				rootSig->RootParamInlinedConstants,
				NumDWORDsFromBytes( a_SizeBytes ),
				a_Data,
				NumDWORDsFromBytes( a_DstOffsetBytes )
			);
		}
	}

	void RHICommandList_D3D12Impl::SetGraphicsState( const RHIGraphicsState& a_GraphicsState, bool a_ClearViewportState, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetGraphicsState( a_GraphicsState, a_ClearViewportState, RHI_DEBUG_SRC_LOC );

		auto* pso = a_GraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>();
		RHI_DEV_CHECK( pso, "Invalid graphics pipeline state!" );

		const bool updateRootSig = !m_GraphicsStateValid
			|| m_CurrentGraphicsState.PipelineState == nullptr
			|| m_CurrentGraphicsState.PipelineState->As<RHIGraphicsPipelineState_D3D12Impl>()->RootSig != pso->RootSig;

		const bool updateFramebuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.Framebuffer != a_GraphicsState.Framebuffer;
		const bool updatePipelineState = !m_GraphicsStateValid || m_CurrentGraphicsState.PipelineState != a_GraphicsState.PipelineState;
		const bool updateIndexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.IndexBuffer != a_GraphicsState.IndexBuffer;
		const bool updateVertexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.VertexBuffer != a_GraphicsState.VertexBuffer;

		uint32_t bindingsUpdateMask = 0;
		if ( updateRootSig )
			bindingsUpdateMask = ~0;

		TODO( "Find a more efficient way for comitting descriptor heaps" );
		// Commit descriptor heaps if needed
		//{
		//	constexpr size_t c_TransientHeapSize = 128; // TODO: Adjust size as needed
		//	bool setDescriptorHeaps = false;

		//	if ( m_SRVUAVHeap == nullptr )
		//	{
		//		setDescriptorHeaps = true;
		//		m_SRVUAVHeap = AllocateHeap(
		//			ERHIDescriptorHeapType::RenderResource,
		//			c_TransientHeapSize,
		//			EDescriptorHeapFlags::GPUVisible,
		//			"CmdList RenderResource Transient Heap"
		//		).get();
		//		RHI_DEV_CHECK( m_SRVUAVHeap, "Failed to allocate transient descriptor heap!" );
		//	}

		//	if ( m_SamplerHeap == nullptr )
		//	{
		//		setDescriptorHeaps = true;
		//		m_SamplerHeap = AllocateHeap(
		//			ERHIDescriptorHeapType::Sampler,
		//			c_TransientHeapSize,
		//			EDescriptorHeapFlags::GPUVisible,
		//			"CmdList Transient Sampler Heap"
		//		).get();
		//		RHI_DEV_CHECK( m_SamplerHeap, "Failed to allocate transient sampler heap!" );
		//	}

		//	if ( setDescriptorHeaps )
		//	{
		//		ID3D12DescriptorHeap* heaps[2] = { m_SRVUAVHeap->Heap(), m_SamplerHeap->Heap() };
		//		m_ActiveCmdList.CmdList->SetDescriptorHeaps( 2, heaps );
		//		bindingsUpdateMask = ~0; // Force update of all bindings
		//	}
		//}


		if ( bindingsUpdateMask == 0 )
			bindingsUpdateMask = RHIUtil::ArrayDifferenceMask( m_CurrentGraphicsState.BindingSets, a_GraphicsState.BindingSets );

		m_CurrentGraphicsState = a_GraphicsState;

		if ( updatePipelineState )
		{
			BindGraphicsPipelineState( pso, updateRootSig );
			m_CmdContext.ReferencedResources.EmplaceBack( pso->Shared() );
		}

		if ( updateFramebuffer )
		{
			BindFramebuffer( a_GraphicsState.Framebuffer );
			for ( const auto& attachment : a_GraphicsState.Framebuffer.ColorAttachments )
				m_CmdContext.ReferencedResources.EmplaceBack( attachment.Texture->Shared() );
			if ( a_GraphicsState.Framebuffer.DepthStencilAttachment )
				m_CmdContext.ReferencedResources.EmplaceBack( a_GraphicsState.Framebuffer.DepthStencilAttachment.Texture->Shared() );

			// Setting the framebuffer clears the viewport state, so we need to re-apply it if needed
			if ( !a_ClearViewportState )
			{
				SetViewportAndScissors( m_ActiveCmdList.CmdList.Get(), m_ViewportState );
			}
		}

		BindGraphicsBindings( a_GraphicsState.BindingSets, bindingsUpdateMask, pso->RootSig );
		for ( const auto& bindingSet : a_GraphicsState.BindingSets )
		{
			if ( bindingSet )
				m_CmdContext.ReferencedResources.EmplaceBack( bindingSet->Shared() );
		}

		if ( updateIndexBuffer )
		{
			D3D12_INDEX_BUFFER_VIEW ibv{};
			if ( a_GraphicsState.IndexBuffer )
			{
				RHIBuffer_D3D12Impl* indexBuffer = a_GraphicsState.IndexBuffer->As<RHIBuffer_D3D12Impl>();
				TODO( "Add support for index buffer offset" );
				ibv.BufferLocation = indexBuffer->ManagedBuffer.Resource()->GetGPUVirtualAddress() /* + a_GraphicsState.IndexBufferOffset */;
				ibv.SizeInBytes = indexBuffer->ManagedBuffer.Resource()->GetDesc().Width /* - a_GraphicsState.IndexBufferOffset */;
				ibv.Format = GetDXGIFormatMap( indexBuffer->Desc().Format ).RTVFormat;
				RHI_DEV_CHECK( ibv.Format == DXGI_FORMAT_R16_UINT || ibv.Format == DXGI_FORMAT_R32_UINT, "Invalid index buffer format!" );

				m_CmdContext.ReferencedResources.EmplaceBack( indexBuffer->Shared() );
			}

			m_ActiveCmdList.CmdList->IASetIndexBuffer( &ibv );
		}


		if ( updateVertexBuffer )
		{
			D3D12_VERTEX_BUFFER_VIEW vbv{};
			if ( a_GraphicsState.VertexBuffer )
			{
				RHIBuffer_D3D12Impl* vertexBuffer = a_GraphicsState.VertexBuffer->As<RHIBuffer_D3D12Impl>();
				const uint32_t stride = vertexBuffer->Desc().Stride > 0
					? vertexBuffer->Desc().Stride
					: a_GraphicsState.PipelineState->Desc().VertexLayout.Stride; // Use PSO vertex layout stride if not specified
				TODO( "Add support for vertex buffer offset" );
				vbv.BufferLocation = vertexBuffer->ManagedBuffer.Resource()->GetGPUVirtualAddress();
				vbv.SizeInBytes = vertexBuffer->ManagedBuffer.Resource()->GetDesc().Width;
				vbv.StrideInBytes = stride;

				m_CmdContext.ReferencedResources.EmplaceBack( vertexBuffer->Shared() );
			}

			m_ActiveCmdList.CmdList->IASetVertexBuffers( 0, 1, &vbv );
		}

		// Set Resource States for resources referenced by the graphics state
		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			m_ResourceStateTracker.SetResourceStatesForFramebuffer( a_GraphicsState.Framebuffer );
			if ( a_GraphicsState.IndexBuffer )
				m_ResourceStateTracker.RequireBufferState( *a_GraphicsState.IndexBuffer, ERHIResourceStates::IndexBuffer );
			if ( a_GraphicsState.VertexBuffer )
				m_ResourceStateTracker.RequireBufferState( *a_GraphicsState.VertexBuffer, ERHIResourceStates::VertexBuffer );
		}


		CommitBarriers();

		m_GraphicsStateValid = true;
	}

	void RHICommandList_D3D12Impl::SetBindingSet( IRHIBindingSet& a_BindingSet, uint32_t a_LayoutIndex, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetBindingSet( a_BindingSet, a_LayoutIndex, RHI_DEBUG_SRC_LOC );

		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for setting binding sets!" );
		if ( !m_GraphicsStateValid )
			return;

		RHIGraphicsState& graphicsState = m_CurrentGraphicsState;
		RHIBindingSet_D3D12Impl* bindingSet = a_BindingSet.As<RHIBindingSet_D3D12Impl>();

		RHI_DEV_CHECK( graphicsState.BindingSets.IsValidIndex( a_LayoutIndex ), "Invalid binding set layout index '{0}'!", a_LayoutIndex );

		graphicsState.BindingSets[a_LayoutIndex] = bindingSet;
		m_CmdContext.ReferencedResources.EmplaceBack( bindingSet->Shared() );

		TODO( "We dont need to validate and update the entire graphics state here, just the binding set" );
		SetGraphicsState( m_CurrentGraphicsState, false, RHI_DEBUG_SRC_LOC );
	}

	void RHICommandList_D3D12Impl::ClearRenderTargets( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ClearRenderTargets( a_Flags, a_ClearValue, a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for clearing render targets!" );

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Color ) )
		{
			if ( a_ColorAttachmentIndex < 0 )
			{
				// Clear all color attachments
				for ( size_t i = 0; i < m_CurrentGraphicsState.Framebuffer.ColorAttachments.Size(); ++i )
				{
					auto& colorAttachment = m_CurrentGraphicsState.Framebuffer.ColorAttachments[i];
					D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUHandle( i );
					m_ActiveCmdList.CmdList->ClearRenderTargetView( rtvHandle, &a_ClearValue.Color[0], 0, nullptr);
				}
			}
			else
			{
				// Clear specific color attachment
				RHI_DEV_CHECK( m_CurrentGraphicsState.Framebuffer.ColorAttachments.IsValidIndex( a_ColorAttachmentIndex ),
					"Invalid color attachment index '{0}' for clearing render targets!", a_ColorAttachmentIndex );

				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUHandle( a_ColorAttachmentIndex );
				m_ActiveCmdList.CmdList->ClearRenderTargetView( rtvHandle, &a_ClearValue.Color[0], 0, nullptr );
			}
		}

		if ( m_CurrentGraphicsState.Framebuffer.DepthStencilAttachment )
		{
			D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAGS( 0 );
			if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Depth ) ) clearFlags |= D3D12_CLEAR_FLAG_DEPTH;
			if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Stencil ) ) clearFlags |= D3D12_CLEAR_FLAG_STENCIL;

			if ( clearFlags )
			{
				m_ActiveCmdList.CmdList->ClearDepthStencilView(
					m_DSVHeap->GetCPUHandle( 0 ),
					clearFlags,
					a_ClearValue.Depth,
					a_ClearValue.Stencil,
					0, nullptr
				);
			}
		}
	}

	void RHICommandList_D3D12Impl::SetViewportState( const RHIViewportState& a_Viewports, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetViewportState( a_Viewports, RHI_DEBUG_SRC_LOC );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for setting viewports!" );

		m_ViewportState = a_Viewports;
		SetViewportAndScissors( m_ActiveCmdList.CmdList.Get(), m_ViewportState );
	}

	void RHICommandList_D3D12Impl::Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::Draw( a_DrawArgs, RHI_DEBUG_SRC_LOC );
		TODO( "Validation" );
		RHI_DEV_CHECK( m_GraphicsStateValid, "Graphics state is not valid for drawing!" );
		RHI_DEV_CHECK( !a_DrawArgs.IsIndexed() || m_CurrentGraphicsState.IndexBuffer,
			"Cannot draw indexed without a valid index buffer!" );

		CommitBarriers();

		if ( a_DrawArgs.IsIndexed() )
		{
			m_ActiveCmdList.CmdList->DrawIndexedInstanced(
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
			m_ActiveCmdList.CmdList->DrawInstanced(
				a_DrawArgs.VertexCount,
				a_DrawArgs.InstanceCount,
				a_DrawArgs.BaseVertex,
				a_DrawArgs.BaseInstance
			);
		}
	}

	void RHICommandList_D3D12Impl::PushDebugGroup( StringView a_Name )
	{
		PIXBeginEvent( m_ActiveCmdList.CmdList.Get(), 0, a_Name.data());
	}

	void RHICommandList_D3D12Impl::PopDebugGroup()
	{
		PIXEndEvent( m_ActiveCmdList.CmdList.Get() );
	}

	void RHICommandList_D3D12Impl::InsertDebugMarker( StringView a_Name )
	{
		PIXSetMarker( m_ActiveCmdList.CmdList.Get(), 0, a_Name.data() );
	}

	CommandContext RHICommandList_D3D12Impl::ReleaseCmdContext( CommandQueue& a_CmdQueue )
	{
		CommandContext cmdContext = std::move( m_CmdContext );
		cmdContext.Fence = a_CmdQueue.Fence;
		cmdContext.SubmittedValue = a_CmdQueue.LastSubmittedValue;

		m_ActiveCmdList.LastSubmittedValue = a_CmdQueue.LastSubmittedValue;
		m_CmdListPool.emplace_back( std::move( m_ActiveCmdList ) );

		// Reset the active command context
		m_CmdContext = {};

		return cmdContext;
	}

	void RHICommandList_D3D12Impl::BindGraphicsPipelineState( RHIGraphicsPipelineState_D3D12Impl* a_PSO, bool a_UpdateRootSignature )
	{
		RHI_DEV_CHECK( a_PSO, "Invalid graphics pipeline state!" );

		if ( a_UpdateRootSignature )
		{
			ASSERT( a_PSO->RootSig, "Graphics pipeline state does not have a valid root signature!" );
			m_ActiveCmdList.CmdList->SetGraphicsRootSignature( a_PSO->RootSig->D3D12Signature.Get() );
		}

		m_ActiveCmdList.CmdList->SetPipelineState( a_PSO->PSO.Get() );
		m_ActiveCmdList.CmdList->IASetPrimitiveTopology( D3D12::Translate( a_PSO->Desc().Topology ) );
	}

	void RHICommandList_D3D12Impl::BindFramebuffer( const RHIFramebuffer& a_Framebuffer )
	{
		if ( m_AutomaticResourceStateTransitionEnabled )
		{
			m_ResourceStateTracker.SetResourceStatesForFramebuffer( a_Framebuffer );
		}

		CommitBarriers();

		ID3D12Device* const device = GetD3D12RHI()->GetD3D12Device();
		m_RTVHeap = AllocateHeap(
			ERHIDescriptorHeapType::RenderTarget,
			a_Framebuffer.ColorAttachments.Size(),
			EDescriptorHeapFlags::None,
			"RTV Heap"
		).get();
		ASSERT( m_RTVHeap, "Failed to allocate RTV heap!" );

		const auto CalculateRenderTargetView = []( const RHITextureDesc& a_Desc, const RHITextureSlice& a_Slice ) -> D3D12_RENDER_TARGET_VIEW_DESC
		{
			const auto slice = a_Slice.Resolve( a_Desc );
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = GetDXGIFormatMap( a_Desc.Format ).RTVFormat;
			switch ( a_Desc.Dimension )
			{
				case ERHITextureDimension::Texture2D:
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D.MipSlice = slice.MipLevel;
					rtvDesc.Texture2D.PlaneSlice = 0;
				}
				case ERHITextureDimension::TextureCube:
				{
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.MipSlice = slice.MipLevel;
					rtvDesc.Texture2DArray.FirstArraySlice = slice.ArraySlice;
					rtvDesc.Texture2DArray.ArraySize = 1; // For cube, we bind one face at a time
					break;
				}
				default:
				{
					TODO( "Implement these you idiot" );
					NOT_IMPLEMENTED;
					break;
				}
			}

			return rtvDesc;
		};

		const auto CalculateDepthStencilView = []( const RHITextureDesc& a_Desc, const RHITextureSlice& a_Slice ) -> D3D12_DEPTH_STENCIL_VIEW_DESC
		{
			const auto slice = a_Slice.Resolve( a_Desc );
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = GetDXGIFormatMap( a_Desc.Format ).RTVFormat;
			switch ( a_Desc.Dimension )
			{
				case ERHITextureDimension::Texture2D:
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
					dsvDesc.Texture2D.MipSlice = slice.MipLevel;
					break;
				}
				case ERHITextureDimension::TextureCube:
				{
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					dsvDesc.Texture2DArray.MipSlice = slice.MipLevel;
					dsvDesc.Texture2DArray.FirstArraySlice = slice.ArraySlice;
					dsvDesc.Texture2DArray.ArraySize = 1; // For cube, we bind one face at a time
					break;
				}
				default:
				{
					TODO( "Implement these you idiot" );
					NOT_IMPLEMENTED;
					break;
				}
			}
			return dsvDesc;
		};

		// Create RTVs for color attachments
		D3D12_CPU_DESCRIPTOR_HANDLE rtvs[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_Framebuffer.ColorAttachments.Size(); ++i )
		{
			auto* tex = a_Framebuffer.ColorAttachments[i].Texture->As<RHITexture_D3D12Impl>();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = CalculateRenderTargetView( tex->Desc(), a_Framebuffer.ColorAttachments[i].Slice );
			device->CreateRenderTargetView( tex->Texture.Resource(), &rtvDesc, m_RTVHeap->GetCPUHandle(i));
			rtvs[i] = m_RTVHeap->GetCPUHandle( i );
		}

		if ( a_Framebuffer.DepthStencilAttachment )
		{
			if ( m_CurrentGraphicsState.PipelineState )
			{
				ASSERT(
					m_CurrentGraphicsState.PipelineState->Desc().FramebufferInfo.DepthStencilFormat == a_Framebuffer.DepthStencilAttachment.Texture->Desc().Format,
					"Depth stencil format of the set DSV does not match the currently bound PSO!"
				);
			}

			m_DSVHeap = AllocateHeap(
				ERHIDescriptorHeapType::DepthStencil,
				1,
				EDescriptorHeapFlags::None,
				"DSV Heap"
			).get();
			ASSERT( m_DSVHeap, "Failed to allocate DSV heap!" );

			auto* depthTex = a_Framebuffer.DepthStencilAttachment.Texture->As<RHITexture_D3D12Impl>();
			TODO( "Handle texture slices for dsv" );
			//const auto viewDesc = CalculateDepthStencilView( depthTex->Desc(), a_Framebuffer.DepthStencilAttachment.Slice );
			device->CreateDepthStencilView( depthTex->Texture.Resource(), nullptr, m_DSVHeap->GetCPUHandle(0));
			const D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_DSVHeap->GetCPUHandle( 0 );
			m_ActiveCmdList.CmdList->OMSetRenderTargets( a_Framebuffer.ColorAttachments.Size(), rtvs, false, &dsv );
		}
		else
		{
			m_ActiveCmdList.CmdList->OMSetRenderTargets( a_Framebuffer.ColorAttachments.Size(), rtvs, false, nullptr );
			m_DSVHeap = nullptr;
		}
	}

	void RHICommandList_D3D12Impl::BindGraphicsBindings(
		Span<IRHIBindingSet const* const> a_BindingSets,
		uint32_t a_UpdateMask,
		const SharedPtr<RootSignature>& a_RootSignature )
	{
		if ( a_BindingSets.empty() )
			return;

		// We will collect the descriptor heaps pointer array to set on the command list.
		// Only up to 2 heap types are needed: CBV_SRV_UAV (render resources) and SAMPLER.
		m_ActiveDescriptorHeaps.Clear();
		m_ActiveDescriptorHeaps.Reserve( a_BindingSets.size() * 2 );

		static thread_local UnorderedMap<ID3D12DescriptorHeap*, Pair<RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE>> rootParamsSet;
		rootParamsSet.clear();
		rootParamsSet.reserve( a_BindingSets.size() * 2 );

		// Iterate binding sets that will be updated and prepare/copy transient descriptors as necessary
		for ( size_t i = 0; i < a_BindingSets.size(); ++i )
		{
			if ( !a_BindingSets[i] || !a_BindingSets[i]->Valid() )
				continue;

			// Check update mask
			if ( ( a_UpdateMask & ( 1u << i ) ) == 0 )
				continue;

			const RHIBindingSet_D3D12Impl* bindingSet = a_BindingSets[i]->As<RHIBindingSet_D3D12Impl>();
			RHIBindingLayout_D3D12Impl* bindingLayout = bindingSet->Desc().Layout->As<RHIBindingLayout_D3D12Impl>();
			RootParameterIndex rootParamOffset = a_RootSignature->Layouts.At( i ).second;

			// ---------- SAMPLERS ----------
			if ( bindingSet->SamplerHeap )
			{
				const auto& transient = AllocateHeap(
					ERHIDescriptorHeapType::Sampler,
					bindingLayout->DescriptorTableSizeSamplers,
					EDescriptorHeapFlags::GPUVisible
				);

				// Copy descriptors: src = bindingSet->SamplerHeap (CPU), dst = transient (GPU-visible)
				Device()->GetD3D12Device()->CopyDescriptorsSimple(
					UINT( bindingLayout->DescriptorTableSizeSamplers ),
					transient->GetCPUHandle( 0 ), // destination CPU handle for GPU-visible heap
					bindingSet->SamplerHeap->GetCPUHandle( 0 ), // source CPU handle
					D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
				);

				// Set the transient heap and root table
				m_ActiveDescriptorHeaps.PushBack( transient->Heap() );
				rootParamsSet[transient->Heap()] = { rootParamOffset + bindingLayout->RootParamSamplers, transient->GetGPUHandle( 0 ) };
			}

			// ---------- RENDER RESOURCES (CBV/SRV/UAV) ----------
			if ( bindingSet->RenderResourceHeap )
			{
				{
					// Allocate a transient GPU-visible range for CBV/SRV/UAV descriptors.
					const auto& transient = AllocateHeap(
						ERHIDescriptorHeapType::RenderResource,
						bindingLayout->DescriptorTableSizeRenderResources,
						EDescriptorHeapFlags::GPUVisible
					);

					// Copy persistent CPU descriptors into transient GPU-visible heap.
					Device()->GetD3D12Device()->CopyDescriptorsSimple(
						UINT( bindingLayout->DescriptorTableSizeRenderResources ),
						transient->GetCPUHandle( 0 ),
						bindingSet->RenderResourceHeap->GetCPUHandle( 0 ),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
					);

					m_ActiveDescriptorHeaps.PushBack( transient->Heap() );
					rootParamsSet[transient->Heap()] = { rootParamOffset + bindingLayout->RootParamRenderResources, transient->GetGPUHandle( 0 ) };
				}
			}

			if ( IsAutomaticResourceStateTransitionEnabled() )
			{
				for ( const RHIBindingSetItem& binding : bindingSet->Desc().Bindings )
				{
					if ( !binding.Resource )
						continue;

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
						default:
							break;
					}
				}
			}
		} // end for each binding set

		if ( IsAutomaticResourceStateTransitionEnabled() )
		{
			CommitBarriers();
		}

		// Finally, set the descriptor heaps and root tables
		if ( !m_ActiveDescriptorHeaps.Empty() )
		{
			// Set descriptor heaps
			m_ActiveCmdList.CmdList->SetDescriptorHeaps( UINT( m_ActiveDescriptorHeaps.Size() ), m_ActiveDescriptorHeaps.Data() );

			// Set root descriptor tables
			for ( const auto& [heap, pair] : rootParamsSet )
			{
				const auto it = rootParamsSet.find( heap );
				if ( it != rootParamsSet.end() )
				{
					m_ActiveCmdList.CmdList->SetGraphicsRootDescriptorTable(
						pair.first,
						pair.second
					);
				}
			}
		}
	}



	const SharedPtr<DescriptorHeap>& RHICommandList_D3D12Impl::AllocateHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags, StringView a_DebugName )
	{
		return m_CmdContext.DescriptorHeaps.EmplaceBack(
			GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap( a_Type, a_NumDescriptors, a_Flags, a_DebugName )
		);
	}

} // namespace Tridium