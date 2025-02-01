#pragma once
#include <Tridium/RHI/DynamicRHI.h>
#include "ComPointer.h"

namespace Tridium::DX12 {

	class DirectX12RHI final : public DynamicRHI
	{
	public:
		virtual bool Init( const RHIConfig& a_Config ) override;
		virtual bool Shutdown() override;
		virtual bool Present() override;

	private:
		ComPtr<IDXGIFactory7> m_DXGIFactory = nullptr;
		ComPtr<ID3D12Device8> m_Device = nullptr;
		ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
		ComPtr<ID3D12Fence1> m_Fence = nullptr;
		uint64_t m_FenceValue = 0;
		void* m_FenceEvent = nullptr;

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12Debug6> m_D3D12Debug = nullptr;
		ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
	#endif
	};

}