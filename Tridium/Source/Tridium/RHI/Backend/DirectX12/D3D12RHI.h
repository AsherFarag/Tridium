#pragma once
#include "D3D12Common.h"
#include <Tridium/RHI/RHICommon.h>
#include <Tridium/RHI/DynamicRHI.h>

namespace Tridium {

	class DirectX12RHI final : public DynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) override;
		// Shutdown the RHI.
		virtual bool Shutdown() override;
		// Present the current frame.
		virtual bool Present() override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		//==============================================

		//====================================================
		// Creates a fence that can be used to synchronize the CPU and GPU.
		virtual RHIFence CreateFence() const override;
		// Queries the state of a fence.
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const override;
		// Blocks the calling CPU thread until the fence is signaled by the GPU.
		virtual void FenceSignal( RHIFence a_Fence ) override;
		//=====================================================

		//=====================================================
		// Resource creation
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc ) override;
		virtual RHIIndexBufferRef CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc ) override;
		virtual RHIVertexBufferRef CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc ) override;
		virtual RHIPipelineStateRef CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		const auto& GetFactory() const { return m_DXGIFactory; }
		const auto& GetDevice() const { return m_Device; }
		const auto& GetCommandQueue() const { return m_CommandQueue; }
		const auto& GetCommandAllocator() const { return m_CommandAllocator; }
		const auto& GetCommandList() const { return m_CommandList; }

		//====================================================

	private:
		ComPtr<DX12::Factory> m_DXGIFactory = nullptr;
		ComPtr<DX12::Device> m_Device = nullptr;
		ComPtr<DX12::CommandQueue> m_CommandQueue = nullptr;
		ComPtr<DX12::CommandAllocator> m_CommandAllocator = nullptr;
		ComPtr<DX12::GraphicsCommandList> m_CommandList = nullptr;
		ComPtr<DX12::Fence> m_Fence = nullptr;
		DX12::FenceValue m_FenceValue = 0;
		void* m_FenceEvent = nullptr;

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<DX12::D3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<DX12::DXGIDebug> m_DXGIDebug = nullptr;
	#endif
	};

}