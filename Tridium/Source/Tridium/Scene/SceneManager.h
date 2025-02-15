#pragma once
#include <Tridium/Utils/Singleton.h>
#include "Scene.h"

namespace Tridium {

	class SceneManager final : public ISingleton<SceneManager, /* _ExplicitSetup */ false>
	{
	public:
		// Gets the currently active Scene.
		static Scene* GetActiveScene() { return Get()->m_ActiveScene.get(); }
		// Gets a weak ptr to the currently active Scene.
		static WeakPtr<Scene> GetActiveSceneWeak() { return Get()->m_ActiveScene; }
		// Set the scene to be active.
		static void SetActiveScene( Scene* a_Scene ) { Get()->m_ActiveScene = SharedPtrCast<Scene>( a_Scene->shared_from_this() ); }
		// Will attempt to load the scene.
		static Scene* LoadScene( const FilePath& a_FilePath );
		// Will attempt to load the scene.
		static Scene* LoadScene( SceneHandle a_SceneAssetHandle );
		// Unload the specified scene.
		static void UnloadScene( Scene* a_Scene ) {}

	private:
		SharedPtr<Scene> m_ActiveScene;
	};

}