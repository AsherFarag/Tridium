#pragma once
#include <Tridium/Core/Core.h>

#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

namespace YAML {

	template<>
	struct convert<Tridium::GUID>
	{
		static Node encode( const Tridium::GUID& rhs )
		{
			Node node;
			node.push_back( rhs.ID() );
			return node;
		}

		static bool decode( const Node& node, Tridium::GUID& rhs )
		{
			rhs = Tridium::GUID( node.as<Tridium::GUID::Type>() );
			return true;
		}
	};

	template<>
	struct convert<Vector3>
	{
		static Node encode( const Vector3& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			node.push_back( rhs.z );
			return node;
		}

		static bool decode( const Node& node, Vector3& rhs )
		{
			if ( !node.IsSequence() || node.size() != 3 )
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vector4>
	{
		static Node encode( const Vector4& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			node.push_back( rhs.z );
			node.push_back( rhs.w );
			return node;
		}

		static bool decode( const Node& node, Vector4& rhs )
		{
			if ( !node.IsSequence() || node.size() != 4 )
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Tridium {
	
	YAML::Emitter& operator<<( YAML::Emitter& out, const GUID& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector3& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector4& v );

}