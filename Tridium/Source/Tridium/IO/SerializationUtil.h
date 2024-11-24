#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/ForEachStructMember.h>

#include <fstream>

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
	struct convert<Tridium::Vector2>
	{
		static Node encode( const Tridium::Vector2& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			return node;
		}

		static bool decode( const Node& node, Tridium::Vector2& rhs )
		{
			if ( !node.IsSequence() || node.size() != 2 )
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Tridium::Vector3>
	{
		static Node encode( const Tridium::Vector3& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			node.push_back( rhs.z );
			return node;
		}

		static bool decode( const Node& node, Tridium::Vector3& rhs )
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
	struct convert<Tridium::Vector4>
	{
		static Node encode( const Tridium::Vector4& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			node.push_back( rhs.z );
			node.push_back( rhs.w );
			return node;
		}

		static bool decode( const Node& node, Tridium::Vector4& rhs )
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

	template<>
	struct convert<Tridium::Color>
	{
		static Node encode( const Tridium::Color& rhs )
		{
			Node node;
			node.push_back( rhs.x );
			node.push_back( rhs.y );
			node.push_back( rhs.z );
			node.push_back( rhs.w );
			return node;
		}

		static bool decode( const Node& node, Tridium::Color& rhs )
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

	template<>
	struct convert<Tridium::Matrix4>
	{
		static Node encode( const Tridium::Matrix4& rhs )
		{
			Node node;
			for ( int i = 0; i < 4; ++i )
			{
				for ( int j = 0; j < 4; ++j )
				{
					node.push_back(rhs[i][j]);
				}
			}
			return node;
		}

		static bool decode( const Node& node, Tridium::Matrix4& rhs )
		{
			if ( !node.IsSequence() || node.size() != 16 )
				return false;

			for ( int i = 0; i < 4; ++i )
			{
				for ( int j = 0; j < 4; ++j )
				{
					rhs[i][j] = node[i * 4 + j].as<float>();
				}
			}

			return true;
		}
	};
}

namespace Tridium {
	
	YAML::Emitter& operator<<( YAML::Emitter& out, const GUID& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector2& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector3& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector4& v );

	YAML::Emitter& operator<<( YAML::Emitter& out, const Color& v );
	
	YAML::Emitter& operator<<( YAML::Emitter& out, const Matrix4& v );
}