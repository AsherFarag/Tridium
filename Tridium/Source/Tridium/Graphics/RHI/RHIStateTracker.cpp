#include "tripch.h"
#include "RHIStateTracker.h"
#include "RHITexture.h"
#include "RHIBuffer.h"

namespace Tridium {

	void RHIResourceStateTracker::AddResourceBarriers( Span<const RHIResourceBarrier> a_Barriers )
	{
		ResourceBarriers.Reserve( ResourceBarriers.Size() + a_Barriers.size() );
		for ( const auto& barrier : a_Barriers )
		{
			RHI_DEV_CHECK( barrier.Resource, "Resource must not be null" );
			ResourceBarriers.EmplaceBack() = barrier;
			barrier.Resource->SetState( barrier.After );
		}
	}

	void RHIResourceStateTracker::RequireTextureState( IRHITexture& a_Texture, ERHIResourceStates a_NewState )
	{
		const ERHIResourceStates currentState = a_Texture.State();
		const bool isTransitionNeeded = currentState != a_NewState;
		if ( isTransitionNeeded )
		{
			ResourceBarriers.EmplaceBack() = RHIResourceBarrier( &a_Texture, currentState, a_NewState );
			a_Texture.SetState( a_NewState );
		}
	}

	void RHIResourceStateTracker::RequireBufferState( IRHIBuffer& a_Buffer, ERHIResourceStates a_NewState )
	{
		if ( a_Buffer.Desc().CpuAccess != ERHICpuAccess::None )
		{
			// CPU access buffers can not change state.
			return;
		}

		const ERHIResourceStates currentState = a_Buffer.State();
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

	void RHIResourceStateTracker::SetResourceStatesForFramebuffer( const RHIFramebuffer& a_Framebuffer )
	{
		ResourceBarriers.Reserve( ResourceBarriers.Size() + a_Framebuffer.ColorAttachments.Size() + 1 );

		// Transition framebuffer attachments to render target state
		for ( const auto& attachment : a_Framebuffer.ColorAttachments )
		{
			if ( attachment ) RequireTextureState( *attachment.Texture, ERHIResourceStates::RenderTarget );
		}

		if ( a_Framebuffer.DepthStencilAttachment )
		{
			RequireTextureState( *a_Framebuffer.DepthStencilAttachment.Texture,
				a_Framebuffer.DepthStencilAttachment.ReadOnly ? ERHIResourceStates::DepthStencilRead : ERHIResourceStates::DepthStencilWrite
			);
		}
	}

} // namespace Tridium