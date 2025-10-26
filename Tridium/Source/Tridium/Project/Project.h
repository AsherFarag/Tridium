#pragma once
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/Scene/SceneManager.h>

namespace Tridium {

	//=============================================================================================
	// Project Config:
	//=============================================================================================
	struct ProjectConfig
	{
		struct GeneralData
		{
			String Name = "Untitled";
			UUID StartScene;
			FilePath WorkingDirectory;
		} General;

		struct AssetConfig
		{
			FilePath AssetDirectory = "Content";
		} Assets;
	};

	//=============================================================================================
	// Project:
	//=============================================================================================
	class Project
	{
	public:

		//=========================================================================================
		NON_COPYABLE( Project );
		Project() = default;
		~Project() = default;

		//=========================================================================================
		ProjectConfig& Config() { return m_Config; }
		const ProjectConfig& Config() const { return m_Config; }

		//=========================================================================================
		AssetDatabase& GetAssetDatabase() { return m_AssetDatabase; }
		const AssetDatabase& GetAssetDatabase() const { return m_AssetDatabase; }

		//=========================================================================================
		SceneManager& GetSceneManager() { return m_SceneManager; }
		const SceneManager& GetSceneManager() const { return m_SceneManager; }

	private:

		//=========================================================================================
		ProjectConfig m_Config;
		AssetDatabase m_AssetDatabase;
		SceneManager m_SceneManager;

		//=========================================================================================
		friend class Engine;
		Expected<void, String> Init( ProjectConfig&& a_Config );
		Expected<void, String> Shutdown();

	};

} // namespace Tridium