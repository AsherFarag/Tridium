#pragma once
#include "D3D12Common.h"

namespace Tridium {

	//=================================================
	// D3D12 Shader Module
	//  Known as a Shader Blob in D3D12.
	//=================================================
	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12ShaderModule, RHIShaderModule )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12ShaderModule, ERHInterfaceType::DirectX12 )
		bool Commit( const RHIShaderModuleDescriptor& a_Desc ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		Array<Byte> Bytecode;
	};

} // namespace Tridium