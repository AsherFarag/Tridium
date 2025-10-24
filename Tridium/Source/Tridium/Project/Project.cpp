#include "tripch.h"
#include "Project.h"

namespace Tridium {

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