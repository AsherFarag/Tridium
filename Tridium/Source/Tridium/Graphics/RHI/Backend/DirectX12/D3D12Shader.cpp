#include "tripch.h"
#include "D3D12Shader.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

	bool D3D12ShaderModule::Commit( const RHIShaderModuleDescriptor& a_Desc )
    {
		m_Descriptor = a_Desc;

		// Copy the byte code from the descriptor span
        Bytecode.Resize( a_Desc.Bytecode.size() );
		std::memcpy( Bytecode.Data(), a_Desc.Bytecode.data(), a_Desc.Bytecode.size_bytes() );

		TODO( "Pretty dirty, figure out a better relationship for a shader and its bytecode" );
		m_Descriptor.Bytecode = Bytecode;

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