#pragma once
#include "DirectX12.h"
#include <Tridium/RHI/RHICommon.h>
#include <Tridium/RHI/DynamicRHI.h>
#include <Tridium/RHI/RHICommandList.h>

namespace Tridium {

	class DirectX12RHI final : public DynamicRHI
	{
	public:
		virtual bool Init( const RHIConfig& a_Config ) override;
		virtual bool Shutdown() override;
		virtual bool Present() override;
		virtual ERHInterfaceType GetRHInterfaceType() const override { return ERHInterfaceType::DirectX12; }

		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;

		virtual RHIFence CreateFence() const override;
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const override;
		virtual void FenceSignal( RHIFence a_Fence ) override;

		//====================================================

		const auto& GetFactory() const { return m_DXGIFactory; }
		const auto& GetDevice() const { return m_Device; }
		const auto& GetCommandQueue() const { return m_CommandQueue; }
		const auto& GetCommandAllocator() const { return m_CommandAllocator; }
		const auto& GetCommandList() const { return m_CommandList; }

	private:
		ComPtr<IDXGIFactory7> m_DXGIFactory = nullptr;
		ComPtr<ID3D12Device8> m_Device = nullptr;
		ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
		ComPtr<ID3D12Fence1> m_Fence = nullptr;
		uint64_t m_FenceValue = 0;
		void* m_FenceEvent = nullptr;

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12Debug6> m_D3D12Debug = nullptr;
		ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
	#endif
	};

}