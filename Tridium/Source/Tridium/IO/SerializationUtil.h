#pragma once
#include <Tridium/Core/Core.h>
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

	template<size_t _Count, ::Tridium::Concepts::Arithmetic T>
	struct convert<Tridium::Vector<_Count, T>>
	{
		using Vector = Tridium::Vector<_Count, T>;
		static Node encode( const Vector& rhs )
		{
			Node node;
			for ( size_t i = 0; i < _Count; ++i )
				node.push_back( rhs[i] );
			return node;
		}

		static bool decode( const Node& node, Tridium::Vector<_Count, T>& rhs )
		{
			if ( !node.IsSequence() || node.size() != _Count )
				return false;

			for ( size_t i = 0; i < _Count; ++i )
			{
				if ( !node[i].IsScalar() )
					return false;

				rhs[i] = node[i].as<T>();
			}
			return true;
		}
	};

	template<>
	struct convert<Tridium::Color4>
	{
		static Node encode( const Tridium::Color4& rhs )
		{
			Node node;
			node.push_back( rhs.r );
			node.push_back( rhs.g );
			node.push_back( rhs.b );
			node.push_back( rhs.a );
			return node;
		}

		static bool decode( const Node& node, Tridium::Color4& rhs )
		{
			if ( !node.IsSequence() || node.size() != 4 )
				return false;

			rhs.r = node[0].as<float>();
			rhs.g = node[1].as<float>();
			rhs.b = node[2].as<float>();
			rhs.a = node[3].as<float>();
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

	template<size_t _Count, Concepts::Arithmetic _Gen>
	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector<_Count, _Gen>& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for ( size_t i = 0; i < _Count; ++i ) out << v[i];
		out << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Color4& v );
	
	YAML::Emitter& operator<<( YAML::Emitter& out, const Matrix4& v );

	namespace IO {

		template<typename T> struct Serializer;

	}
}