#include "tripch.h"
#include "SceneManager.h"

namespace Tridium {

    OldScene* OldSceneManager::LoadScene( const FilePath& a_FilePath )
    {
        return nullptr;
    }

    OldScene* OldSceneManager::LoadScene( SceneHandle a_SceneAssetHandle )
    {
        return nullptr;
    }

    SceneManager* SceneManager::Get()
    {
        static SceneManager instance;
        return &instance;
	}

} // namespace Tridium
