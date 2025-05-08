#include "tripch.h"
#include "D3D12Buffer.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {
	D3D12Buffer::D3D12Buffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
		: RHIBuffer( a_Desc )
	{
		m_Desc.Size = Math::Max( a_Desc.Size, a_Data.size_bytes() );
		if ( m_Desc.Size == 0 )
		{
			ASSERT( false, "Buffer size is 0!" );
			return;
		}

		const bool initData = a_Data.size() > 0;
		ERHIResourceStates initialState = initData ? ERHIResourceStates::CopyDest : ERHIResourceStates::Common;
		SetState( initialState );

		const RHIFormatInfo& formatInfo = GetRHIFormatInfo( a_Desc.Format );
		D3D12_RESOURCE_DESC d3d12Desc = GetD3D12ResourceDesc();

		// Create the texture
		if ( !ManagedBuffer.Commit( d3d12Desc, D3D12::Translate( initialState ) ) )
		{
			ASSERT( false, "Failed to create D3D12 buffer" );
			return;
		}

		D3D12_SET_DEBUG_NAME( ManagedBuffer.Resource, m_Desc.Name );

		if ( initData )
		{
			m_Desc.Size = a_Data.size();
			D3D12CommandContext& copyCmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Copy );

			// Create the upload buffer
			D3D12::ManagedResource uploadBuffer{};
			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC uploadBufferDesc = {};
			uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			uploadBufferDesc.Width = m_Desc.Size;
			uploadBufferDesc.Height = 1;
			uploadBufferDesc.DepthOrArraySize = 1;
			uploadBufferDesc.MipLevels = 1;
			uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			uploadBufferDesc.SampleDesc.Count = 1;
			uploadBufferDesc.SampleDesc.Quality = 0;
			uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			{
				ASSERT( false, "Failed to create D3D12 upload buffer" );
				return;
			}

			// Copy data to upload buffer
			char* uploadBufferAddress;
			D3D12_RANGE uploadRange = { 0, m_Desc.Size };
			uploadBuffer.Resource->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
			memcpy( uploadBufferAddress, a_Data.data(), a_Data.size() );
			uploadBuffer.Resource->Unmap( 0, &uploadRange );

			// Ensure the copy command list is reset
			TODO( "Should we be doing this, this way?" );
			if ( FAILED( copyCmdCtx.CmdAllocator->Reset() ) )
			{
				ASSERT( false, "Failed to reset command allocator" );
				return;
			}
			if ( FAILED( copyCmdCtx.CmdList->Reset( copyCmdCtx.CmdAllocator.Get(), nullptr ) ) )
			{
				ASSERT( false, "Failed to reset command list" );
				return;
			}

			// Copy data to buffer
			copyCmdCtx.CmdList->CopyBufferRegion( ManagedBuffer.Resource.Get(), 0, uploadBuffer.Resource.Get(), 0, m_Desc.Size );

			// Execute the copy command list
			if ( FAILED( copyCmdCtx.CmdList->Close() ) )
			{
				ASSERT( false, "Failed to close command list" );
				return;
			}

			ID3D12CommandList* cmdLists[] = { copyCmdCtx.CmdList.Get() };
			copyCmdCtx.CmdQueue->ExecuteCommandLists( 1, cmdLists );
			copyCmdCtx.Wait( copyCmdCtx.Signal() );
		}
	}

	D3D12_RESOURCE_DESC D3D12Buffer::GetD3D12ResourceDesc() const
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = m_Desc.Size;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		return desc;
	}
}
