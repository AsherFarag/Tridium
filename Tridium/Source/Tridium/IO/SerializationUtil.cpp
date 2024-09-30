#include "tripch.h"
#include "SerializationUtil.h"

namespace Tridium {

	YAML::Emitter& operator<<( YAML::Emitter& out, const GUID& v )
	{
		out << v.ID();
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector2& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector3& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Vector4& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Color& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<( YAML::Emitter& out, const Matrix4& v )
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;

		for ( int i = 0; i < 4; ++i ) 
		{
			for ( int j = 0; j < 4; ++j ) 
			{
				out << v[i][j];
			}
		}

		out << YAML::EndSeq;
		return out;
	}
}