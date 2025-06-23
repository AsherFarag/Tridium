#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include "RHIResource.h"
#include "RHISwapChain.h"

namespace Tridium {

	//==============================================
	// Forward declarations
	struct RHITextureSubresourceData;
	//==============================================

	//===========================
	// RHI Configuration
	struct RHIConfig
	{
		// The RHI backend to use.
		ERHInterfaceType RHIType = ERHInterfaceType::Null;

		// Enables debug features for the RHI.
		// NOTE: This option does nothing if RHI_DEBUG_ENABLED is false.
		bool UseDebug = false;

		// The swap chain description used to create the swap chain.
		RHISwapChainDesc SwapChainDesc{};

		// Force single-threaded rendering.
		bool SingleThreaded = false;

		// This specifies how many frames the CPU can prepare while the GPU is rendering.
		uint32_t MaxFramesInFlight = RHIConstants::MaxFrameBuffers;

		constexpr auto& SetRHIType( ERHInterfaceType a_Type ) { RHIType = a_Type; return *this; }
		constexpr auto& SetUseDebug( bool a_UseDebug ) { UseDebug = a_UseDebug; return *this; }
		constexpr auto& SetSwapChainDesc( const RHISwapChainDesc& a_Desc ) { SwapChainDesc = a_Desc; return *this; }
		constexpr auto& SetSingleThreaded( bool a_SingleThreaded ) { SingleThreaded = a_SingleThreaded; return *this; }
		constexpr auto& SetMaxFramesInFlight( uint32_t a_MaxFramesInFlight ) { MaxFramesInFlight = a_MaxFramesInFlight; return *this; }
	};

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
		// Executes the given command lists and returns a fence value that can be used to wait for the commands to complete.
		virtual RHIFenceValue ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType ) = 0;
		// Waits for the RHI to finish processing all commands and become idle.
		virtual bool WaitForIdle() = 0;
		// Waits until the specified fence value is reached on the given command queue type.
		virtual void WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue ) = 0;
		// Runs garbage collection and cleans up references to RHI resources that are no longer in use by command lists.
		virtual void CollectGarbage() = 0;
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
		//=====================================================

		//=====================================================
		// Miscellaneous
		const auto& Config() const { return m_Config; }
		virtual IRHISwapChain* GetSwapChain() const = 0;
		virtual GPUInfo GetGPUInfo() const = 0;
		const auto& RegisteredResources() const { return m_RegisteredResources; }

		virtual void RegisterRHIObject( IRHIObject& a_Resource ) 
		{ 
			m_RegisteredResources.emplace( std::hash<IRHIObject*>()( &a_Resource ), a_Resource.Weak() );
		}

		virtual bool UnregisterRHIObject( IRHIObject& a_Resource ) 
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
		RHIConfig m_Config{}; // Configuration used to initialise the RHI
		UnorderedMap<size_t, RHIObjectWeakRef> m_RegisteredResources{}; // Resources registered with the RHI
	};

	namespace Concepts {
		template<typename T>
		concept IsDynamicRHI = std::is_base_of_v<IDynamicRHI, T>;
	}


} // namespace Tridium