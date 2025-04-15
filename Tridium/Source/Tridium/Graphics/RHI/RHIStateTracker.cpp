#include "tripch.h"
#include "RHIStateTracker.h"

namespace Tridium {

	void RHIResourceStateTracker::RequireTextureState( RHITexture& a_Texture, ERHIResourceStates a_NewState )
	{
		const ERHIResourceStates currentState = a_Texture.GetState();
		const bool isTransitionNeeded = currentState != a_NewState;
		if ( isTransitionNeeded )
		{
			ResourceBarriers.EmplaceBack() = RHIResourceBarrier( &a_Texture, currentState, a_NewState );
			a_Texture.SetState( a_NewState );
		}
	}

	void RHIResourceStateTracker::RequireBufferState( RHIBuffer& a_Buffer, ERHIResourceStates a_NewState )
	{
		if ( a_Buffer.Descriptor().CpuAccess != ERHICpuAccess::None )
		{
			// CPU access buffers can not change state.
			return;
		}

		const ERHIResourceStates currentState = a_Buffer.GetState();
		const bool isTransitionNeeded = currentState != a_NewState;
		if ( isTransitionNeeded )
		{
			// Check if this buffer is already being transitioned
			// If so, just combine the states
			for ( auto& barrier : ResourceBarriers )
			{
				if ( barrier.Resource == &a_Buffer )
				{
					barrier.After |= a_NewState;
					a_Buffer.SetState( barrier.After );
					return;
				}
			}

			ResourceBarriers.EmplaceBack() = RHIResourceBarrier( &a_Buffer, currentState, a_NewState );
			a_Buffer.SetState( a_NewState );
		}
	}

} // namespace Tridium