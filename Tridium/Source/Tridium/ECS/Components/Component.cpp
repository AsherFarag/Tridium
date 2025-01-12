#include "tripch.h"
#include "Component.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( Component )
		PROPERTY( m_GameObject )
	END_REFLECT_COMPONENT( Component )

	BEGIN_REFLECT_COMPONENT( NativeScriptComponent )
	END_REFLECT_COMPONENT( NativeScriptComponent )

	Component::Component()
		: m_GameObject()
	{
	}

}