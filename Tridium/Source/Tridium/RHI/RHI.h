#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIMesh.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		bool Initialise( const RHIConfig& a_Config );
		bool Shutdown();
		bool Present();
		//===========================
	}

}
