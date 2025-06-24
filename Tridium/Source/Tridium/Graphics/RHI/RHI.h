#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIVertexLayout.h"
#include "RHIPipelineState.h"
#include "ShaderLibrary.h"
#include "RHIBuffer.h"
#include "RHICommandList.h"
#include "RHIShaderBindings.h"
#include "RHISwapChain.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		bool Initialise( const RHIConfig& a_Config );
		// Shutdown the RHI.
		bool Shutdown();
		// Begins a new frame, incrementing the frame index.
		void BeginFrame();
		// Ends the current frame.
		void EndFrame();
		// Present the current frame to the screen using the swap chain.
		bool Present();
		// Executes the given command lists and returns a fence value that can be used to wait for the commands to complete.
		RHIFenceValue ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType );
		// Waits for the RHI to finish processing all commands and become idle.
		bool WaitForIdle();
		// Waits until the specified fence value is reached on the given command queue type.
		void WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue );
		// Runs garbage collection and cleans up references to RHI resources that are no longer in use by command lists.
		void CollectGarbage();
		//===========================

		//===========================
		// RHI Query and Functions
		[[nodiscard]] inline uint32_t MaxFramesInFlight() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->MaxFramesInFlight(); }
		[[nodiscard]] inline uint32_t FrameIndex() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->FrameIndex(); }
		[[nodiscard]] inline const RHIConfig& Config() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->Config(); }
		[[nodiscard]] inline ERHInterfaceType GetRHIType() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->GetRHIType(); }
		[[nodiscard]] inline IRHISwapChain* GetSwapChain() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->GetSwapChain(); }
		[[nodiscard]] inline GPUInfo GetGPUInfo() { RHI_DEV_CHECK( s_DynamicRHI, "Null RHI" ); return s_DynamicRHI->GetGPUInfo(); }
		//===========================

		//===========================
		// Resource creation
		[[nodiscard]] RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		[[nodiscard]] RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data = {} );
		[[nodiscard]] RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc );
		[[nodiscard]] RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc );
		[[nodiscard]] RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc );
		[[nodiscard]] RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc );
		[[nodiscard]] RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc );
		[[nodiscard]] RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc );

		// Wrappers

		[[nodiscard]] inline RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, RHITextureSubresourceData a_SubResourcesData )
		{
			return CreateTexture( a_Desc, Span<RHITextureSubresourceData>{ &a_SubResourcesData, 1 } );
		}

		// Executes the given command lists and returns a fence value that can be used to wait for the commands to complete.
		inline RHIFenceValue ExecuteCommandLists( IRHICommandList* const* a_CommandLists, size_t a_Count, ERHICommandQueueType a_QueueType )
		{
			return ExecuteCommandLists( Span<IRHICommandList* const>{ a_CommandLists, a_Count }, a_QueueType );
		}

		//===========================
	}

}
