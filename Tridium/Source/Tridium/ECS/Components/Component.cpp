#include "tripch.h"
#include "Component.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/ECS/GameObject.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( Component )
		PROPERTY( m_EntityID )
	END_REFLECT_COMPONENT( Component )

	Component::Component()
		: m_EntityID( NullEntity )
	{
	}

	OldGameObject Component::GetGameObject() const
	{
		return OldGameObject( m_EntityID );
	}

}