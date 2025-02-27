#pragma once
#include "RHIDefinitions.h"
#include <Tridium/Containers/String.h>

namespace Tridium {

	using RHISwapChainRef = SharedPtr<class RHISwapChain>;

	//===========================
	// RHI Globals
	//  A struct to hold global RHI information.
	//===========================
	struct RHIGlobals
	{
		// True if the rendering hardware interface has been initialised.
		bool IsRHIInitialised = false;

		// The RHI configuration.
		RHIConfig Config;

		// The swap chain instance used by the RHI.
		// This is set by the RHI implementation automatically but can be overridden by the user.
		// An example for overriding the swap chain is to use a custom swap chain for VR.
		RHISwapChainRef SwapChain;

		//====================================================
		// RHI Query Information
		//====================================================

		// Whether the RHI supports being able to send commands from multiple threads.
		bool SupportsMultithreading = false;

		struct GPUInfo
		{
			String AdapterName;
			String AdapterInternalDriverVersion;
			String AdapterUserDriverVersion;
			String AdapterDriverDate;
			uint32_t DeviceID = 0u;
			uint32_t VendorID = 0u;
		} GPUInfo;
	};

	extern RHIGlobals s_RHIGlobals;
	
	// The time it took the GPU to render the last frame.
	extern uint32_t s_GPUFrameTime;

	namespace RHI {

		inline const RHISwapChainRef& GetSwapChain()
		{
			return s_RHIGlobals.SwapChain;
		}

		inline void SetSwapChain( const RHISwapChainRef& a_SwapChain )
		{
			s_RHIGlobals.SwapChain = a_SwapChain;
		}

	}
}