#include "tripch.h"
#include "D3D12ShaderBindingLayout.h"

namespace Tridium {

    bool D3D12ShaderBindingLayout::Commit( const void* a_Params )
    {
        return false;
    }

    bool D3D12ShaderBindingLayout::Release()
    {
        return false;
    }

    bool D3D12ShaderBindingLayout::IsValid() const
    {
        return false;
    }

    const void* D3D12ShaderBindingLayout::NativePtr() const
    {
        return nullptr;
    }

}
