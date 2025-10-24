#pragma once
#include <Tridium/Scene/Scene.h>

// TEMP
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	class OldSceneManager final : public ISingleton<OldSceneManager, /* _ExplicitSetup */ false>
	{
	public:
		// Gets the currently active Scene.
		static OldScene* GetActiveScene() { return Get()->m_ActiveScene.get(); }
		// Gets a weak ptr to the currently active Scene.
		static WeakPtr<OldScene> GetActiveSceneWeak() { return Get()->m_ActiveScene; }
		// Set the scene to be active.
		static void SetActiveScene( OldScene* a_Scene ) { Get()->m_ActiveScene = SharedPtrCast<OldScene>( a_Scene->shared_from_this() ); }
		// Will attempt to load the scene.
		static OldScene* LoadScene( const FilePath& a_FilePath );
		// Will attempt to load the scene.
		static OldScene* LoadScene( SceneHandle a_SceneAssetHandle );
		// Unload the specified scene.
		static void UnloadScene( OldScene* a_Scene ) {}

	private:
		SharedPtr<OldScene> m_ActiveScene;
	};

	//=================================================================================================
	// SceneManager: Handles the loading, unloading, and management of scenes.
	//=================================================================================================
	class SceneManager
	{
	public:

		//=============================================================================================
		static SceneManager* Get();

		//=============================================================================================
		static const SharedPtr<Scene>& ActiveScene() { return Get()->m_ActiveScene; }
		static void SetActiveScene( const SharedPtr<Scene>& a_Scene ) { Get()->m_ActiveScene = a_Scene; }

	private:

		//=============================================================================================
		SharedPtr<Scene> m_ActiveScene;

	};

}