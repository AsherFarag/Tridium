#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

namespace Tridium {

	//==============================================
	// Forward declarations
	// - Resource Types
	class RHITexture;
	using RHITextureRef = SharedPtr<RHITexture>;
	struct RHITextureDescriptor;
	class RHIIndexBuffer;
	using RHIIndexBufferRef = SharedPtr<RHIIndexBuffer>;
	struct RHIIndexBufferDescriptor;
	class RHIVertexBuffer;
	using RHIVertexBufferRef = SharedPtr<RHIVertexBuffer>;
	struct RHIVertexBufferDescriptor;
	class RHIPipelineState;
	using RHIPipelineStateRef = SharedPtr<RHIPipelineState>;
	struct RHIPipelineStateDescriptor;
	class RHICommandList;
	using RHICommandListRef = SharedPtr<RHICommandList>;
	struct RHICommandListDescriptor;
	class RHIShaderModule;
	using RHIShaderModuleRef = SharedPtr<RHIShaderModule>;
	struct RHIShaderModuleDescriptor;
	//==============================================

	//==============================================
	// DynamicRHI
	//  The core interace for the dynamicly bound RHI.
	//==============================================
	class DynamicRHI
	{
	public:
		DynamicRHI() = default;
		virtual ~DynamicRHI() = default;

		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) = 0;
		// Shutdown the RHI.
		virtual bool Shutdown() = 0;
		// Present the current frame.
		virtual bool Present() = 0;
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) = 0;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const = 0;
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::Null; }
		//==============================================

		//====================================================
		// Creates a fence that can be used to synchronize the CPU and GPU.
		virtual RHIFence CreateFence() const = 0;
		// Queries the state of a fence.
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const = 0;
		// Blocks the calling CPU thread until the fence is signaled by the GPU.
		virtual void FenceSignal( RHIFence a_Fence ) = 0;
		//=====================================================

		//=====================================================
		// Resource creation
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc ) = 0;
		virtual RHIIndexBufferRef CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc ) = 0;
		virtual RHIVertexBufferRef CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc ) = 0;
		virtual RHIPipelineStateRef CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc ) = 0;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) = 0;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) = 0;
		//=====================================================

		#if RHI_DEBUG_ENABLED

		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() {}

		#endif // RHI_DEBUG_ENABLED
	};

	//==============================================
	// The global dynamic RHI instance.
	// Defined in RHI.cpp
	extern DynamicRHI* s_DynamicRHI;
	//==============================================

	namespace Concepts {
		template<typename T>
		concept IsDynamicRHI = std::is_base_of_v<DynamicRHI, T>;
	}


} // namespace Tridium