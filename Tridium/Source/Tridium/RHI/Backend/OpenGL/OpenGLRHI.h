#pragma once
#include <Tridium/RHI/RHICommon.h>
#include <Tridium/RHI/DynamicRHI.h>
#include <Tridium/Core/Containers/Array.h>
#include <Tridium/Core/Containers/String.h>

// Resources
#include <Tridium/RHI/RHICommandList.h>
#include <Tridium/RHI/RHITexture.h>
#include <Tridium/RHI/RHIMesh.h>
#include <Tridium/RHI/RHIPipelineState.h>


namespace Tridium {

	class OpenGLRHI final : public DynamicRHI
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
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::OpenGL; }
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

	#if RHI_DEBUG_ENABLED
		virtual void DumpDebug() override;
	#endif // RHI_DEBUG_ENABLED
	};

}