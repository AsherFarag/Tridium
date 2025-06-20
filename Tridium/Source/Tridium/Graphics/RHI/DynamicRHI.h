#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include "RHIResource.h"

namespace Tridium {

	//==============================================
	// Forward declarations
	struct RHITextureSubresourceData;
	//==============================================

	//==============================================
	// DynamicRHI Interface
	//  The core interface for the dynamically bound RHI.
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
		virtual RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) = 0;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data ) = 0;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc ) = 0;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc ) = 0;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc ) = 0;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc ) = 0;
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc ) = 0;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc ) = 0;
		virtual RHIFenceRef CreateFence( const RHIFenceDesc& a_Desc ) = 0;
		//=====================================================

		//=====================================================
		// Miscellaneous
		virtual GPUInfo GetGPUInfo() const = 0;
		const auto& RegisteredResources() const { return m_RegisteredResources; }

		virtual void RegisterRHIResource( IRHIObject& a_Resource ) 
		{ 
			m_RegisteredResources.emplace( std::hash<IRHIObject*>()( &a_Resource ), a_Resource.Weak() );
		}

		virtual bool UnregisterRHIResource( IRHIObject& a_Resource ) 
		{
			if ( auto it = m_RegisteredResources.find( std::hash<IRHIObject*>()( &a_Resource ) ); it != m_RegisteredResources.end() )
			{
				m_RegisteredResources.erase( it );
				return true;
			}
			return false;
		}
		//=====================================================


		#if RHI_DEBUG_ENABLED

		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() {}

		#endif // RHI_DEBUG_ENABLED

	protected:
		UnorderedMap<size_t, RHIObjectWeakRef> m_RegisteredResources{}; // Resources registered with the RHI
	};

	namespace Concepts {
		template<typename T>
		concept IsDynamicRHI = std::is_base_of_v<IDynamicRHI, T>;
	}


} // namespace Tridium