#include "tripch.h"
#include "CoreComponents.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( GameObjectFlagsComponent )
		BASE( Component )
		PROPERTY( Flags )
	END_REFLECT_COMPONENT( GameObjectFlagsComponent );

}