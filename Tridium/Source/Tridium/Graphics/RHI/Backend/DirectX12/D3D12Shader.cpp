#include "tripch.h"
#include "D3D12Shader.h"
#include "D3D12RHI.h"

namespace Tridium {

    bool D3D12ShaderModule::Commit( const void* a_Params )
    {
        const RHIShaderModuleDescriptor* desc = ParamsToDescriptor<RHIShaderModuleDescriptor>( a_Params );
		if ( desc == nullptr )
		{
			return false;
		}

        return true;
    }

    bool D3D12ShaderModule::Release()
    {
        return false;
    }

    bool D3D12ShaderModule::IsValid() const
    {
        return true;
    }

    const void* D3D12ShaderModule::NativePtr() const
    {
        return nullptr;
    }

} // namespace Tridium