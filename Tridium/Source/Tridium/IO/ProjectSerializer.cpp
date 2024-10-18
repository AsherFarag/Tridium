#include "tripch.h"
#include "ProjectSerializer.h"
#include <Tridium/Project/Project.h>
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {


	ProjectSerializer::ProjectSerializer( const SharedPtr<Project>& a_Project )
		: m_Project( a_Project )
	{
	}

	void ProjectSerializer::SerializeText( const std::string& a_Path )
	{
		YAML::Emitter out;

		out << YAML::BeginMap; // Project
		{
			out << YAML::Key << "Configuration";
			out << YAML::Value << YAML::BeginMap; // Configuration
			{
				out << YAML::Key << "Name" << YAML::Value << m_Project->m_ProjectConfig.Name;
				out << YAML::Key << "AssetDirectory" << YAML::Value << m_Project->m_ProjectConfig.AssetDirectory.ToString();
				out << YAML::Key << "StartScene" << YAML::Value << m_Project->m_ProjectConfig.StartScene.ToString();
			}
			out << YAML::EndMap; // Configuration
		}
		out << YAML::EndMap; // Project

		std::ofstream outFile( a_Path );
		outFile << out.c_str();
	}

	bool ProjectSerializer::DeserializeText( const std::string& a_Path )
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
			m_Project->m_ProjectConfig.Name = configNode["Name"].as<std::string>();
			m_Project->m_ProjectConfig.AssetDirectory = IO::FilePath( configNode["AssetDirectory"].as<std::string>() );
			m_Project->m_ProjectConfig.StartScene = IO::FilePath( configNode["StartScene"].as<std::string>() );
		}
		else
		{
			return false;
		}

		return true;
	}

}