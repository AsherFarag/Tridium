#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12SwapChain : public RHISwapChain
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override { return SwapChain.Get(); }

		ComPtr<D3D12::SwapChain> SwapChain;
		ComPtr<D3D12::DescriptorHeap> RTVDescHeap;
		InlineArray<Pair<D3D12_CPU_DESCRIPTOR_HANDLE, ComPtr<D3D12::Resource>>, RHIQuery::MaxColorTargets> RTVs;
	};

}