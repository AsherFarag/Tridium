#include "tripch.h"
#include "Project.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

    Project::Project( const String& a_Name, const FilePath& a_AssetDirectory, SceneHandle a_StartScene )
    {
		Config.Name = a_Name;
		Config.Editor.ProjectName = a_Name;
		Config.Editor.AssetDirectory = a_AssetDirectory;
		Config.StartScene = a_StartScene;
    }

    Project::~Project()
    {
    }
}