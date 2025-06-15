#pragma once
#include "RHICommon.h"
#include "RHIResource.h"

namespace Tridium {

	// Forward declarations
	class IRHITexture;
	class IRHIBuffer;
	struct RHIFramebuffer;

	//=========================================================
	// RHI Resource State Tracker
	//  Responsible for tracking and managing the state of RHI resources
	//  using resource barriers.
	//  Utility class for RHI implementations.
	//=========================================================
	struct RHIResourceStateTracker
	{
		Array<RHIResourceBarrier> ResourceBarriers;

		void RequireTextureState( IRHITexture& a_Texture, ERHIResourceStates a_NewState );
		void RequireBufferState( IRHIBuffer& a_Buffer, ERHIResourceStates a_NewState );
		void SetResourceStatesForFramebuffer( const RHIFramebuffer& a_Framebuffer );
		void Clear() { ResourceBarriers.Clear(); }
	};

}