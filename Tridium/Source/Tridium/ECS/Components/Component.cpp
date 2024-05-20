#include "tripch.h"
#include "Component.h"

namespace Tridium {

	ScriptableComponent::ScriptableComponent()
	{
	}


	ScriptableComponent::~ScriptableComponent()
	{
		OnDestroy();
	}

}