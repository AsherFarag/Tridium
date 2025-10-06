#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {
	RHIBuffer_D3D12Impl::RHIBuffer_D3D12Impl( IDynamicRHI* a_Device, const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data )
		: IRHIBuffer( a_Device, a_Desc )
	{
		m_Desc.Size = Math::Max( a_Desc.Size, a_Data.size_bytes() );
		if ( m_Desc.Size == 0 )
		{
			RHI_DEV_CHECK( false, "Buffer size is 0!" );
			return;
		}

		if ( m_Desc.IsConstantBuffer() )
		{
			// Constant buffers must be 256-byte aligned.
			// See: https://learn.microsoft.com/en-us/windows/win32/direct3d12/constant-buffers-and-structured-buffers-in-direct3d-12
			// > "When creating a constant buffer view (CBV), the size in bytes must be a multiple of 256."
			// > "This is because the hardware can only access constant buffer data at 256-byte boundaries."
			// > "For example, if you have a structure that is 300 bytes, you must allocate 512 bytes for the constant buffer."
			// > "The first 300 bytes will contain your data, and the remaining 212 bytes will be unused padding."
			// > "This ensures that the constant buffer is properly aligned for the hardware to access it."
			m_Desc.Size = AlignUp( m_Desc.Size, size_t( D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT ) );
		}

		SetState( a_Desc.InitialState );

		D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

		const D3D12_RESOURCE_DESC d3d12Desc = GetD3D12ResourceDesc();
		D3D12MA::ALLOCATION_DESC allocDesc{};
		
		switch ( m_Desc.HeapType )
		{
			case ERHIHeapType::Dynamic: // We just treat dynamic as default for now
			case ERHIHeapType::Immutable:
			case ERHIHeapType::Default:
				allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				initialState = D3D12_RESOURCE_STATE_COMMON;
				break;
			case ERHIHeapType::Staging:
				allocDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
				initialState = D3D12_RESOURCE_STATE_COPY_DEST;
				break;
			default:
				RHI_DEV_CHECK( false, "Unsupported heap type for buffer '{}'", m_Desc.Name );
				return;
		}

		// Create the texture
		if ( !ManagedBuffer.Commit( d3d12Desc, allocDesc, initialState ) )
		{
			ASSERT( false, "Failed to allocate D3D12 buffer" );
			return;
		}

		D3D12_SET_DEBUG_NAME( ManagedBuffer.Resource(), m_Desc.Name, L"Unnamed Buffer");

		if ( a_Data.size() == 0 )
		{
			return; // No data to upload
		}

		RHI_DEV_CHECK( m_Desc.HeapType != ERHIHeapType::Staging,
					   "Cannot upload initial data to staging buffer '{}'", m_Desc.Name );

		// Create the upload buffer
		D3D12::ManagedResource uploadBuffer{};
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
		uploadBuffer.Resource()->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		memcpy( uploadBufferAddress, a_Data.data(), a_Data.size() );
		uploadBuffer.Resource()->Unmap( 0, &uploadRange );

		const auto beforeBarrier = D3D12_RESOURCE_BARRIER{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = D3D12_RESOURCE_TRANSITION_BARRIER{
				.pResource = ManagedBuffer.Resource(),
				.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				.StateBefore = initialState,
				.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST
			}
		};

		const auto afterBarrier = D3D12_RESOURCE_BARRIER{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = D3D12_RESOURCE_TRANSITION_BARRIER{
				.pResource = ManagedBuffer.Resource(),
				.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
				.StateAfter = Translate( a_Desc.InitialState )
			}
		};

		TODO( "Handle CPU Write buffers as they suck and are upload heaps " );

		auto* cmdList = Device()->GetResourceInitCommandList();
		cmdList->Open();
		cmdList->GetD3D12CmdList()->ResourceBarrier( 1, &beforeBarrier );
		cmdList->GetD3D12CmdList()->CopyBufferRegion( ManagedBuffer.Resource(), 0, uploadBuffer.Resource(), 0, m_Desc.Size );
		cmdList->GetD3D12CmdList()->ResourceBarrier( 1, &afterBarrier );
		cmdList->Close();

		IRHICommandList* cmdListPtr = cmdList;
		const RHIFenceValue fence = Device()->ExecuteCommandLists( Span{ &cmdListPtr, 1 }, cmdListPtr->Desc().QueueType );
		Device()->WaitForFence( cmdListPtr->Desc().QueueType, fence );
	}

	const void* RHIBuffer_D3D12Impl::Map()
	{
		RHI_DEV_CHECK( Desc().HeapType == ERHIHeapType::Staging,
					   "Buffer '{}' must be created with staging heap to be mappable", Desc().Name );

		D3D12_RANGE range = { 0, Desc().Size };
		void* mappedBuffer;
		const HRESULT hr = ManagedBuffer.Resource()->Map( 0, &range, &mappedBuffer );

		if ( FAILED( hr ) )
		{
			return nullptr;
		}

		return mappedBuffer;
	}

	void RHIBuffer_D3D12Impl::Unmap()
	{
		RHI_DEV_CHECK( Desc().HeapType == ERHIHeapType::Staging,
					   "Buffer '{}' must be created with staging heap to be unmapped", Desc().Name );

		D3D12_RANGE range = { 0, Desc().Size };
		ManagedBuffer.Resource()->Unmap( 0, &range );
	}

	D3D12_RESOURCE_DESC RHIBuffer_D3D12Impl::GetD3D12ResourceDesc() const
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

	D3D12_SHADER_RESOURCE_VIEW_DESC RHIBuffer_D3D12Impl::CreateSRVDesc( ERHIBufferType a_Type, RHIBufferRange a_Range, ERHIFormat a_Format ) const
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		a_Range.Offset = Math::Min( a_Range.Offset, m_Desc.Size );
		a_Range.Size = Math::Min( a_Range.Size, m_Desc.Size - a_Range.Offset );

		if ( a_Format == ERHIFormat::Unknown )
			a_Format = m_Desc.Format; // Use the buffer's format if not specified

		switch ( a_Type )
		{
			case ERHIBufferType::Raw:
			{
				srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
				srvDesc.Buffer.FirstElement = NumDWORDsFromBytes( a_Range.Offset );
				srvDesc.Buffer.NumElements = NumDWORDsFromBytes( a_Range.Size );
				break;
			}
			case ERHIBufferType::Formatted:
			{
				RHI_DEV_CHECK( a_Format != ERHIFormat::Unknown, "Invalid format for formatted buffer SRV" );
				RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Format );
				srvDesc.Format = Translate( a_Format );
				srvDesc.Buffer.FirstElement = a_Range.Offset / formatInfo.BytesPerBlock;
				srvDesc.Buffer.NumElements = a_Range.Size / formatInfo.BytesPerBlock;
				break;
			}
			case ERHIBufferType::Structured:
			{
				RHI_DEV_CHECK( m_Desc.Stride != 0, "Stride must be set for structured buffers" );
				srvDesc.Buffer.FirstElement = a_Range.Offset / m_Desc.Stride;
				srvDesc.Buffer.NumElements = a_Range.Size / m_Desc.Stride;
				srvDesc.Buffer.StructureByteStride = m_Desc.Stride;
				break;
			}
			default:
			{
				ASSERT( false, "Unsupported buffer type for SRV" );
				return {};
			}
		}

		return srvDesc;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC RHIBuffer_D3D12Impl::CreateCBVDesc( RHIBufferRange a_Range ) const
	{
		RHI_DEV_CHECK( m_Desc.IsConstantBuffer(), "Buffer is not a constant buffer" );
		a_Range.Offset = Math::Min( a_Range.Offset, m_Desc.Size );
		a_Range.Size = Math::Min( a_Range.Size, m_Desc.Size - a_Range.Offset );

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = ManagedBuffer.Resource()->GetGPUVirtualAddress() + a_Range.Offset;
		cbvDesc.SizeInBytes = AlignUp( a_Range.Size, size_t( D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT ) );

		return cbvDesc;
	}

}
