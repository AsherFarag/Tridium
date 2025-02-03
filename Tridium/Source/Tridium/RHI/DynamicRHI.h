#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

namespace Tridium {

	// Forward declarations
	struct RHIConfig;
	class RHICommandList;
	using RHICommandListRef = SharedPtr<RHICommandList>;
	enum class ERHInterfaceType : uint8_t;

	//==============================================
	// DynamicRHI
	//  An interace for the dynamicly bound RHI.
	//==============================================
	class DynamicRHI
	{
	public:
		DynamicRHI() = default;
		virtual ~DynamicRHI() = default;

		virtual bool Init( const RHIConfig& a_Config ) = 0;
		virtual bool Shutdown() = 0;
		virtual bool Present() = 0;
		virtual ERHInterfaceType GetRHInterfaceType() const = 0;

		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) = 0;

		//====================================================
		// Creates a fence that can be used to synchronize the CPU and GPU.
		virtual RHIFence CreateFence() const = 0;

		// Queries the state of a fence.
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const = 0;

		// Blocks the calling CPU thread until the fence is signaled by the GPU.
		virtual void FenceSignal( RHIFence a_Fence ) = 0;
		//=====================================================

	#if RHI_DEBUG_ENABLED
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