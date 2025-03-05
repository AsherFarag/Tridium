#include "tripch.h"
#include "D3D12ResourceAllocator.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {
    bool D3D12ResourceAllocator::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIResourceAllocatorDescriptor>( a_Params );
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = desc->Capacity;
		heapDesc.Type = ToD3D12::GetDescriptorHeapType( desc->AllocatorType );
        heapDesc.NodeMask = 0;

		// Set the flags
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.Flags |= desc->Flags.HasFlag( ERHIResourceAllocatorFlags::ShaderVisible ) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        const HRESULT hr = RHI::GetD3D12RHI()->GetDevice()->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &DescriptorHeap ) );
        if ( FAILED( hr ) )
        {
            return false;
        }

		DescriptorSize = RHI::GetD3D12RHI()->GetDevice()->GetDescriptorHandleIncrementSize( heapDesc.Type );
		return true;
    }

    bool D3D12ResourceAllocator::Release()
    {
		DescriptorHeap.Release();
		return true;
    }

    bool D3D12ResourceAllocator::IsValid() const
    {
		return DescriptorHeap.Get() != nullptr;
    }

    const void* D3D12ResourceAllocator::NativePtr() const
    {
		return DescriptorHeap.Get();
    }

    bool D3D12ResourceAllocator::Allocate( uint32_t a_Count, uint32_t* o_Offset )
    {
		if ( a_Count + m_Size > GetDescriptor()->Capacity )
		{
			ASSERT_LOG( false, "Failed to allocate enough descriptors in resource allocator '{0}'", GetDescriptor()->Name );
			return false;
		}

		if ( o_Offset )
		{
			*o_Offset = m_Size;
		}

		m_Size += a_Count;
		return true;
    }

} // namespace Tridium