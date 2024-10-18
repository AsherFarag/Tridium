#pragma once
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	struct ProjectConfiguration
	{
		std::string Name = "Untitled";
		IO::FilePath ProjectDirectory{};
		IO::FilePath AssetDirectory{ "Content" }; // Relative to ProjectDirectory
		IO::FilePath StartScene{};

		ProjectConfiguration() = default;
		ProjectConfiguration( const std::string& a_Name, const IO::FilePath& a_AssetDirectory, const IO::FilePath& a_StartScene )
			: Name( a_Name ), ProjectDirectory( a_AssetDirectory ), StartScene( a_StartScene ) {}
	};

	class Project
	{
		friend class ProjectSerializer;

	public:
		Project() = default;
		Project( const std::string& a_Name, const IO::FilePath& a_AssetDirectory, const IO::FilePath& a_StartScene );
		~Project();

		ProjectConfiguration& GetConfiguration() { return m_ProjectConfig; }
		const ProjectConfiguration& GetConfiguration() const { return m_ProjectConfig; }
		IO::FilePath GetAssetDirectory() const { return m_ProjectConfig.ProjectDirectory + m_ProjectConfig.AssetDirectory; }

	protected:
		ProjectConfiguration m_ProjectConfig;
	};
}