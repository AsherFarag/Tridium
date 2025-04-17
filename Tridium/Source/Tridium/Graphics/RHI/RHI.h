#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIVertexLayout.h"
#include "RHIPipelineState.h"
#include "ShaderLibrary.h"
#include "RHIBuffer.h"
#include "RHICommandList.h"
#include "RHIShaderBindingLayout.h"
#include "RHISwapChain.h"
#include "RHIFence.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		
		// Initialise the RHI with the given configuration.
		bool Initialise( const RHIConfig& a_Config );
		// Shutdown the RHI.
		bool Shutdown();
		// Present the current frame to the screen using the swap chain.
		bool Present();
		// Execute the given command list on the GPU.
		bool ExecuteCommandList( const RHICommandListRef& a_CommandList );

		// Wait for the frame fence to complete.
		void FrameFenceWait();
		//===========================

		//===========================
		// RHI Query and Functions
		uint32_t FrameIndex();
		RHIFeatureInfo GetFeatureInfo( ERHIFeature a_Feature );
		ERHIFeatureSupport GetFeatureSupport( ERHIFeature a_Feature );
		bool IsFeatureSupported( ERHIFeature a_Feature );
		//===========================

		//===========================
		// Resource creation
		[[nodiscard]] RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc );
		[[nodiscard]] RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc );
		[[nodiscard]] RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		[[nodiscard]] RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data = {} );
		[[nodiscard]] RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc );
		[[nodiscard]] RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc );
		[[nodiscard]] RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc );
		[[nodiscard]] RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc );
		[[nodiscard]] RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc );

		// Wrappers

		[[nodiscard]] inline RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, RHITextureSubresourceData a_SubResourcesData )
		{
			return CreateTexture( a_Desc, Span<RHITextureSubresourceData>{ &a_SubResourcesData, 1 } );
		}

		inline RHIFenceRef CreateFence( ERHIFenceType a_Type = ERHIFenceType::CPUWaitOnly, StringView a_Name = {} )
		{
			return CreateFence( RHIFenceDescriptor{ a_Name, a_Type } );
		}
		//===========================
	}

}
