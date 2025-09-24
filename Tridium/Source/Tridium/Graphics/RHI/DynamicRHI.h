#pragma once
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Graphics/RHI/RHIResource.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>

namespace Tridium {

	//=================================================================================================
	// RHI Stats: Contains various statistics about the RHI usage.
	//=================================================================================================
	struct RHIStats
	{
		//=============================================================================================
		// The number of frames that have been rendered since the RHI was initialised.
		// This is set by IDynamicRHI.
		uint64_t Frame = 0;

		//=============================================================================================
		// The number of live RHI objects (textures, buffers, pipelines, etc).
		// This is set by IDynamicRHI.
		uint32_t NumRHIObjects = 0;

		//=============================================================================================
		// The number of RHI objects created this frame.
		// This is set by IDynamicRHI.
		uint64_t NumRHIObjectsCreatedThisFrame = 0;

		//=============================================================================================
		// Statistics regarding descriptor heaps used by the RHI.
		// NOTE: Not all RHIs use descriptor heaps, so this is optional.
		struct DescriptorHeapStats
		{
			//=========================================================================================
			// What type of descriptor heap this is.
			ERHIDescriptorHeapType Type = ERHIDescriptorHeapType::Unknown;

			//=========================================================================================
			// The number of allocated descriptors in the heap.
			uint32_t NumAllocatedDescriptors = 0;

			//=========================================================================================
			// The number of descriptor allocations.
			uint32_t NumAllocations = 0;

			//=========================================================================================
			// The size, in bytes, of the heap.
			size_t HeapSizeInBytes = 0;
		};
		Array<DescriptorHeapStats> HeapStats;

		//=============================================================================================
		// Resets the stats to their default values.
		void Reset()
		{
			Frame = 0;
			NumRHIObjects = 0;
			NumRHIObjectsCreatedThisFrame = 0;
			HeapStats.Clear();
		}
	};

	//=================================================================================================
	// RHI Configuration: Contains the configuration that is used to initialise the RHI.
	//=================================================================================================
	struct RHIConfig
	{
		//=============================================================================================
		// The RHI backend to use.
		ERHInterfaceType RHIType = ERHInterfaceType::Null;

		//=============================================================================================
		// Enables debug features for the RHI.
		// NOTE: This option does nothing if RHI_DEBUG_ENABLED is false.
		bool UseDebug = false;

		//=============================================================================================
		// The swap chain description used to create the swap chain.
		RHISwapChainDesc SwapChainDesc{};

		//=============================================================================================
		// Force single-threaded rendering.
		bool SingleThreaded = false;

		//=============================================================================================
		// This specifies how many frames the CPU can prepare while the GPU is rendering.
		uint32_t MaxFramesInFlight = RHIConstants::MaxFramesInFlight;

		//=============================================================================================
		constexpr auto& SetRHIType( ERHInterfaceType a_Type ) { RHIType = a_Type; return *this; }
		constexpr auto& SetUseDebug( bool a_UseDebug ) { UseDebug = a_UseDebug; return *this; }
		constexpr auto& SetSwapChainDesc( const RHISwapChainDesc& a_Desc ) { SwapChainDesc = a_Desc; return *this; }
		constexpr auto& SetSingleThreaded( bool a_SingleThreaded ) { SingleThreaded = a_SingleThreaded; return *this; }
		constexpr auto& SetMaxFramesInFlight( uint32_t a_MaxFramesInFlight ) { MaxFramesInFlight = a_MaxFramesInFlight; return *this; }
	};

	//=================================================================================================
	// DynamicRHI Interface: The core interface for the dynamically bound RHI.
	//=================================================================================================
	class IDynamicRHI
	{
	public:

		//=============================================================================================
		IDynamicRHI() = default;
		virtual ~IDynamicRHI() = default;

		//=============================================================================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) = 0;
		// Shutdown the RHI.
		virtual bool Shutdown() = 0;
		// Begins a new frame, incrementing the frame index.
		virtual void BeginFrame();
		// Ends the current frame.
		virtual void EndFrame();
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
		//=============================================================================================

		//=============================================================================================
		// Resource creation
		virtual RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<struct RHITextureSubresourceData> a_SubResourcesData ) = 0;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data ) = 0;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc ) = 0;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc ) = 0;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc ) = 0;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc ) = 0;
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc ) = 0;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc ) = 0;
		//=============================================================================================

		//=============================================================================================
		// Miscellaneous
		// Returns the total number of frames that have been rendered since the RHI was initialised.
		uint64_t FrameCount() const { return m_FrameCount; }
		// Returns the maximum number of frames in flight, specified in the RHI configuration.
		uint32_t MaxFramesInFlight() const { return m_Config.MaxFramesInFlight; }
		// Returns the current frame index.
		uint32_t FrameIndex() const { return m_FrameCount % MaxFramesInFlight(); }
		// Returns the configuration used to initialise the RHI.
		const auto& Config() const { return m_Config; }
		// Returns the swap chain associated with the RHI.
		virtual IRHISwapChain* GetSwapChain() const = 0;
		// Returns information about the GPU.
		virtual GPUInfo GetGPUInfo() const = 0;
		// Returns all currently registered RHI objects.
		const auto& RegisteredRHIObjects() const { return m_RegisteredRHIObjects; }
		// Registers an RHI object with the RHI.
		virtual void RegisterRHIObject( IRHIObject& a_Resource );
		// Unregisters an RHI object from the RHI, if it was registered.
		virtual bool UnregisterRHIObject( IRHIObject& a_Resource );
		// Returns statistics about the RHI usage at this current moment.
		virtual bool QueryRHIStats( RHIStats& o_Stats ) const;
		//=============================================================================================

	#if RHI_DEBUG_ENABLED

		//=============================================================================================
		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() {}

	#endif // RHI_DEBUG_ENABLED

	protected:

		//=============================================================================================
		// Configuration used to initialise the RHI
		RHIConfig m_Config{};
		// Resources registered with the RHI
		UnorderedMap<size_t, RHIObjectWeakRef> m_RegisteredRHIObjects{};
		// Total number of objects created since RHI::BeginFrame was last called.
		uint64_t m_ObjectsCreatedThisFrame = 0;
		// Total number of frames rendered since the RHI was initialised
		uint64_t m_FrameCount = 0;

	};

	inline void IDynamicRHI::BeginFrame()
	{
		m_ObjectsCreatedThisFrame = 0;
	}

	inline void IDynamicRHI::EndFrame()
	{
		++m_FrameCount;
	}

	inline void IDynamicRHI::RegisterRHIObject( IRHIObject& a_Resource )
	{
		m_RegisteredRHIObjects.emplace( std::hash<IRHIObject*>()(&a_Resource), a_Resource.Weak() );
		++m_ObjectsCreatedThisFrame;
	}

	inline bool IDynamicRHI::UnregisterRHIObject( IRHIObject& a_Resource )
	{
		if ( auto it = m_RegisteredRHIObjects.find( std::hash<IRHIObject*>()(&a_Resource) ); it != m_RegisteredRHIObjects.end() )
		{
			m_RegisteredRHIObjects.erase( it );
			return true;
		}
		return false;
	}

	inline bool IDynamicRHI::QueryRHIStats( RHIStats& o_Stats ) const
	{
		o_Stats.Reset();
		o_Stats.Frame = m_FrameCount;
		o_Stats.NumRHIObjects = Cast<uint32_t>( m_RegisteredRHIObjects.size() );
		o_Stats.NumRHIObjectsCreatedThisFrame = m_ObjectsCreatedThisFrame;
		return true;
	}

} // namespace Tridium