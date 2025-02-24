#include "tripch.h"
#include "D3D12Shader.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

    bool D3D12ShaderModule::Commit( const void* a_Params )
    {
        const RHIShaderModuleDescriptor* desc = ParamsToDescriptor<RHIShaderModuleDescriptor>( a_Params );
		if ( desc == nullptr )
		{
			return false;
		}

		// Copy the byte code from the descriptor span
        Bytecode.Resize( desc->Bytecode.size() );
		std::memcpy( Bytecode.Data(), desc->Bytecode.data(), desc->Bytecode.size_bytes() );

		TODO( "Pretty dirty, figure out a better relationship for a shader and its bytecode" );
		Descriptor.Get<RHIShaderModuleDescriptor>()->Bytecode = Bytecode;

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