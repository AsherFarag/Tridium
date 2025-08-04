#pragma once
#include <Tridium/Utils/Log.h>
#include <Tridium/Containers/Deque.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindings.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIDescriptorAllocator.h>
#include <Tridium/Graphics/RHI/RHIStateTracker.h>

#include <Vulkan/vulkan.h>

namespace Tridium::Vulkan {

	//================================
	// Forward declarations
	class RHITexture_VulkanImpl;
	class RHIBuffer_VulkanImpl;
	class RHISampler_VulkanImpl;
	class RHIShaderModule_VulkanImpl;
	class RHISwapChain_VulkanImpl;
	class RHIGraphicsPipelineState_VulkanImpl;
	class RHICommandList_VulkanImpl;
	class DynamicRHI_VulkanImpl;
	//=================================

	//======================================================================
	StringView GetVulkanErrorString( VkResult a_Result );

	//======================================================================
	// DYNAMIC RHI IMPLEMENTATION
	//======================================================================

	class DynamicRHI_VulkanImpl final : public IDynamicRHI
	{
	public:
		//==============================================           
		// Core RHI function
		bool Init( const RHIConfig& a_Config ) override;
		bool Shutdown() override;
		void BeginFrame() override {}
		void EndFrame() override {}
		RHIFenceValue ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType ) override { return 0; }
		bool WaitForIdle() override { return true; }
		void WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue ) override {}
		void CollectGarbage() override {}
		ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::DirectX12; }
		//==============================================

		//=====================================================
		// Resource creation
		virtual RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override { return nullptr; }
		virtual RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data ) override {return nullptr;}
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc ) override {return nullptr;}
		virtual RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc ) override {return nullptr;}
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc ) override {return nullptr;}
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc ) override {return nullptr;}
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc ) override {return nullptr;}
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc ) override { return nullptr; }
		//=====================================================

		//=====================================================
		// Miscellaneous
		IRHISwapChain* GetSwapChain() const override { return nullptr; }
		virtual GPUInfo GetGPUInfo() const override { return {}; }
		//=====================================================

	protected:

		VkInstance m_Instance = VK_NULL_HANDLE; // Vulkan instance handle

	};
}