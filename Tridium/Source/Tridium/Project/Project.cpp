#include "tripch.h"
#include "Project.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

    Project::Project( const String& a_Name, const FilePath& a_AssetDirectory, SceneHandle a_StartScene )
    {
		m_ProjectConfig.Name = a_Name;
		m_ProjectConfig.Editor.ProjectName = a_Name;
		m_ProjectConfig.Editor.AssetDirectory = a_AssetDirectory;
		m_ProjectConfig.StartScene = a_StartScene;
    }

    Project::~Project()
    {
    }
}