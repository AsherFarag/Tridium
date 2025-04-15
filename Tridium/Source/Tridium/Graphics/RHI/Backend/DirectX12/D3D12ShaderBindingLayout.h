#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12ShaderBindingLayout, RHIShaderBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12ShaderBindingLayout, ERHInterfaceType::DirectX12 );
		bool Commit( const RHIShaderBindingLayoutDescriptor & a_Desc ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<ID3D12RootSignature> m_RootSignature;
	};

} // namespace Tridium