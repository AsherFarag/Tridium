#pragma once
#include <Tridium/Core/Containers/String.h>

namespace Tridium {

	//===========================
	// RHI Globals
	//  A struct to hold global RHI information.
	//===========================
	struct RHIGlobals
	{
		// True if the rendering hardware interface has been initialised.
		bool IsRHIInitialised = false;
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
}