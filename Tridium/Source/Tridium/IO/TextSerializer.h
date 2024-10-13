#pragma once
#include <Tridium/Core/Core.h>

#include "Archive.h"

namespace Tridium {

	class GameObject;

}

namespace Tridium::IO {

	enum class ETextSerializationFormat
	{
		YAML,
		JSON,
	};

	// =================================================================================================
	// SerializeToText
	// =================================================================================================

	// Serialize a value into a text format into the given archive.
	template<typename T>
	void SerializeToText( Archive& a_Archive, const T& a_Data )
	{
		a_Archive << YAML::Value << a_Data;
	}

#define _TRIDUM_SERIALIZE_TO_TEXT( Type ) template<> void SerializeToText( Archive& a_Archive, const Type& a_Data )

	// ----------- Math Types -----------

	_TRIDUM_SERIALIZE_TO_TEXT( Vector2 );
	_TRIDUM_SERIALIZE_TO_TEXT( Vector3 );
	_TRIDUM_SERIALIZE_TO_TEXT( Vector4 );
	_TRIDUM_SERIALIZE_TO_TEXT( Color );
	_TRIDUM_SERIALIZE_TO_TEXT( Quaternion );
	_TRIDUM_SERIALIZE_TO_TEXT( Matrix3 );
	_TRIDUM_SERIALIZE_TO_TEXT( Matrix4 );

	// ----------- STD Types -----------

	template<typename T>
	void SerializeToText( Archive& a_Archive, const std::vector<T>& a_Data )
	{
		a_Archive << YAML::Flow;
		a_Archive << YAML::BeginSeq;
		for ( const T& it : a_Data )
		{
			SerializeToText( a_Archive, it );
		}
		a_Archive << YAML::EndSeq;
	}

	template<typename _Key, typename _Val>
	void SerializeToText( Archive& a_Archive, const std::map<_Key, _Val>& a_Data )
	{
		a_Archive << YAML::BeginSeq;
		for ( const auto& it : a_Data )
		{
			a_Archive << YAML::Flow << YAML::BeginSeq;
			SerializeToText( a_Archive, it.first );
			SerializeToText( a_Archive, it.second );
			a_Archive << YAML::EndSeq;
		}
		a_Archive << YAML::EndSeq;
	}

	template<typename _Key, typename _Val>
	void SerializeToText( Archive& a_Archive, const std::unordered_map<_Key, _Val>& a_Data )
	{
		a_Archive << YAML::BeginSeq;
		for ( const auto& it : a_Data )
		{
			a_Archive << YAML::Flow << YAML::BeginSeq;
			SerializeToText( a_Archive, it.first );
			SerializeToText( a_Archive, it.second );
			a_Archive << YAML::EndSeq;
		}
		a_Archive << YAML::EndSeq;
	}

	// ---------- Tridium Types -----------

	_TRIDUM_SERIALIZE_TO_TEXT( GUID );


#undef _TRIDUM_SERIALIZE_TO_TEXT



	// =================================================================================================
	// DeserializeFromText
	// =================================================================================================

	// Deserialize a value from a text format from the given node.
	// Returns true if the deserialization was successful, false otherwise.
	template<typename T>
	bool DeserializeFromText( const YAML::Node& a_Node, T& o_Data )
	{
		if ( a_Node )
		{
			o_Data = a_Node.as<T>();
			return true;
		}
		return false;
	}

#define _TRIDUM_DESERIALIZE_FROM_TEXT( Type ) template<> bool DeserializeFromText( const YAML::Node& a_Node, Type& o_Data )

	// ----------- Math Types -----------

	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector2 );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector3 );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Vector4 );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Color );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Quaternion );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Matrix3 );
	_TRIDUM_DESERIALIZE_FROM_TEXT( Matrix4 );

	// ----------- STD Types -----------

	template<typename T>
	bool DeserializeFromText( const YAML::Node& a_Node, std::vector<T>& o_Data )
	{
		if ( a_Node && a_Node.IsSequence() )
		{
			const size_t size = a_Node.size();
			o_Data.reserve( size );

			size_t index = 0;
			for ( const auto& it : a_Node )
			{
				if ( !DeserializeFromText( it, o_Data[index] ) )
					return false;

				++index;
			}

			return true;
		}

		return false;
	}

	template<typename _Key, typename _Val>
	bool DeserializeFromText( const YAML::Node& a_Node, std::map<_Key, _Val>& o_Data )
	{
		if ( a_Node && a_Node.IsSequence() )
		{
			for ( const auto& it : a_Node )
			{
				if ( !it.IsSequence() || it.size() != 2 )
					return false;

				_Key key;
				_Val val;
				if ( !DeserializeFromText( it[0], key ) || !DeserializeFromText( it[1], val ) )
					return false;

				o_Data[key] = val;
			}

			return true;
		}

		return false;
	}

	template<typename _Key, typename _Val>
	bool DeserializeFromText( const YAML::Node& a_Node, std::unordered_map<_Key, _Val>& o_Data )
	{
		if ( a_Node && a_Node.IsSequence() )
		{
			for ( const auto& it : a_Node )
			{
				if ( !it.IsSequence() || it.size() != 2 )
					return false;

				_Key key;
				_Val val;
				if ( !DeserializeFromText( it[0], key ) || !DeserializeFromText( it[1], val ) )
					return false;

				o_Data[key] = val;
			}

			return true;
		}

		return false;
	}

	// ---------- Tridium Types -----------

	_TRIDUM_DESERIALIZE_FROM_TEXT( GUID );

#undef _TRIDUM_DESERIALIZE_FROM_TEXT

} // namespace Tridium::IO