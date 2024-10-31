#include "tripch.h"
#include "TextSerializer.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium::IO {

	// =================================================================================================
	// SerializeToText
	// =================================================================================================

#define _TRIDUM_SERIALIZE_TO_TEXT( Type ) template<> void SerializeToText( Archive& a_Archive, const Type& a_Value )

	// ----------- Math Types -----------

	_TRIDUM_SERIALIZE_TO_TEXT( Vector2 )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Vector3 )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Vector4 )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << a_Value.w;
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Color )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.r;
		a_Archive << a_Value.g;
		a_Archive << a_Value.b;
		a_Archive << a_Value.a;
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Quaternion )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << a_Value.w;
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Matrix3 )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		for ( int i = 0; i < 3; ++i )
		{
			for ( int j = 0; j < 3; ++j )
			{
				a_Archive << a_Value[i][j];
			}
		}
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Matrix4 )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		for ( int i = 0; i < 4; ++i )
		{
			for ( int j = 0; j < 4; ++j )
			{
				a_Archive << a_Value[i][j];
			}
		}
		a_Archive << YAML::EndSeq;
	}

	_TRIDUM_SERIALIZE_TO_TEXT( Rotator )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.Euler.x;
		a_Archive << a_Value.Euler.y;
		a_Archive << a_Value.Euler.z;
		a_Archive << YAML::EndSeq;
	}

	// ----------- Std Types -----------

	// ----------- Tridium Types -----------

	_TRIDUM_SERIALIZE_TO_TEXT( GUID )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( SceneHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( MaterialHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( MeshSourceHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( StaticMeshHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( ShaderHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( TextureHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( CubeMapHandle )
	{
		a_Archive << a_Value.ID();
	}

	_TRIDUM_SERIALIZE_TO_TEXT( LuaHandle )
	{
		a_Archive << a_Value.ID();
	}



	// =================================================================================================
	// DeserializeFromText
	// =================================================================================================

#define _TRIDUM_DESERIALIZE_FROM_TEXT( Type ) template<> bool DeserializeFromText( const YAML::Node& a_Node, Type& o_Value )

	// ----------- Math Types -----------

	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector2 )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 2 )
		{
			o_Value.x = a_Node[0].as<float>();
			o_Value.y = a_Node[1].as<float>();
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector3 )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 3 )
		{
			o_Value.x = a_Node[0].as<float>();
			o_Value.y = a_Node[1].as<float>();
			o_Value.z = a_Node[2].as<float>();
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector4 )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 4 )
		{
			o_Value.x = a_Node[0].as<float>();
			o_Value.y = a_Node[1].as<float>();
			o_Value.z = a_Node[2].as<float>();
			o_Value.w = a_Node[3].as<float>();
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Color )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 4 )
		{
			o_Value.r = a_Node[0].as<float>();
			o_Value.g = a_Node[1].as<float>();
			o_Value.b = a_Node[2].as<float>();
			o_Value.a = a_Node[3].as<float>();
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Quaternion )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 4 )
		{
			o_Value.x = a_Node[0].as<float>();
			o_Value.y = a_Node[1].as<float>();
			o_Value.z = a_Node[2].as<float>();
			o_Value.w = a_Node[3].as<float>();
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Matrix3 )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 9 )
		{
			for ( int i = 0; i < 3; ++i )
			{
				for ( int j = 0; j < 3; ++j )
				{
					o_Value[i][j] = a_Node[i * 3 + j].as<float>();
				}
			}
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Matrix4 )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 16 )
		{
			for ( int i = 0; i < 4; ++i )
			{
				for ( int j = 0; j < 4; ++j )
				{
					o_Value[i][j] = a_Node[i * 4 + j].as<float>();
				}
			}
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( Rotator )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 3 )
		{
			o_Value.SetFromEuler( Vector3( a_Node[0].as<float>(), a_Node[1].as<float>(), a_Node[2].as<float>() ) );
			return true;
		}
		return false;
	}

	// ----------- Std Types -----------

	// ----------- Tridium Types -----------

	_TRIDUM_DESERIALIZE_FROM_TEXT( GUID )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = GUID( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( SceneHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = SceneHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( MaterialHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = MaterialHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( MeshSourceHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = MeshSourceHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( StaticMeshHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = StaticMeshHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( ShaderHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = ShaderHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( TextureHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = TextureHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( CubeMapHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = CubeMapHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	_TRIDUM_DESERIALIZE_FROM_TEXT( LuaHandle )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = LuaHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

}