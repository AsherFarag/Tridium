#include "tripch.h"
#include "ProjectSerializer.h"
#include <Tridium/Project/Project.h>
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

	void ProjectSerializer::SerializeText( const ProjectConfig& a_Project, const FilePath& a_Path )
	{
		YAML::Emitter out;

		out << YAML::BeginMap; // Project
		{
			out << YAML::Key << "Configuration";
			out << YAML::Value << YAML::BeginMap; // Configuration
			{
				out << YAML::Key << "Name" << YAML::Value << a_Project.Name;
				out << YAML::Key << "AssetDirectory" << YAML::Value << a_Project.Editor.AssetDirectory.ToString();
				out << YAML::Key << "StartScene" << YAML::Value << static_cast<SceneHandle::Type>( a_Project.StartScene.ID() );
			}
			out << YAML::EndMap; // Configuration
		}
		out << YAML::EndMap; // Project

		FilePath path = a_Path;
		path.ReplaceExtension( "tproject" );

		std::ofstream outFile( path.ToString() );
		outFile << out.c_str();
		outFile.close();
	}

	bool ProjectSerializer::DeserializeText( ProjectConfig& o_Project, const FilePath& a_Path )
	{
		YAML::Node data;

		try
		{
			data = YAML::LoadFile( a_Path );
		}
		catch ( const YAML::BadFile& e )
		{
			return false;
		}

		if ( auto configNode = data["Configuration"] )
		{
			o_Project.Name = configNode["Name"].as<std::string>();
			o_Project.Editor.AssetDirectory = FilePath( configNode["AssetDirectory"].as<std::string>() );
			o_Project.StartScene = configNode["StartScene"].as<SceneHandle::Type>();

			return true;
		}

		return false;
	}

} // namespace Tridium