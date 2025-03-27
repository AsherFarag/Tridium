#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

namespace Tridium {

	//==============================================
	// Forward declarations
	FORWARD_DECLARE_RHI_RESOURCE( Sampler );
	FORWARD_DECLARE_RHI_RESOURCE( Texture );
	FORWARD_DECLARE_RHI_RESOURCE( IndexBuffer );
	FORWARD_DECLARE_RHI_RESOURCE( VertexBuffer );
	FORWARD_DECLARE_RHI_RESOURCE( GraphicsPipelineState );
	FORWARD_DECLARE_RHI_RESOURCE( CommandList );
	FORWARD_DECLARE_RHI_RESOURCE( ShaderModule );
	FORWARD_DECLARE_RHI_RESOURCE( ShaderBindingLayout );
	FORWARD_DECLARE_RHI_RESOURCE( SwapChain );
	//==============================================

	//==============================================
	// DynamicRHI Interface
	//  The core interace for the dynamicly bound RHI.
	//==============================================
	class IDynamicRHI
	{
	public:
		IDynamicRHI() = default;
		virtual ~IDynamicRHI() = default;

		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) = 0;
		// Shutdown the RHI.
		virtual bool Shutdown() = 0;
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
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) = 0;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc ) = 0;
		virtual RHIIndexBufferRef CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc ) = 0;
		virtual RHIVertexBufferRef CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc ) = 0;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) = 0;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) = 0;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) = 0;
		virtual RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc ) = 0;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) = 0;
		//=====================================================

		#if RHI_DEBUG_ENABLED

		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() {}

		#endif // RHI_DEBUG_ENABLED
	};

	//==============================================
	// The global dynamic RHI instance.
	// Defined in RHI.cpp
	extern IDynamicRHI* s_DynamicRHI;
	//==============================================

	namespace Concepts {
		template<typename T>
		concept IsDynamicRHI = std::is_base_of_v<IDynamicRHI, T>;
	}


} // namespace Tridium