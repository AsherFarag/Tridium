#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIMesh.h"
#include "RHIPipelineState.h"
#include "RHIShader.h"
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
		//===========================
	}

}
