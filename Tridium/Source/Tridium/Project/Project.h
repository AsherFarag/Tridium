#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Project/ProjectConfig.h>

namespace Tridium {

	class Project
	{
		friend class ProjectSerializer;

	public:
		Project() = default;
		Project( const String& a_Name, const FilePath& a_AssetDirectory, SceneHandle a_StartScene );
		~Project();
		
		ProjectConfig& GetConfiguration() { return m_ProjectConfig; }
		const ProjectConfig& GetConfiguration() const { return m_ProjectConfig; }
		FilePath GetAssetDirectory() const { return m_ProjectConfig.Editor.ProjectDirectory / m_ProjectConfig.Editor.AssetDirectory; }

	protected:
		ProjectConfig m_ProjectConfig;
	};

} // namespace Tridium