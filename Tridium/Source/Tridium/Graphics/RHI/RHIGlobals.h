#pragma once
#include "RHIDefinitions.h"
#include "RHIConstants.h"
#include <Tridium/Containers/String.h>

namespace Tridium {

	//===========================
	// RHI Globals
	//  A struct to hold global RHI information.
	struct RHIGlobals
	{
		// True if the rendering hardware interface has been initialised.
		bool IsRHIInitialised = false;

		// Static information about the GPU.
		GPUInfo GPUInfo{};

		//====================================================
		// RHI Query Information
		//====================================================

		// The current frame index of the frame buffer.
		// This will never exceed RHIConstants::MaxFrameBuffers.
		size_t FrameIndex = 0u;

		// Whether the RHI supports being able to send commands from multiple threads.
		bool SupportsMultithreading = false;
	};

	extern RHIGlobals s_RHIGlobals;
	
	// The time it took the GPU to render the last frame.
	extern uint32_t s_GPUFrameTime;

	namespace RHI {

		inline size_t FrameIndex()
		{
			return s_RHIGlobals.FrameIndex;
		}

		inline bool IsInitialised()
		{
			return s_RHIGlobals.IsRHIInitialised;
		}

	}
}