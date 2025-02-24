#include "tripch.h"
#include "D3D12Mesh.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

    //////////////////////////////////////////////////////////////////////////
    // D3D12 Index Buffer
    //////////////////////////////////////////////////////////////////////////

    bool D3D12IndexBuffer::Commit( const void* a_Params )
    {
        const auto* desc = ParamsToDescriptor<RHIIndexBufferDescriptor>( a_Params );
        return false;
    }

    bool D3D12IndexBuffer::Release()
    {
        return false;
    }

    bool D3D12IndexBuffer::IsValid() const
    {
        return false;
    }

    const void* D3D12IndexBuffer::NativePtr() const
    {
        return nullptr;
    }



    //////////////////////////////////////////////////////////////////////////
    // D3D12 Vertex Buffer
    //////////////////////////////////////////////////////////////////////////

    bool D3D12VertexBuffer::Commit( const void* a_Params )
    {
        const auto* desc = ParamsToDescriptor<RHIVertexBufferDescriptor>( a_Params );

        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT; // 64KB alignment
        resourceDesc.Width = desc->InitialData.size_bytes();
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES heapProps;
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.CreationNodeMask = 0;
        heapProps.VisibleNodeMask = 0;

		D3D12RHI* rhi = RHI::GetD3D12RHI();
        auto device = rhi->GetDevice();

        HRESULT hr = device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE,
            &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, IID_PPV_ARGS( &VBO ) );

        if ( FAILED( hr ) )
        {
            return false;
        }


#if RHI_DEBUG_ENABLED
        if ( RHIQuery::IsDebug() && !desc->Name.empty() )
        {
            WString wName = ToD3D12::ToWString( desc->Name.data() );
            VBO->SetName( wName.c_str() );
            D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
        }
#endif

        // Create the descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.NodeMask = 0;
        hr = device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &DescriptorHeap ) );
        if ( FAILED( hr ) )
        {
            return false;
        }

        // If we have initial data, write it to the buffer
        if ( desc->InitialData.size() > 0 )
        {
            // return ASSERT_LOG( Write( desc->InitialData ), "Failed to write initial data to buffer" );
			Write( desc->InitialData );
        }

        return true;
    }

    bool D3D12VertexBuffer::Release()
    {
        return false;
    }

    bool D3D12VertexBuffer::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
    {
        if ( !IsValid() )
        {
            return false;
        }

        // Acquire RHI objects
		auto* rhi = RHI::GetD3D12RHI();
        auto& device = rhi->GetDevice();
        auto& commandList = rhi->GetCommandList();
        auto& commandAllocator = rhi->GetCommandAllocator();
        auto& commandQueue = rhi->GetCommandQueue();
        HRESULT hr;

        // Reset command allocator and list if needed (depends on your usage pattern)
        commandAllocator->Reset();
        commandList->Reset( commandAllocator.Get(), nullptr );

        // Create an upload buffer to store incoming data on the CPU side
        D3D12_RESOURCE_DESC uploadBufferDesc = {};
        uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadBufferDesc.Width = a_Data.size();
        uploadBufferDesc.Height = 1;
        uploadBufferDesc.DepthOrArraySize = 1;
        uploadBufferDesc.MipLevels = 1;
        uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadBufferDesc.SampleDesc.Count = 1;
        uploadBufferDesc.SampleDesc.Quality = 0;
        uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES uploadHeapProps = {};
        uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadHeapProps.CreationNodeMask = 1;
        uploadHeapProps.VisibleNodeMask = 1;

        ComPtr<ID3D12Resource> uploadBuffer;
        hr = device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &uploadBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS( &uploadBuffer ) );
        if ( FAILED( hr ) )
            return false;

        // Map upload buffer and copy the data
        {
            char* uploadAddress;
            D3D12_RANGE rangeNoWrite = { 0, 0 };
            hr = uploadBuffer->Map( 0, &rangeNoWrite, reinterpret_cast<void**>( &uploadAddress ) );
            if ( FAILED( hr ) )
                return false;

            std::memcpy( uploadAddress, a_Data.data(), a_Data.size() );
            uploadBuffer->Unmap( 0, nullptr );
        }

        // Transition the vertex buffer resource to COPY_DEST before copying,
        // if it is not already in that state (depends on your engine). Example:
        // commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        //     VBO.Get(),
        //     currentState,
        //     D3D12_RESOURCE_STATE_COPY_DEST));

        // Issue the actual copy from the upload heap to the GPU vertex buffer
        commandList->CopyBufferRegion(
            VBO.Get(),           // pDstResource
            a_DstOffset,               // DstOffset
            uploadBuffer.Get(),        // pSrcResource
            0,                         // SrcOffset
            a_Data.size() );            // NumBytes

        // Transition the vertex buffer resource back to its usable state.
        // For example, to be used as a vertex buffer:
        // commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        //     VBO.Get(),
        //     D3D12_RESOURCE_STATE_COPY_DEST,
        //     D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

        hr = commandList->Close();
        if ( FAILED( hr ) )
            return false;

        ID3D12CommandList* commandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists( 1, commandLists );

        rhi->FenceSignal( {} );

        return true;
    }

    bool D3D12VertexBuffer::IsWritable() const
    {
        return false;
    }

	bool D3D12VertexBuffer::IsValid() const
	{
        return VBO != nullptr
            && Descriptor != nullptr;
	}

	const void* D3D12VertexBuffer::NativePtr() const
	{
		return VBO.Get();
	}

}
