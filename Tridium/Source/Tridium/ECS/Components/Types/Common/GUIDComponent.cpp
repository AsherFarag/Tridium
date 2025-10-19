#include "tripch.h"
#include "GUIDComponent.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( OldGUIDComponent, Scriptable )
		BASE( Component )
		PROPERTY( m_ID, Serialize | VisibleAnywhere )
	END_REFLECT_COMPONENT( OldGUIDComponent )

	OldGUIDComponent::OldGUIDComponent( GUID a_ID )
		: m_ID( a_ID ) {}

}
