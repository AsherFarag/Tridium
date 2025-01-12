#include "tripch.h"
#include "TextSerializer.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium::IO {

	// =================================================================================================
	// SerializeToText
	// =================================================================================================

	// ----------- Math Types -----------

	template<> 
	void SerializeToText( Archive& a_Archive, const Vector2& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << YAML::EndSeq;
	}

	template<>
	void SerializeToText( Archive& a_Archive, const Vector3& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << YAML::EndSeq;
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const Vector4& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << a_Value.w;
		a_Archive << YAML::EndSeq;
	}

	template<>
	void SerializeToText( Archive& a_Archive, const iVector2& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << YAML::EndSeq;
	}

	template<>
	void SerializeToText( Archive& a_Archive, const iVector3& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << YAML::EndSeq;
	}

	template<>
	void SerializeToText( Archive& a_Archive, const iVector4& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << a_Value.w;
		a_Archive << YAML::EndSeq;
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const Color& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.r;
		a_Archive << a_Value.g;
		a_Archive << a_Value.b;
		a_Archive << a_Value.a;
		a_Archive << YAML::EndSeq;
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const Quaternion& a_Value )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		a_Archive << a_Value.x;
		a_Archive << a_Value.y;
		a_Archive << a_Value.z;
		a_Archive << a_Value.w;
		a_Archive << YAML::EndSeq;
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const Matrix3& a_Value )
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

	template<> 
	void SerializeToText( Archive& a_Archive, const Matrix4& a_Value )
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

	template<> 
	void SerializeToText( Archive& a_Archive, const Rotator& a_Value )
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

	template<> 
	void SerializeToText( Archive& a_Archive, const GUID& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const GameObject& a_Value )
	{
		a_Archive << static_cast<EntityIDType>( a_Value.ID() );
	}


	template<> 
	void SerializeToText( Archive& a_Archive, const SceneHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const MaterialHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<>
	void SerializeToText( Archive& a_Archive, const MeshSourceHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<>
	void SerializeToText( Archive& a_Archive, const StaticMeshHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const ShaderHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const TextureHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const CubeMapHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}

	template<> 
	void SerializeToText( Archive& a_Archive, const LuaScriptHandle& a_Value )
	{
		a_Archive << a_Value.ID();
	}



	// =================================================================================================
	// DeserializeFromText
	// =================================================================================================

	// ----------- Math Types -----------

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, Vector2& o_Value )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 2 )
		{
			o_Value.x = a_Node[0].as<float>();
			o_Value.y = a_Node[1].as<float>();
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, Vector3& o_Value )
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

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, Vector4& o_Value )
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

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, iVector2& o_Value )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 2 )
		{
			o_Value.x = a_Node[0].as<int>();
			o_Value.y = a_Node[1].as<int>();
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, iVector3& o_Value )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 3 )
		{
			o_Value.x = a_Node[0].as<int>();
			o_Value.y = a_Node[1].as<int>();
			o_Value.z = a_Node[2].as<int>();
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, iVector4& o_Value )
	{
		if ( a_Node && a_Node.IsSequence() && a_Node.size() == 4 )
		{
			o_Value.x = a_Node[0].as<int>();
			o_Value.y = a_Node[1].as<int>();
			o_Value.z = a_Node[2].as<int>();
			o_Value.w = a_Node[3].as<int>();
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, Color& o_Value )
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

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, Quaternion& o_Value )
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

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, Matrix3& o_Value )
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

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, Matrix4& o_Value )
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

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, Rotator& o_Value )
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

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, GUID& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = GUID( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, GameObject& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = GameObject( a_Node.as<EntityIDType>() );
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, SceneHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = SceneHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, MaterialHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = MaterialHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, MeshSourceHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = MeshSourceHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, StaticMeshHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = StaticMeshHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, ShaderHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = ShaderHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, TextureHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = TextureHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<>
	bool DeserializeFromText( const YAML::Node& a_Node, CubeMapHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = CubeMapHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

	template<> 
	bool DeserializeFromText( const YAML::Node& a_Node, LuaScriptHandle& o_Value )
	{
		if ( a_Node && a_Node.IsScalar() )
		{
			o_Value = LuaScriptHandle( a_Node.as<uint64_t>() );
			return true;
		}
		return false;
	}

}