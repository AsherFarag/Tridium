#include "tripch.h"
#include "SceneManager.h"

namespace Tridium {

    Scene* SceneManager::LoadScene( const IO::FilePath& a_FilePath )
    {
        return nullptr;
    }

    Scene* SceneManager::LoadScene( SceneHandle a_SceneAssetHandle )
    {
        return nullptr;
    }

	bool SceneManager::UnloadScene( UniquePtr<Scene> a_Scene )
	{
		if ( !CORE_ASSERT( a_Scene ) )
			return false;

		return Internal_UnloadScene( std::move( a_Scene ) );
	}

	bool SceneManager::UnloadActiveScene()
	{
		if ( !CORE_ASSERT( m_ActiveScene ) )
			return false;
		return Internal_UnloadScene( std::move( m_ActiveScene ) );
	}

	bool SceneManager::Internal_UnloadScene( UniquePtr<Scene> a_Scene )
	{
		a_Scene->OnEndPlay();
		return true;
	}

}
