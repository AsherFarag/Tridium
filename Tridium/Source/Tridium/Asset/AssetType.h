#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/GUID.h>
#include <Tridium/Utils/StringHelpers.h>

namespace Tridium {

	enum class EAssetType : uint8_t
	{
		None = 0,
		Scene,
		Material,
		MeshSource,
		StaticMesh,
		Shader,
		Texture,
		CubeMap,
		LuaScript,
	};

    using AssetHandle = GUID;

	namespace Internal {

		template <EAssetType _AssetType>
		class TypedAssetHandle : public AssetHandle
		{
		public:
			static constexpr EAssetType AssetType = _AssetType;

			TypedAssetHandle() = default;
			TypedAssetHandle( const AssetHandle& a_Handle ) : AssetHandle( a_Handle ) {}
			TypedAssetHandle( AssetHandle::Type a_Handle ) : AssetHandle( a_Handle ) {}

			TypedAssetHandle& operator=( const AssetHandle& a_Handle )
			{
				AssetHandle::operator=( a_Handle );
				return *this;
			}

			bool operator==( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator==( a_Handle );
			}

			bool operator!=( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator!=( a_Handle );
			}

			bool operator<( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator<( a_Handle );
			}

			bool operator>( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator>( a_Handle );
			}

			bool operator<=( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator<=( a_Handle );
			}

			bool operator>=( const AssetHandle& a_Handle ) const
			{
				return AssetHandle::operator>=( a_Handle );
			}

			operator GUID() const
			{
				return AssetHandle::operator GUID();
			}

			operator const GUID() const
			{
				return AssetHandle::operator const GUID();
			}
		};
	}

	using SceneHandle = Internal::TypedAssetHandle<EAssetType::Scene>;
	using MaterialHandle = Internal::TypedAssetHandle<EAssetType::Material>;
	using MeshSourceHandle = Internal::TypedAssetHandle<EAssetType::MeshSource>;
	using StaticMeshHandle = Internal::TypedAssetHandle<EAssetType::StaticMesh>;
	using ShaderHandle = Internal::TypedAssetHandle<EAssetType::Shader>;
	using TextureHandle = Internal::TypedAssetHandle<EAssetType::Texture>;
	using CubeMapHandle = Internal::TypedAssetHandle<EAssetType::CubeMap>;
	using LuaScriptHandle = Internal::TypedAssetHandle<EAssetType::LuaScript>;

	static const char* AssetTypeToString( EAssetType a_Type )
	{
		switch ( a_Type )
		{
		case EAssetType::Scene: return "Scene";
		case EAssetType::Material: return "Material";
		case EAssetType::MeshSource: return "MeshSource";
		case EAssetType::StaticMesh: return "Mesh";
		case EAssetType::Shader: return "Shader";
		case EAssetType::Texture: return "Texture";
		case EAssetType::CubeMap: return "CubeMap";
		case EAssetType::LuaScript: return "LuaScript";
		}

		return "None";
	}

	static EAssetType AssetTypeFromString( const char* a_Type )
	{
		if ( strcmp( a_Type, "Scene" ) == 0 ) return EAssetType::Scene;
		if ( strcmp( a_Type, "Material" ) == 0 ) return EAssetType::Material;
		if ( strcmp( a_Type, "MeshSource" ) == 0 ) return EAssetType::MeshSource;
		if ( strcmp( a_Type, "Mesh" ) == 0 ) return EAssetType::StaticMesh;
		if ( strcmp( a_Type, "Shader" ) == 0 ) return EAssetType::Shader;
		if ( strcmp( a_Type, "Texture" ) == 0 ) return EAssetType::Texture;
		if ( strcmp( a_Type, "CubeMap" ) == 0 ) return EAssetType::CubeMap;
		if ( strcmp( a_Type, "LuaScript" ) == 0 ) return EAssetType::LuaScript;

		return EAssetType::None;
	}

	EAssetType GetAssetTypeFromFileExtension( const std::string& a_Extension );
}

namespace std {

	template <Tridium::EAssetType _AssetType>
	struct hash<Tridium::Internal::TypedAssetHandle<_AssetType>>
	{
		size_t operator()( const Tridium::Internal::TypedAssetHandle<_AssetType>& a_Handle ) const
		{
			return std::hash<Tridium::AssetHandle>()( a_Handle );
		}
	};
}