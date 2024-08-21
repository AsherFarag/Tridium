#pragma once

namespace Tridium {

	struct ProjectConfiguration
	{
		std::string Name = "Untitled";
		IO::FilePath AssetDirectory = "Content";
		IO::FilePath MetaDirectory = "Meta";
		IO::FilePath StartScene = "Content";

		ProjectConfiguration() = default;
		ProjectConfiguration( const std::string& a_Name, const IO::FilePath& a_AssetDirectory, const IO::FilePath& a_StartScene )
			: Name( a_Name ), AssetDirectory( a_AssetDirectory ), StartScene( a_StartScene ) {}
	};

	class Project
	{
		friend class ProjectSerializer;

	public:
		Project() = default;
		Project( const std::string& a_Name, const IO::FilePath& a_AssetDirectory, const IO::FilePath& a_StartScene );
		~Project();

		ProjectConfiguration& GetConfiguration() { return m_ProjectConfig; }

		const std::string& GetName() const { return m_ProjectConfig.Name; }
		const IO::FilePath& GetAssetDirectory() const { return m_ProjectConfig.AssetDirectory; }
		const IO::FilePath& GetMetaDirectory() const { return m_ProjectConfig.MetaDirectory; }
		const IO::FilePath& GetStartSceneDirectory() const { return m_ProjectConfig.StartScene; }


	private:
		ProjectConfiguration m_ProjectConfig;
	};
}