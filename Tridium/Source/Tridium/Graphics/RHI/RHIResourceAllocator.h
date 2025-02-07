#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHIResourceAllocator
	//  Resource allocator represents a collection of contiguous allocations of descriptors,
	//  one allocation for every descriptor.
	RHI_RESOURCE_BASE_TYPE( ResourceAllocator )
	{
		uint32_t Capacity = 0;
	}
	//=======================================================

} // namespace Tridium