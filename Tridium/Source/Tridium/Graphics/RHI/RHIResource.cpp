#include "tripch.h"
#include "RHIResource.h"
#include "DynamicRHI.h"

namespace Tridium {

	IRHIObject::~IRHIObject()
	{
		TODO( "This" );
		if ( RHI::GetDynamicRHI() )
		{
			Device()->UnregisterRHIObject( *this );
		}
	}
}
