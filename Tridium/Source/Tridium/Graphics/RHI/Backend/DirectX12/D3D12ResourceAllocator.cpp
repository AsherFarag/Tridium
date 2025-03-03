#include "tripch.h"
#include "D3D12ResourceAllocator.h"

namespace Tridium {
    bool D3D12ResourceAllocator::Commit( const void* a_Params )
    {
        return false;
    }

    bool D3D12ResourceAllocator::Release()
    {
        return false;
    }

    bool D3D12ResourceAllocator::IsValid() const
    {
        return false;
    }

    const void* D3D12ResourceAllocator::NativePtr() const
    {
        return nullptr;
    }

} // namespace Tridium