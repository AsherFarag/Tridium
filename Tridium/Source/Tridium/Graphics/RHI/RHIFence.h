#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Fence
	//  A fence is a synchronization primitive that can be used to synchronize the CPU and GPU.
	//=======================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIFenceDescriptor, RHIFence )
	{
		ERHIFenceType Type = ERHIFenceType::CPUWaitOnly;
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIFence )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIFence, ERHIResourceType::Fence );

		virtual bool Commit( const RHIFenceDescriptor& a_Desc ) = 0;
		virtual uint64_t GetCompletedValue() = 0;
		virtual void Signal( uint64_t a_Value ) = 0;
		virtual void Wait( uint64_t a_Value ) = 0;
	};

} // namespace Tridium