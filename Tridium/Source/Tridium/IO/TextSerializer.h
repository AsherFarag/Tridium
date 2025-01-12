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

	// ----------- Math Types -----------

	template<> void SerializeToText( Archive& a_Archive, const Vector2& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Vector3& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Vector4& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const iVector2& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const iVector3& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const iVector4& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Color& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Quaternion& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Matrix3& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Matrix4& a_Data );
	template<> void SerializeToText( Archive& a_Archive, const Rotator& a_Data );

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

	 template<> void SerializeToText( Archive& a_Archive, const GUID& a_Data );
	 template<> void SerializeToText( Archive& a_Archive, const GameObject& );
	 template<> void SerializeToText( Archive& a_Archive, const SceneHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const MaterialHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const MeshSourceHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const StaticMeshHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const ShaderHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const TextureHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const CubeMapHandle& );
	 template<> void SerializeToText( Archive& a_Archive, const LuaScriptHandle& );


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

	// ----------- Math Types -----------

	template<> bool DeserializeFromText( const YAML::Node& a_Node, Vector2& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Vector3& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Vector4& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, iVector2& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, iVector3& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, iVector4& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Color& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Quaternion& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Matrix3& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Matrix4& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, Rotator& o_Data );

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

	template<> bool DeserializeFromText( const YAML::Node& a_Node, GUID& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, GameObject& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, SceneHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, MaterialHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, MeshSourceHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, StaticMeshHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, ShaderHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, TextureHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, CubeMapHandle& o_Data );
	template<> bool DeserializeFromText( const YAML::Node& a_Node, LuaScriptHandle& o_Data );

} // namespace Tridium::IO