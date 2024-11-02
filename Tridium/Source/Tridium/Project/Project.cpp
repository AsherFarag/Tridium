#include "tripch.h"
#include "Project.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

    Project::Project( const std::string& a_Name, const IO::FilePath& a_AssetDirectory, SceneHandle a_StartScene )
        : m_ProjectConfig( a_Name, a_AssetDirectory, a_StartScene ) 
    {}

    Project::~Project()
    {
    }
}