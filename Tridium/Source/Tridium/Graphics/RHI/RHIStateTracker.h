#pragma once
#include "RHICommon.h"
#include "RHITexture.h"
#include "RHIBuffer.h"

namespace Tridium {

	//=========================================================
	// RHI Resource State Tracker
	//  Responsible for tracking and managing the state of RHI resources
	//  using resource barriers.
	//  Utility class for RHI implementations.
	//=========================================================
	struct RHIResourceStateTracker
	{
		Array<RHIResourceBarrier> ResourceBarriers;

		void RequireTextureState( RHITexture& a_Texture, ERHIResourceStates a_NewState );
		void RequireBufferState( RHIBuffer& a_Buffer, ERHIResourceStates a_NewState );
	};

}