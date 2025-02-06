#include "tripch.h"
#include "D3D12Shader.h"

namespace Tridium {

    bool D3D12ShaderModule::Commit( const void* a_Params )
    {
        return false;
    }

    bool D3D12ShaderModule::Release()
    {
        return false;
    }

    bool D3D12ShaderModule::IsValid() const
    {
        return false;
    }

    const void* D3D12ShaderModule::NativePtr() const
    {
        return nullptr;
    }

} // namespace Tridium