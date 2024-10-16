#pragma once

namespace Tridium {

	struct ProjectConfiguration
	{
		std::string Name = "Untitled";
		fs::path AssetDirectory = "Content"; // TEMP
		fs::path MetaDirectory = "meta";
		fs::path StartScene = "Content";

		ProjectConfiguration() = default;
		ProjectConfiguration( const std::string& a_Name, const fs::path& a_AssetDirectory, const fs::path& a_StartScene )
			: Name( a_Name ), AssetDirectory( a_AssetDirectory ), StartScene( a_StartScene ) {}
	};

	class Project
	{
		friend class ProjectSerializer;

	public:
		Project() = default;
		Project( const std::string& a_Name, const fs::path& a_AssetDirectory, const fs::path& a_StartScene );
		~Project();

		ProjectConfiguration& GetConfiguration() { return m_ProjectConfig; }

		const std::string& GetName() const { return m_ProjectConfig.Name; }
		const fs::path& GetAssetDirectory() const { return m_ProjectConfig.AssetDirectory; }
		const fs::path& GetStartSceneDirectory() const { return m_ProjectConfig.StartScene; }


	private:
		ProjectConfiguration m_ProjectConfig;
	};
}