#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	RHIShaderModule_D3D12Impl::RHIShaderModule_D3D12Impl( const RHIShaderModuleDescriptor& a_Desc )
		: RHIShaderModule( a_Desc )
    {
		// Copy the byte code from the descriptor span
        Bytecode.Resize( a_Desc.Bytecode.size() );
		std::memcpy( Bytecode.Data(), a_Desc.Bytecode.data(), a_Desc.Bytecode.size_bytes() );

		TODO( "Pretty dirty, figure out a better relationship for a shader and its bytecode" );
		m_Desc.Bytecode = Bytecode;
    }

    bool RHIShaderModule_D3D12Impl::Release()
    {
        return false;
    }

    bool RHIShaderModule_D3D12Impl::Valid() const
    {
        return true;
    }

    const void* RHIShaderModule_D3D12Impl::NativePtr() const
    {
        return nullptr;
    }

} // namespace Tridium