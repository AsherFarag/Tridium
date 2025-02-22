#pragma once
#include "D3D12Common.h"
#include "D3D12Context.h"

namespace Tridium {

	class D3D12RHI final : public IDynamicRHI
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
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::DirectX12; }
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
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		const auto& GetFactory() const { return m_DXGIFactory; }
		const auto& GetDevice() const { return m_Device; }
		const auto& GetCommandQueue() const { return m_CommandQueue; }
		const auto& GetCommandAllocator() const { return m_CommandAllocator; }
		const auto& GetCommandList() const { return m_CommandList; }
		const auto& GetFence() const { return m_Fence; }
		D3D12::FenceValue& GetFenceValue() { return m_FenceValue; }
		HANDLE GetFenceEvent() const { return m_FenceEvent; }

		//====================================================

	private:
		ComPtr<D3D12::Factory> m_DXGIFactory = nullptr;
		ComPtr<D3D12::Device> m_Device = nullptr;
		ComPtr<D3D12::CommandQueue> m_CommandQueue = nullptr;
		ComPtr<D3D12::CommandAllocator> m_CommandAllocator = nullptr;
		ComPtr<D3D12::GraphicsCommandList> m_CommandList = nullptr;
		ComPtr<D3D12::Fence> m_Fence = nullptr;
		D3D12::FenceValue m_FenceValue = 0;
		HANDLE m_FenceEvent = nullptr;

		//=====================================================

		TODO( "Possibly move this to a separate class" );
		struct WindowData
		{
			static constexpr uint32_t s_FrameCount = 2;

			ComPtr<D3D12::SwapChain> SwapChain = nullptr;
			ComPtr<D3D12::Resource> Buffers[s_FrameCount] = { nullptr, nullptr };
			uint32_t BufferIndex = 0;

			ComPtr<D3D12::DescriptorHeap> RTVDescHeap = nullptr;
			D3D12_CPU_DESCRIPTOR_HANDLE RTVHandles[s_FrameCount];

			bool Initialize( D3D12RHI& a_RHI );
		} m_WindowData;

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<D3D12::D3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<D3D12::DXGIDebug> m_DXGIDebug = nullptr;
	#endif
	};

	namespace RHI {
		static D3D12RHI* GetD3D12RHI()
		{
		#if RHI_DEBUG_ENABLED
			if ( s_DynamicRHI->GetRHIType() != ERHInterfaceType::DirectX12 )
			{
				ASSERT_LOG( false, "The current RHI is not DirectX 12!" );
				return nullptr;
			}
		#endif
			return static_cast<D3D12RHI*>( s_DynamicRHI );
		}
	}

}