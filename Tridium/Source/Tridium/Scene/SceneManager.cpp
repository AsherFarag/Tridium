#include "tripch.h"
#include "SceneManager.h"

namespace Tridium {

    SceneManager* SceneManager::Get()
    {
        static SceneManager instance;
        return &instance;
	}

} // namespace Tridium
