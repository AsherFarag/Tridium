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
		struct DeferredDeleteObject
		{
		};

		inline const D3D12TierInfo& TierInfo() { return D3D12Tiers.Tier2; }
	}

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
		virtual RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		const auto& GetFactory() const { return m_DXGIFactory; }
		const auto& GetDevice() const { return m_Device; }
		const auto& GetCommandQueue() const { return m_CommandQueue; }
		const auto& GetCommandAllocator() const { return m_CommandAllocator; }
		const auto& GetCommandList() const { return m_CommandList; }
		const auto& GetAllocator() const { return m_Allocator; }
		const auto& GetUploadBuffer() const { return m_UploadBuffer; }
		auto& GetUploadBuffer() { return m_UploadBuffer; }
		size_t GetNextCommandListIndex() { return m_NextCommandListIndex; }

		const auto& GetCopyQueue() const { return m_CopyQueue; }
		const auto& GetCopyCmdAllocator() const { return m_CopyAllocator; }
		const auto& GetCopyCmdList() const { return m_CopyCommandList; }
		const auto& GetCopyFence() const { return m_CopyFence; }
		uint64_t IncrementCopyFence() { return ++m_CopyFenceValue; }

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
		ComPtr<ID3D12::Factory> m_DXGIFactory = nullptr;
		ComPtr<ID3D12::Device> m_Device = nullptr;
		ComPtr<IDXGIAdapter> m_Adapter = nullptr;
		ComPtr<ID3D12::CommandQueue> m_CommandQueue = nullptr;
		ComPtr<ID3D12::CommandAllocator> m_CommandAllocator = nullptr;
		ComPtr<ID3D12::GraphicsCommandList> m_CommandList = nullptr;
		ComPtr<D3D12MA::Allocator> m_Allocator = nullptr;
		D3D12::DescriptorHeapManager m_DescriptorHeapManager{};
		size_t m_NextCommandListIndex = 0;
		Array<D3D12::DeferredDeleteObject> m_ObjectsToDelete{};
		D3D12::UploadBuffer m_UploadBuffer{};

		TODO( "Most likely temp" );
		RHIFenceRef m_Fence = nullptr;
		uint64_t m_FenceValue = 0;

		ComPtr<ID3D12CommandQueue> m_CopyQueue = nullptr;
		ComPtr<ID3D12CommandAllocator> m_CopyAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> m_CopyCommandList = nullptr;
		D3D12FenceRef m_CopyFence = nullptr;
		uint64_t m_CopyFenceValue = 0;

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12::D3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<ID3D12::DXGIDebug> m_DXGIDebug = nullptr;
	#endif
	};

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