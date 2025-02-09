#pragma once
#include <Tridium/Utils/Singleton.h>
#include "Scene.h"

namespace Tridium {

	class SceneManager final : public ISingleton<SceneManager>
	{
	public:
		// Returns a pointer to the active scene
		Scene* GetActiveScene() { return m_ActiveScene.get(); }
		// Will attempt to load the scene at the specified file path and set it as the active scene.
		Scene* LoadScene( const FilePath& a_FilePath );
		// Will attempt to load the scene with the specified asset handle and set it as the active scene.
		Scene* LoadScene( SceneHandle a_SceneAssetHandle );
		// Will attempt to unload the specified scene.
		bool UnloadScene( UniquePtr<Scene> a_Scene );
		// Will attempt to unload the active scene.
		bool UnloadActiveScene();

	private:
		bool Internal_UnloadScene( UniquePtr<Scene> a_Scene );

	private:
		UniquePtr<Scene> m_ActiveScene;
	};

}