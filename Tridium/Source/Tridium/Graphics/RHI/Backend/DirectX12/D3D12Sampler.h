#pragma once
#include "D3D12Common.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

	class D3D12Sampler final : public RHISampler
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );
		bool Commit( const void* a_Params ) override
		{
			const auto* desc = ParamsToDescriptor<RHISamplerDescriptor>( a_Params );
			if ( desc == nullptr )
			{
				return false;
			}

			// Create the sampler heap
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.NumDescriptors = 1;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if ( FAILED( RHI::GetD3D12RHI()->GetDevice()->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &SamplerHeap ) ) ) )
			{
				return false;
			}

			// Get the handle
			SamplerHandle = SamplerHeap->GetGPUDescriptorHandleForHeapStart();

			// Create the sampler
			SamplerDesc.Filter = D3D12::Translate( desc->Filter );
			SamplerDesc.AddressU = D3D12::Translate( desc->AddressU );
			SamplerDesc.AddressV = D3D12::Translate( desc->AddressV );
			SamplerDesc.AddressW = D3D12::Translate( desc->AddressW );
			SamplerDesc.MipLODBias = desc->MipLODBias;
			SamplerDesc.MaxAnisotropy = desc->MaxAnisotropy;
			SamplerDesc.ComparisonFunc = D3D12::Translate( desc->ComparisonFunc );
			SamplerDesc.BorderColor[0] = desc->BorderColor.r;
			SamplerDesc.BorderColor[1] = desc->BorderColor.g;
			SamplerDesc.BorderColor[2] = desc->BorderColor.b;
			SamplerDesc.BorderColor[3] = desc->BorderColor.a;
			SamplerDesc.MinLOD = desc->MinLOD;
			SamplerDesc.MaxLOD = desc->MaxLOD;

			RHI::GetD3D12RHI()->GetDevice()->CreateSampler( &SamplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart() );

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