#pragma once
#include "D3D12Common.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12Sampler, RHISampler )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12Sampler, ERHInterfaceType::DirectX12 )
		bool Commit( const RHISamplerDescriptor& a_Desc )
		{
			m_Descriptor = a_Desc;

			// Create the sampler heap
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.NumDescriptors = 1;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if ( FAILED( GetD3D12RHI()->GetDevice()->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &SamplerHeap ) ) ) )
			{
				return false;
			}

			// Get the handle
			SamplerHandle = SamplerHeap->GetGPUDescriptorHandleForHeapStart();

			// Create the sampler
			SamplerDesc.Filter = D3D12::Translate( a_Desc.Filter );
			SamplerDesc.AddressU = D3D12::Translate( a_Desc.AddressU );
			SamplerDesc.AddressV = D3D12::Translate( a_Desc.AddressV );
			SamplerDesc.AddressW = D3D12::Translate( a_Desc.AddressW );
			SamplerDesc.MipLODBias = a_Desc.MipLODBias;
			SamplerDesc.MaxAnisotropy = a_Desc.MaxAnisotropy;
			SamplerDesc.ComparisonFunc = D3D12::Translate( a_Desc.ComparisonFunc );
			SamplerDesc.BorderColor[0] = a_Desc.BorderColor.r;
			SamplerDesc.BorderColor[1] = a_Desc.BorderColor.g;
			SamplerDesc.BorderColor[2] = a_Desc.BorderColor.b;
			SamplerDesc.BorderColor[3] = a_Desc.BorderColor.a;
			SamplerDesc.MinLOD = a_Desc.MinLOD;
			SamplerDesc.MaxLOD = a_Desc.MaxLOD;

			GetD3D12RHI()->GetDevice()->CreateSampler( &SamplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart() );

			return true;
		}

		bool Release() override
		{
			SamplerHeap.Release();
			return true;
		}

		bool IsValid() const override
		{
			return SamplerHeap != nullptr;
		}

		const void* NativePtr() const override
		{
			return SamplerHeap.Get();
		}

		D3D12_STATIC_SAMPLER_DESC GetStaticSamplerDesc( uint32_t a_ShaderRegister, ERHIShaderVisibility a_ShaderVisibility = ERHIShaderVisibility::All ) const
		{
			D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc{};
			StaticSamplerDesc.Filter = SamplerDesc.Filter;
			StaticSamplerDesc.AddressU = SamplerDesc.AddressU;
			StaticSamplerDesc.AddressV = SamplerDesc.AddressV;
			StaticSamplerDesc.AddressW = SamplerDesc.AddressW;
			StaticSamplerDesc.MipLODBias = SamplerDesc.MipLODBias;
			StaticSamplerDesc.MaxAnisotropy = SamplerDesc.MaxAnisotropy;
			StaticSamplerDesc.ComparisonFunc = SamplerDesc.ComparisonFunc;
			StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			StaticSamplerDesc.MinLOD = SamplerDesc.MinLOD;
			StaticSamplerDesc.MaxLOD = SamplerDesc.MaxLOD;
			StaticSamplerDesc.ShaderRegister = a_ShaderRegister;
			StaticSamplerDesc.RegisterSpace = 0;
			StaticSamplerDesc.ShaderVisibility = D3D12::Translate( a_ShaderVisibility );
			return StaticSamplerDesc;
		}

		D3D12_SAMPLER_DESC SamplerDesc{};
		ComPtr<ID3D12::DescriptorHeap> SamplerHeap;
		D3D12_GPU_DESCRIPTOR_HANDLE SamplerHandle = {};
	};

} // namespace Tridium