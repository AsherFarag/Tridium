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
        file.write( "Name:", 5 );
        file.write( a_Project.m_ProjectConfig.Name.c_str(), a_Project.m_ProjectConfig.Name.length() + 1 );
        file.write( "\n", 1);

        file.write( "Asset Directory:", 16 );
        file.write( a_Project.m_ProjectConfig.AssetDirectory.string().c_str(), a_Project.m_ProjectConfig.AssetDirectory.string().length() + 1);
        file.write( "\n", 1 );

        file.write( "Start Scene:", 12 );
        file.write( a_Project.m_ProjectConfig.StartScene.string().c_str(), a_Project.m_ProjectConfig.StartScene.string().length() + 1 );
        file.write( "\n", 1 );

        file.close();
        TE_CORE_INFO( "Project serialized successfully!" );
	}

	Project* ProjectSerializer::Deserialize( const std::string& a_FilePath )
	{
        Project* project = new Project();

        if ( Deserialize( *project, a_FilePath ) )
            return project;

        delete project;
        return nullptr;
	}

    bool ProjectSerializer::Deserialize( Project& a_Project, const std::string& a_FilePath )
    {
        std::ifstream file( a_FilePath, std::ios::binary );
        if ( !file.is_open() )
        {
            TE_CORE_FATAL( "Failed to deserialize project at [{0}]", a_FilePath );
            return false;
        }

        while ( !file.eof() )
        {
            std::string key, value;
            file.getline( (char*)key.c_str(), 256, ':' );
            file.getline( (char*)value.c_str(), 256, '\0' );
            if ( !_strcmpi( key.c_str(), "Name" ) )
                a_Project.m_ProjectConfig.Name = value.c_str();
            else if ( !_strcmpi( key.c_str(), "\nAsset Directory" ) )
                a_Project.m_ProjectConfig.AssetDirectory = value.c_str();
            else if ( !_strcmpi( key.c_str(), "\nStart Scene" ) )
                a_Project.m_ProjectConfig.StartScene = value.c_str();
        }

        file.close();

        TE_CORE_INFO( "Project deserialized successfully!" );

        return true;
    }


    Project::Project( const std::string& a_Name, const fs::path& a_AssetDirectory, const fs::path& a_StartScene )
        : m_ProjectConfig( a_Name, a_AssetDirectory, a_StartScene ) 
    {}

    Project::~Project()
    {
        ProjectSerializer::Serialize( *this,
            ( m_ProjectConfig.AssetDirectory / m_ProjectConfig.Name).replace_extension( ".TEproject" ).string());
    }
}