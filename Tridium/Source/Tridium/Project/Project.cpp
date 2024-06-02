#include "tripch.h"
#include "Project.h"
#include <fstream>

namespace Tridium {

	void ProjectSerializer::Serialize( const Project& a_Project, const std::string& a_FilePath )
	{
        std::ofstream file( a_FilePath, std::ios::binary );
        if ( !file.is_open() )
        {
            TE_CORE_FATAL( "Failed to serialize project at [{0}]", a_FilePath );
            return;
        }

        file.write( reinterpret_cast<const char*>( &a_Project ), sizeof( a_Project ) );
        file.close();
        TE_CORE_INFO( "Project serialized successfully!" );
	}

	Project* ProjectSerializer::Deserialize( const std::string& a_FilePath )
	{
        Project* project = new Project();
        std::ifstream file( a_FilePath, std::ios::binary );
        if ( !file.is_open() ) 
        {
            TE_CORE_FATAL( "Failed to deserialize project at [{0}]", a_FilePath );
            return nullptr;
        }

        file.read( reinterpret_cast<char*>( &*project ), sizeof( *project ) );
        file.close();

        TE_CORE_INFO( "Project deserialized successfully!" );

        return project;
	}


    Project::Project( const std::string& a_Name, const fs::path& a_AssetDirectory, const fs::path& a_StartScene )
        : m_ProjectConfig( a_Name, a_AssetDirectory, a_StartScene ) 
    {}

    Project::~Project()
    {
        //ProjectSerializer::Serialize( *this,
        //    ( m_ProjectConfig.AssetDirectory / m_ProjectConfig.Name / ".TEproject" ).string() );
    }
}