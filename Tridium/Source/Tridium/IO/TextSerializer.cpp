#include "tripch.h"
#include "TextSerializer.h"

namespace Tridium::IO {

	// =================================================================================================
	// SerializeToText
	// =================================================================================================

#define _TRIDUM_SERIALIZE_TO_TEXT( Type ) \
	template<> void SerializeToText( Archive& a_Archive, const Type& a_Value )

#pragma region Math Types

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

#pragma endregion

#pragma region Std Types

#pragma endregion


	// =================================================================================================
	// DeserializeFromText
	// =================================================================================================

#define _TRIDUM_DESERIALIZE_FROM_TEXT( Type ) \
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Type& o_Value )

#pragma region Math Types

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

#pragma endregion
}