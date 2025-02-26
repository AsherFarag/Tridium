#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIMesh.h"
#include "RHIPipelineState.h"
#include "ShaderLibrary.h"
#include "RHIBuffer.h"
#include "RHICommandList.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		
		// Initialise the RHI with the given configuration.
		bool Initialise( const RHIConfig& a_Config );
		// Shutdown the RHI.
		bool Shutdown();
		// Present the current frame.
		bool Present();
		// Execute the given command list on the GPU.
		bool ExecuteCommandList( const RHICommandListRef& a_CommandList );
		//===========================

		//===========================
		// Fence functions
	
		// Creates a fence that can be used to synchronize the CPU and GPU.
		RHIFence CreateFence();
		// Queries the state of a fence.
		ERHIFenceState GetFenceState( RHIFence a_Fence );
		// Blocks the calling CPU thread until the fence is signaled by the GPU.
		void FenceSignal( RHIFence a_Fence );
		//===========================

		//===========================
		// Resource creation
		RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc );
		RHIIndexBufferRef CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc );
		RHIVertexBufferRef CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc );
		RHIPipelineStateRef CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc );
		RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc );
		RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc );
		RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc );

		RHITextureRef CreateTexture2D( 
			uint32_t a_Width, uint32_t a_Height,
			Span<const uint8_t> a_Data,
			ERHITextureFormat a_Format = ERHITextureFormat::RGBA8,
			const char* a_Name = nullptr,
			ERHIUsageHint a_Usage = ERHIUsageHint::Default,
			SharedPtr<RHIResourceAllocator> a_Allocator = nullptr );

		RHIIndexBufferRef CreateIndexBuffer(
			Span<const Byte> a_InitialData,
			ERHIDataType a_DataType = ERHIDataType::UInt16,
			const char* a_Name = nullptr,
			ERHIUsageHint a_UsageHint = ERHIUsageHint::Default,
			SharedPtr<RHIResourceAllocator> a_Allocator = nullptr );

		RHIVertexBufferRef CreateVertexBuffer(
			Span<const Byte> a_InitialData,
			RHIVertexLayout a_Layout,
			const char* a_Name = nullptr,
			ERHIUsageHint a_UsageHint = ERHIUsageHint::Default,
			SharedPtr<RHIResourceAllocator> a_Allocator = nullptr );
		//===========================
	}

}
