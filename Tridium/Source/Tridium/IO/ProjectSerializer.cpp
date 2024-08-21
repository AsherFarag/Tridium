#include "tripch.h"
#include "ProjectSerializer.h"
#include <Tridium/Project/Project.h>
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace YAML {

	template<>
	struct convert<Tridium::ProjectConfiguration>
	{
		static Node encode( const Tridium::ProjectConfiguration& rhs )
		{
			Node node;
			node.push_back( rhs.Name );
			node.push_back( rhs.AssetDirectory.ToString() );
			node.push_back( rhs.MetaDirectory.ToString() );
			node.push_back( rhs.StartScene.ToString() );
			return node;
		}

		static bool decode( const Node& node, Tridium::ProjectConfiguration& rhs )
		{
			if ( auto val = node["Name"] )
				rhs.Name = val.as<std::string>();
			else
				return false;

			if ( auto val = node["Asset Directory"] )
				rhs.AssetDirectory = val.as<std::string>();
			else
				return false;

			if ( auto val = node["Meta Directory"] )
				rhs.MetaDirectory = val.as<std::string>();
			else
				return false;

			if ( auto val = node["Start Scene"] )
				rhs.MetaDirectory = val.as<std::string>();
			else
				return false;

			return true;
		}
	};

}

namespace Tridium {


	YAML::Emitter& operator<<( YAML::Emitter& out, const ProjectConfiguration& v )
	{
		out << YAML::BeginMap;
		
		out << YAML::Key << "Name" << YAML::Value << v.Name;
		out << YAML::Key << "Asset Directory" << YAML::Value << v.AssetDirectory.ToString();
		out << YAML::Key << "Meta Directory" << YAML::Value << v.MetaDirectory.ToString();
		out << YAML::Key << "Start Scene" << YAML::Value << v.StartScene.ToString();

		out << YAML::EndMap;
		return out;
	}

	ProjectSerializer::ProjectSerializer( const SharedPtr<Project>& project )
		: m_Project( project )
	{
	}

	void ProjectSerializer::SerializeText( const std::string& filepath )
	{
		YAML::Emitter out;

		out << YAML::Key << "Configuration"; out << YAML::Value << m_Project->GetConfiguration();

		std::ofstream outFile( filepath );
		outFile << out.c_str();
	}

	bool ProjectSerializer::DeserializeText( const std::string& filepath )
	{
		YAML::Node data = YAML::LoadFile( filepath );

		auto configNode = data["Configuration"];

		if ( !configNode )
			return false;

		m_Project->m_ProjectConfig = configNode.as<ProjectConfiguration>();

		return true;
	}

}