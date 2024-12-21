#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorEvents.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium::Editor {

	MulticastDelegate<void, GameObjectID> Events::OnGameObjectSelected;

}

#endif // IS_EDITOR
