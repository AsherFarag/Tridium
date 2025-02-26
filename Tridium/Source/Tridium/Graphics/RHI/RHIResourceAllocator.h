#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHIResourceAllocator
	//  Resource allocator represents a collection of contiguous allocations of descriptors,
	//  one allocation for every descriptor.
	DEFINE_RHI_RESOURCE( ResourceAllocator )
	{
		uint32_t Capacity = 0;
	}
	//=======================================================

} // namespace Tridium