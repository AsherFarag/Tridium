#pragma once
#include "RHIDefinitions.h"
#include "RHIConstants.h"
#include <Tridium/Containers/String.h>

namespace Tridium {

	using RHISwapChainRef = SharedPtr<class RHISwapChain>;
	using RHIFenceRef = SharedPtr<class RHIFence>;

	//===========================
	// RHI Globals
	//  A struct to hold global RHI information.
	//===========================
	struct RHIGlobals
	{
		// True if the rendering hardware interface has been initialised.
		bool IsRHIInitialised = false;

		// The RHI configuration.
		RHIConfig Config{};

		// Static information about the GPU.
		GPUInfo GPUInfo{};

		// The swap chain instance used by the RHI.
		// This is set by the RHI implementation automatically but can be overridden by the user.
		// An example for overriding the swap chain is to use a custom swap chain for VR.
		// Initialised by RHI::Initialise.
		RHISwapChainRef SwapChain{};

		// The fence instance used by the RHI.
		// Initialised by RHI::Initialise.
		RHIFenceRef Fence{};

		// Fence values for each frame.
		uint64_t FrameFenceValue{};

		// The current frame index of the frame buffer.
		// This will never exceed RHIConstants::MaxFrameBuffers.
		uint32_t FrameIndex = 0u;

		//====================================================
		// RHI Query Information
		//====================================================

		// Whether the RHI supports being able to send commands from multiple threads.
		bool SupportsMultithreading = false;
	};

	extern RHIGlobals s_RHIGlobals;
	
	// The time it took the GPU to render the last frame.
	extern uint32_t s_GPUFrameTime;

	namespace RHI {

		inline bool IsInitialised()
		{
			return s_RHIGlobals.IsRHIInitialised;
		}

		inline const RHISwapChainRef& GetSwapChain()
		{
			return s_RHIGlobals.SwapChain;
		}

		inline void SetSwapChain( const RHISwapChainRef& a_SwapChain )
		{
			s_RHIGlobals.SwapChain = a_SwapChain;
		}

		inline const RHIFenceRef& GetGlobalFence()
		{
			return s_RHIGlobals.Fence;
		}

	}
}