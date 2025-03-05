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
		bool Present() override;
		RHITextureRef GetBackBuffer() override;

		ComPtr<D3D12::SwapChain> SwapChain;
		RHIResourceAllocatorRef RenderTargetAllocator;
		InlineArray<RHITextureRef, RHIQuery::MaxColorTargets> RTVs;
	};

}