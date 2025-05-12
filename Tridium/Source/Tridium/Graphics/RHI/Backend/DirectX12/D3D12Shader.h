#pragma once
#include "D3D12Common.h"

namespace Tridium {

	//=================================================
	// D3D12 Shader Module
	//  Known as a Shader Blob in D3D12.
	//=================================================
	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIShaderModule_D3D12Impl, RHIShaderModule )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIShaderModule_D3D12Impl, ERHInterfaceType::DirectX12 )
		RHIShaderModule_D3D12Impl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		Array<Byte> Bytecode;
	};

} // namespace Tridium