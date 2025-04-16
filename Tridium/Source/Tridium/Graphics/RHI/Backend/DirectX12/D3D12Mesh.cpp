#include "tripch.h"
#include "D3D12Mesh.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

#if 0

    //////////////////////////////////////////////////////////////////////////
    // D3D12 Index Buffer
    //////////////////////////////////////////////////////////////////////////

    bool D3D12IndexBuffer::Commit( const void* a_Params )
    {
        const auto* desc = ParamsToDescriptor<RHIIndexBufferDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		D3D12_RESOURCE_DESC resourceDesc;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT; // 64KB alignment
		resourceDesc.Width = desc->InitialData.size_bytes();
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = D3D12::To<DXGI_FORMAT>::From( desc->DataType );
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12RHI* rhi = GetD3D12RHI();

		if ( !IBO.Commit( resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST ) )
		{
			return false;
		}

        return true;
    }

    bool D3D12IndexBuffer::Release()
    {
		IBO.Release();
		return true;
    }

    bool D3D12IndexBuffer::IsValid() const
    {
		return IBO.Valid();
    }

	size_t D3D12IndexBuffer::GetSizeInBytes() const
	{
		if ( !IsValid() )
		{
			return 0;
		}
		D3D12_RESOURCE_DESC desc = IBO.Resource->GetDesc();
		return desc.Width;
	}

	bool D3D12IndexBuffer::Read( Array<Byte>& o_Data, size_t a_SrcOffset )
	{
		return false;
	}

    bool D3D12IndexBuffer::IsReadable() const
    {
		return false;
    }

    bool D3D12IndexBuffer::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
    {
		return false;
    }

	bool D3D12IndexBuffer::IsWritable() const
	{
		return false;
	}

	bool D3D12IndexBuffer::IsReady() const
	{
		return true;
	}

	void D3D12IndexBuffer::Wait()
	{
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

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		if ( !VBO.Commit( resourceDesc, allocDesc, D3D12_RESOURCE_STATE_COPY_DEST ) )
		{
			return false;
		}

        D3D12_SET_DEBUG_NAME( VBO.Resource.Get(), desc->Name );

		return true;
    }

    bool D3D12VertexBuffer::Release()
    {
        return false;
    }

	size_t D3D12VertexBuffer::GetSizeInBytes() const
	{
		if ( !IsValid() )
		{
			return 0;
		}
		D3D12_RESOURCE_DESC desc = VBO.Resource->GetDesc();
		return desc.Width;
	}

	bool D3D12VertexBuffer::Read( Array<Byte>& o_Data, size_t a_SrcOffset )
	{
		NOT_IMPLEMENTED;
		return false;
	}

	bool D3D12VertexBuffer::IsReadable() const
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
        auto* rhi = GetD3D12RHI();
        auto& copyCmdAllocator = rhi->GetCopyCmdAllocator();
        auto& copyCmdList = rhi->GetCopyCmdList();
        auto& copyQueue = rhi->GetCopyQueue();

        // Reset copy allocator and list
        copyCmdAllocator->Reset();
        copyCmdList->Reset( copyCmdAllocator.Get(), nullptr );

        // Allocate a temporary upload buffer
        thread_local D3D12::ManagedResource uploadBuffer{};
        D3D12MA::ALLOCATION_DESC allocDesc = {};
        allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

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

        if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
            return false;

        // Copy data to the upload buffer
        void* mappedMemory = nullptr;
        D3D12_RANGE uploadRange = { 0, a_Data.size() };
        uploadBuffer.Resource->Map( 0, &uploadRange, &mappedMemory );
        std::memcpy( mappedMemory, a_Data.data(), a_Data.size() );
        uploadBuffer.Resource->Unmap( 0, &uploadRange );

        // Copy buffer region
        copyCmdList->CopyBufferRegion(
            VBO.Resource.Get(),                   // Destination buffer
            a_DstOffset,                 // Destination offset
            uploadBuffer.Resource.Get(), // Source buffer
            0,                           // Source offset
            a_Data.size() );              // Size in bytes

        // Add a barrier to transition the vertex buffer to VERTEX_AND_CONSTANT_BUFFER
        //D3D12_RESOURCE_BARRIER barrier = {};
        //barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        //barrier.Transition.pResource = VBO.Resource.Get();
        //barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        //barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        //copyCmdList->ResourceBarrier( 1, &barrier );

        // Close and execute copy commands
        if ( FAILED( copyCmdList->Close() ) )
        {
            return false;
        }

        ID3D12CommandList* cmdLists[] = { copyCmdList.Get() };
        copyQueue->ExecuteCommandLists( 1, cmdLists );

        // Signal a fence that can be waited on asynchronously
        m_CopyFenceValue = rhi->IncrementCopyFence();
        if ( FAILED( copyQueue->Signal( rhi->GetCopyFence()->NativePtrAs<ID3D12Fence>(), m_CopyFenceValue ) ) )
            return false;

        return true;
    }

	bool D3D12VertexBuffer::IsWritable() const
	{
		return true;
	}

    bool D3D12VertexBuffer::IsReady() const
    {
        if ( !IsValid()
            || GetD3D12RHI()->GetCopyFence()->GetCompletedValue() < m_CopyFenceValue )
        {
            return false;
        }

        return true;
    }

    void D3D12VertexBuffer::Wait()
    {
        if ( !IsValid() )
        {
            return;
        }

        auto* rhi = GetD3D12RHI();
        const auto& copyFence = rhi->GetCopyFence();
        copyFence->Wait( m_CopyFenceValue );
    }

#endif
}
