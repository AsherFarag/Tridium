#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorEvents.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium::Editor::Events {

	MulticastDelegate<void, GameObject> OnGameObjectSelected;

}

#endif // IS_EDITOR
