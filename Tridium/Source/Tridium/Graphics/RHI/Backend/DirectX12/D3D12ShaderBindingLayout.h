#pragma once
#include "D3D12Common.h"

namespace Tridium {

	using RootParameterIndex = uint32_t;

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingLayout_D3D12Impl, RHIBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingLayout_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIBindingLayout_D3D12Impl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<ID3D12RootSignature> m_RootSignature;

		uint32_t InlinedConstantsSize = 0; // Size of the inlined constants in bytes
		RootParameterIndex RootParamInlinedConstants = ~0;
		RootParameterIndex RootParamSRV = ~0;
		RootParameterIndex RootParamSamplers = ~0;
		InlineArray<D3D12_ROOT_PARAMETER1, 32> RootParams;
	};

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingSet_D3D12Impl, RHIBindingSet )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingSet_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIBindingSet_D3D12Impl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

} // namespace Tridium