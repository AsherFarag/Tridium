#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=============================================
	// RHI Command Allocator
	//  A resource that allocates command buffers.
	//=============================================
	DEFINE_RHI_RESOURCE( CommandAllocator )
	{
		size_t Capacity = 32u;
	};

}