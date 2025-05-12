#pragma once
#include "D3D12Common.h"
#include "D3D12Context.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12Fence.h"
#include "Utils/D3D12UploadBuffer.h"
#include "Utils/ThirdParty/D3D12MemAlloc.h"

namespace Tridium {

	struct D3D12TierInfo
	{
		size_t MaxShaderVisibleDescriptorHeapSize = 0;
		size_t MaxShaderVisibleSamplerHeapSize = 0;
	};

	constexpr struct
	{
		const D3D12TierInfo Tier1 = {
			.MaxShaderVisibleDescriptorHeapSize = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1,
			.MaxShaderVisibleSamplerHeapSize = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE
		};
		const D3D12TierInfo Tier2 = {
			.MaxShaderVisibleDescriptorHeapSize = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
			.MaxShaderVisibleSamplerHeapSize = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE
		};
		const D3D12TierInfo Tier3 = {
			.MaxShaderVisibleDescriptorHeapSize = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
			.MaxShaderVisibleSamplerHeapSize = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE
		};
	} D3D12Tiers;

	namespace D3D12 {
		struct DeferredDeleteObject {};

		inline const D3D12TierInfo& TierInfo() { return D3D12Tiers.Tier2; }
	}

	//======================================================================
	// D3D12 Command Context
	//  Owns and manages the command queue, command allocator, command list and fence.
	struct D3D12CommandContext
	{
		ComPtr<ID3D12CommandQueue> CmdQueue = nullptr;
		ComPtr<ID3D12CommandAllocator> CmdAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> CmdList = nullptr;

		ComPtr<ID3D12Fence1> Fence = nullptr;
		HANDLE FenceEvent = nullptr;
		uint64_t FenceValue = 0;

		~D3D12CommandContext()
		{
			if ( CmdList ) CmdList->Release();
			if ( CmdAllocator ) CmdAllocator->Reset();
			if ( CmdQueue ) CmdQueue->Signal( Fence.Get(), FenceValue );
			if ( FenceEvent ) CloseHandle( FenceEvent );
		}

		[[nodiscard]] bool IsFenceComplete( uint64_t a_FenceValue ) const
		{
			return CmdQueue && (Fence->GetCompletedValue() >= a_FenceValue);
		}

		[[nodiscard]] uint64_t Signal()
		{
			if ( CmdQueue )
			{
				CmdQueue->Signal( Fence.Get(), ++FenceValue );
				return FenceValue;
			}
			return 0;
		}

		void Wait( uint64_t a_FenceValue )
		{
			if ( Fence->GetCompletedValue() < a_FenceValue )
			{
				Fence->SetEventOnCompletion( a_FenceValue, FenceEvent );
				WaitForSingleObject( FenceEvent, INFINITE );
			}
		}
	};



	//======================================================================
	// D3D12 RHI
	//  DirectX12 Dynamic RHI implementation.
	class D3D12RHI final : public IDynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) override;
		// Shutdown the RHI.
		virtual bool Shutdown() override;
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::DirectX12; }
		//==============================================

		//=====================================================
		// Resource creation
		virtual RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc ) override;
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) override;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data ) override;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) override;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) override;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		auto& GetCommandContext( ERHICommandQueueType a_Type )
		{
			ASSERT( a_Type < ERHICommandQueueType::COUNT, "Invalid command queue type!" );
			return m_CmdContexts[static_cast<size_t>( a_Type )];
		}

		const auto& GetCommandContext( ERHICommandQueueType a_Type ) const
		{
			ASSERT( a_Type < ERHICommandQueueType::COUNT, "Invalid command queue type!" );
			return m_CmdContexts[static_cast<size_t>( a_Type )];
		}

		const auto& GetDXGIFactory() const { return m_DXGIFactory; }
		const auto& GetDXGIAdapter() const { return m_DXGIAdapter; }
		const auto& GetAllocator() const { return m_Allocator; }
		const auto& GetUploadBuffer() const { return m_UploadBuffer; }
		auto& GetUploadBuffer() { return m_UploadBuffer; }
		size_t GetNextCommandListIndex() { return m_NextCommandListIndex; }

		bool SupportsDeviceVersion( uint32_t a_Version ) const { return m_MaxD3D12DeviceVersion >= a_Version; }
		ID3D12Device* GetD3D12Device() const { return m_Device.Get(); }

#define GET_D3D12_DEVICE( _Version ) \
		ID3D12Device##_Version* GetD3D12Device##_Version() const \
		{ \
			RHI_DEV_CHECK( m_MaxD3D12DeviceVersion >= _Version, "D3D12Device" #_Version " is not supported! Max supported version is: ", m_MaxD3D12DeviceVersion ); \
			return static_cast<ID3D12Device##_Version*>( m_Device.Get() ); \
		}

		GET_D3D12_DEVICE( 1 );
		GET_D3D12_DEVICE( 2 );
		GET_D3D12_DEVICE( 3 );
		GET_D3D12_DEVICE( 4 );
		GET_D3D12_DEVICE( 5 );

#undef GET_D3D12_DEVICE

		D3D12::DescriptorHeapManager& GetDescriptorHeapManager() { return m_DescriptorHeapManager; }
		const D3D12::DescriptorHeapManager& GetDescriptorHeapManager() const { return m_DescriptorHeapManager; }

		template<typename T>
		void DeferredDelete( const T& a_Object )
		{
			LOG( LogCategory::DirectX, Debug, "Deferred delete object." );
			m_ObjectsToDelete.EmplaceBack();
		}

		//====================================================

	private:
		ComPtr<IDXGIAdapter> m_DXGIAdapter = nullptr;
		ComPtr<IDXGIFactory> m_DXGIFactory = nullptr;
		ComPtr<ID3D12Device> m_Device = nullptr;
		ComPtr<D3D12MA::Allocator> m_Allocator = nullptr;
		D3D12::DescriptorHeapManager m_DescriptorHeapManager{};

		uint32_t m_MaxD3D12DeviceVersion = 0;

		size_t m_NextCommandListIndex = 0;
		Array<D3D12::DeferredDeleteObject> m_ObjectsToDelete{};
		D3D12::UploadBuffer m_UploadBuffer{};
		FixedArray<D3D12CommandContext, size_t( ERHICommandQueueType::COUNT )> m_CmdContexts{};

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
	#endif
	};

	static D3D12RHI* GetD3D12RHI()
	{
	#if RHI_DEBUG_ENABLED
		if ( s_DynamicRHI->GetRHIType() != ERHInterfaceType::DirectX12 )
		{
			ASSERT( false, "The current RHI is not DirectX 12!" );
			return nullptr;
		}
	#endif
		return static_cast<D3D12RHI*>( s_DynamicRHI );
	}

}