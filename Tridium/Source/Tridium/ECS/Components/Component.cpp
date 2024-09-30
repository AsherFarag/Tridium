#include "tripch.h"
#include "Component.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( Component )
		PROPERTY( m_GameObject )
	END_REFLECT( Component )

	BEGIN_REFLECT_COMPONENT( ScriptableComponent )
		BASE( Component )
	END_REFLECT( ScriptableComponent )

}