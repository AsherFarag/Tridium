#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=============================================
	// RHI Command Allocator
	//  A resource that allocates command buffers.
	//=============================================
	RHI_RESOURCE_BASE_TYPE( CommandAllocator )
	{
		size_t Capacity = 32u;
	};

}