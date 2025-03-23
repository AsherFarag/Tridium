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
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		ComPtr<D3D12::SwapChain> SwapChain;
		RHIResourceAllocatorRef RenderTargetAllocator;
		InlineArray<RHITextureRef, RHIConstants::MaxColorTargets> RTVs;

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		bool m_ShouldResize = false;

		bool ResizeBuffers();
		void ReleaseBuffers();
		bool GetBackBuffers();
	};

}