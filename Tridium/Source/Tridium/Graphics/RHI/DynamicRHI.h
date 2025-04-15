#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

namespace Tridium {

	//==============================================
	// Forward declarations
	struct RHITextureSubresourceData;
	FORWARD_DECLARE_RHI_RESOURCE( RHISampler );
	FORWARD_DECLARE_RHI_RESOURCE( RHITexture );
	FORWARD_DECLARE_RHI_RESOURCE( RHIBuffer );
	FORWARD_DECLARE_RHI_RESOURCE( RHIGraphicsPipelineState );
	FORWARD_DECLARE_RHI_RESOURCE( RHICommandList );
	FORWARD_DECLARE_RHI_RESOURCE( RHIShaderModule );
	FORWARD_DECLARE_RHI_RESOURCE( RHIShaderBindingLayout );
	FORWARD_DECLARE_RHI_RESOURCE( RHISwapChain );
	FORWARD_DECLARE_RHI_RESOURCE( RHIFence );
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

		//=====================================================
		// Resource creation
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) = 0;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) = 0;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data ) = 0;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) = 0;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) = 0;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) = 0;
		virtual RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc ) = 0;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) = 0;
		virtual RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc ) = 0;
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