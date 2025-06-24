#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {
	RHIBuffer_D3D12Impl::RHIBuffer_D3D12Impl( IDynamicRHI* a_Device, const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data )
		: IRHIBuffer( a_Device, a_Desc )
	{
		m_Desc.Size = Math::Max( a_Desc.Size, a_Data.size_bytes() );
		if ( m_Desc.Size == 0 )
		{
			ASSERT( false, "Buffer size is 0!" );
			return;
		}

		// Initial state for buffers must always be Common, according to the D3D12 spec.
		SetState( ERHIResourceStates::Common );

		D3D12_RESOURCE_DESC d3d12Desc = GetD3D12ResourceDesc();

		// Create the texture
		if ( !ManagedBuffer.Commit( d3d12Desc, D3D12::Translate( State() ) ) )
		{
			ASSERT( false, "Failed to create D3D12 buffer" );
			return;
		}

		D3D12_SET_DEBUG_NAME( ManagedBuffer.Resource(), m_Desc.Name, L"Unnamed Buffer");

		if ( a_Data.size() > 0 )
		{
			SetState( ERHIResourceStates::CopyDest );
			m_Desc.Size = a_Data.size();

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
			uploadBuffer.Resource()->Map(0, &uploadRange, (void**)&uploadBufferAddress);
			memcpy( uploadBufferAddress, a_Data.data(), a_Data.size() );
			uploadBuffer.Resource()->Unmap(0, &uploadRange);

			auto* cmdList = Device()->GetResourceInitCommandList();
			cmdList->Open();
			Cast<ID3D12GraphicsCommandList*>( cmdList->GetD3D12CmdList() )->CopyBufferRegion( 
				ManagedBuffer.Resource(), 0, uploadBuffer.Resource(), 0, m_Desc.Size
			);
			cmdList->Close();

			IRHICommandList* cmdListPtr = cmdList;
			const RHIFenceValue fence = Device()->ExecuteCommandLists( Span{ &cmdListPtr, 1 }, ERHICommandQueueType::Copy );
			Device()->WaitForFence( ERHICommandQueueType::Copy, fence );
		}
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
