#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorEvents.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium {

	MulticastDelegate<void, GameObject> Events::OnGameObjectSelected;

}

#endif // IS_EDITOR
