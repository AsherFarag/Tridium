#include "tripch.h"
#include "Project.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

    OldProject::OldProject( const String& a_Name, const FilePath& a_AssetDirectory, SceneHandle a_StartScene )
    {
		Config.Name = a_Name;
		Config.Editor.ProjectName = a_Name;
		Config.Editor.AssetDirectory = a_AssetDirectory;
		Config.StartScene = a_StartScene;
    }

    OldProject::~OldProject()
    {
    }

    Expected<void, String> Project::Init( ProjectConfig&& a_Config )
    {
        m_Config = std::move( a_Config );

		// Initialize Asset Database
		if ( auto result = m_AssetDatabase.Init(); result.IsError() )
        {
            return result;
        }

		return {};
	}

    Expected<void, String> Project::Shutdown()
    {
        // Shutdown Asset Database
        if ( auto result = m_AssetDatabase.Shutdown(); result.IsError() )
        {
            return result;
		}

        return {};
	}

} // namespace Tridium