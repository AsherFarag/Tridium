#include "tripch.h"
#include "RHIResource.h"
#include "DynamicRHI.h"

namespace Tridium {

	IRHIObject::~IRHIObject()
	{
		Device()->UnregisterRHIObject( *this );
		LOG( LogCategory::Debug, Debug, "RHI Object '{}' destroyed" );
	}
}
