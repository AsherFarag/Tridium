#include "tripch.h"
#include "GUIDComponent.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( GUIDComponent )
		BASE(Component)
		PROPERTY( m_ID, FLAGS(Serialize) )
	END_REFLECT( GUIDComponent )

}
