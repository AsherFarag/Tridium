#include "tripch.h"
#include "RHIResource.h"

namespace Tridium {

	RHIResource::~RHIResource()
	{
		Release();
	}

}