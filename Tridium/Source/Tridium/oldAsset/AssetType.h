#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/GUID.h>

namespace Tridium {

	enum class EAssetTypeOld : uint8_t
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

		template <EAssetTypeOld _AssetType>
		class TypedAssetHandle : public AssetHandle
		{
		public:
			static constexpr EAssetTypeOld AssetType = _AssetType;

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

	using SceneHandle = Internal::TypedAssetHandle<EAssetTypeOld::Scene>;
	using MaterialHandle = Internal::TypedAssetHandle<EAssetTypeOld::Material>;
	using MeshSourceHandle = Internal::TypedAssetHandle<EAssetTypeOld::MeshSource>;
	using StaticMeshHandle = Internal::TypedAssetHandle<EAssetTypeOld::StaticMesh>;
	using ShaderHandle = Internal::TypedAssetHandle<EAssetTypeOld::Shader>;
	using TextureHandle = Internal::TypedAssetHandle<EAssetTypeOld::Texture>;
	using CubeMapHandle = Internal::TypedAssetHandle<EAssetTypeOld::CubeMap>;
	using LuaScriptHandle = Internal::TypedAssetHandle<EAssetTypeOld::LuaScript>;

	static const char* AssetTypeToString( EAssetTypeOld a_Type )
	{
		switch ( a_Type )
		{
		case EAssetTypeOld::Scene: return "Scene";
		case EAssetTypeOld::Material: return "Material";
		case EAssetTypeOld::MeshSource: return "MeshSource";
		case EAssetTypeOld::StaticMesh: return "Mesh";
		case EAssetTypeOld::Shader: return "Shader";
		case EAssetTypeOld::Texture: return "Texture";
		case EAssetTypeOld::CubeMap: return "CubeMap";
		case EAssetTypeOld::LuaScript: return "LuaScript";
		}

		return "None";
	}

	static EAssetTypeOld AssetTypeFromString( const char* a_Type )
	{
		if ( strcmp( a_Type, "Scene" ) == 0 ) return EAssetTypeOld::Scene;
		if ( strcmp( a_Type, "Material" ) == 0 ) return EAssetTypeOld::Material;
		if ( strcmp( a_Type, "MeshSource" ) == 0 ) return EAssetTypeOld::MeshSource;
		if ( strcmp( a_Type, "Mesh" ) == 0 ) return EAssetTypeOld::StaticMesh;
		if ( strcmp( a_Type, "Shader" ) == 0 ) return EAssetTypeOld::Shader;
		if ( strcmp( a_Type, "Texture" ) == 0 ) return EAssetTypeOld::Texture;
		if ( strcmp( a_Type, "CubeMap" ) == 0 ) return EAssetTypeOld::CubeMap;
		if ( strcmp( a_Type, "LuaScript" ) == 0 ) return EAssetTypeOld::LuaScript;

		return EAssetTypeOld::None;
	}

	EAssetTypeOld GetAssetTypeFromFileExtension( const std::string& a_Extension );
}

namespace std {

	template <Tridium::EAssetTypeOld _AssetType>
	struct hash<Tridium::Internal::TypedAssetHandle<_AssetType>>
	{
		size_t operator()( const Tridium::Internal::TypedAssetHandle<_AssetType>& a_Handle ) const
		{
			return std::hash<Tridium::AssetHandle>()( a_Handle );
		}
	};
}