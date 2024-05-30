#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace Tridium {

	struct ProjectConfiguration
	{
		std::string Name = "Untitled";
		fs::path AssetDirectory = "Content"; // TEMP
		fs::path StartScene;
	};

	class Project
	{
	public:
		static Ref<Project> GetActiveProject()		    { TE_CORE_ASSERT( s_ActiveProject ); return s_ActiveProject; }
		static const fs::path& GetAssetDirectory()	    { TE_CORE_ASSERT( s_ActiveProject ); return s_ActiveProject->m_ProjectConfig.AssetDirectory; }
		static const fs::path& GetStartSceneDirectory() { TE_CORE_ASSERT( s_ActiveProject ); return s_ActiveProject->m_ProjectConfig.StartScene; }


	private:
		ProjectConfiguration m_ProjectConfig;

	private:
		static Ref<Project> s_ActiveProject;
	};

}