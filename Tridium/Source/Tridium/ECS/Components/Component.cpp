#include "tripch.h"
#include "Component.h"

namespace Tridium {

	ScriptableComponent::~ScriptableComponent()
	{
		//OnDestroy();
	}

	void Component::OnReflect()
	{
		BEGIN_REFLECT( Component )
			PROPERTY( &Component::m_GameObject, "GameObject" )
			FUNCTION( &Component::GetGameObject)
		END_REFLECT
	}

}