#pragma once

namespace Tridium {

	struct ProjectConfiguration
	{
		std::string Name = "Untitled";
		fs::path AssetDirectory = "Content"; // TEMP
		fs::path StartScene;

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

		const std::string& GetName() const { return m_ProjectConfig.Name; }
		const fs::path& GetAssetDirectory() const { return m_ProjectConfig.AssetDirectory; }
		const fs::path& GetStartSceneDirectory() const { return m_ProjectConfig.StartScene; }


	private:
		ProjectConfiguration m_ProjectConfig;
	};

	class ProjectSerializer
	{
	public:
		static void Serialize( const Project& a_Project, const std::string& a_FilePath );
		static Project* Deserialize( const std::string& a_FilePath );
	};

}