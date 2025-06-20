#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	//RHISampler_D3D12Impl::RHISampler_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
	//	: IRHISampler( a_Device, a_Desc )
	//{
	//	// Create the sampler
	//	SamplerDesc.Filter = Translate( a_Desc.MinFilter, a_Desc.MagFilter, a_Desc.MipFilter );
	//	SamplerDesc.AddressU = Translate( a_Desc.AddressU );
	//	SamplerDesc.AddressV = Translate( a_Desc.AddressV );
	//	SamplerDesc.AddressW = Translate( a_Desc.AddressW );
	//	SamplerDesc.MipLODBias = a_Desc.MipLODBias;
	//	SamplerDesc.MaxAnisotropy = a_Desc.MaxAnisotropy;
	//	SamplerDesc.ComparisonFunc = Translate( a_Desc.ComparisonFunc );
	//	SamplerDesc.BorderColor[0] = a_Desc.BorderColor[0];
	//	SamplerDesc.BorderColor[1] = a_Desc.BorderColor[1];
	//	SamplerDesc.BorderColor[2] = a_Desc.BorderColor[2];
	//	SamplerDesc.BorderColor[3] = a_Desc.BorderColor[3];
	//	SamplerDesc.MinLOD = a_Desc.MinLOD;
	//	SamplerDesc.MaxLOD = a_Desc.MaxLOD;
	//}

	//D3D12_STATIC_SAMPLER_DESC RHISampler_D3D12Impl::GetStaticSamplerDesc( uint32_t a_ShaderRegister, ERHIShaderVisibility a_ShaderVisibility ) const
	//{
	//	D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc{};
	//	StaticSamplerDesc.Filter = SamplerDesc.Filter;
	//	StaticSamplerDesc.AddressU = SamplerDesc.AddressU;
	//	StaticSamplerDesc.AddressV = SamplerDesc.AddressV;
	//	StaticSamplerDesc.AddressW = SamplerDesc.AddressW;
	//	StaticSamplerDesc.MipLODBias = SamplerDesc.MipLODBias;
	//	StaticSamplerDesc.MaxAnisotropy = SamplerDesc.MaxAnisotropy;
	//	StaticSamplerDesc.ComparisonFunc = SamplerDesc.ComparisonFunc;
	//	StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	//	StaticSamplerDesc.MinLOD = SamplerDesc.MinLOD;
	//	StaticSamplerDesc.MaxLOD = SamplerDesc.MaxLOD;
	//	StaticSamplerDesc.ShaderRegister = a_ShaderRegister;
	//	StaticSamplerDesc.RegisterSpace = 0;
	//	StaticSamplerDesc.ShaderVisibility = Translate( a_ShaderVisibility );
	//	return StaticSamplerDesc;
	//}

}