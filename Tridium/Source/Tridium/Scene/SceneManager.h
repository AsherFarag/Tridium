#pragma once
#include <Tridium/Scene/Scene.h>

namespace Tridium {

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