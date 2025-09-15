#include "tripch.h"
#include "AssetFactoryOld.h"
#include "Asset.h"
#include "Loaders/AssetLoader.h"
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

#include <Tridium/oldAsset/Loaders/SceneLoader.h>
#include <Tridium/oldAsset/Loaders/TextureLoader.h>
#include <Tridium/oldAsset/Loaders/ShaderLoader.h>
#include <Tridium/oldAsset/Loaders/MaterialLoader.h>
#include <Tridium/oldAsset/Loaders/MeshLoader.h>
#include <Tridium/oldAsset/Loaders/LuaScriptLoader.h>

namespace Tridium {

	std::unordered_map<EAssetTypeOld, SharedPtr<IAssetLoaderOld>> AssetFactoryOld::s_AssetFactories;

	void AssetFactoryOld::Init()
	{
		s_AssetFactories[EAssetTypeOld::Scene] = MakeShared<SceneLoader>();
		s_AssetFactories[EAssetTypeOld::Texture] = MakeShared<TextureLoader>();
		s_AssetFactories[EAssetTypeOld::CubeMap] = MakeShared<CubeMapLoader>();
		s_AssetFactories[EAssetTypeOld::Shader] = MakeShared<ShaderLoader>();
		s_AssetFactories[EAssetTypeOld::Material] = MakeShared<MaterialLoader>();
		s_AssetFactories[EAssetTypeOld::MeshSource] = MakeShared<MeshSourceLoader>();
		s_AssetFactories[EAssetTypeOld::StaticMesh] = MakeShared<StaticMeshLoader>();
		s_AssetFactories[EAssetTypeOld::LuaScript] = MakeShared<LuaScriptLoader>();
	}

	void AssetFactoryOld::SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
		auto it = s_AssetFactories.find( a_MetaData.AssetType );
		if ( it != s_AssetFactories.end() )
		{
			it->second->SaveAsset( a_MetaData, a_Asset );
		}
		else
		{
			LOG( LogCategory::Asset, Warn, "No asset loader found for asset type {0}", AssetTypeToString( a_MetaData.AssetType ) );
		}
	}

	SharedPtr<Asset> AssetFactoryOld::LoadAsset( const OldAssetMetaData& a_MetaData )
	{
		PROFILE_FUNCTION( ProfilerCategory::AssetStreaming );
		auto it = s_AssetFactories.find( a_MetaData.AssetType );
		if ( it != s_AssetFactories.end() )
		{
			SharedPtr<Asset> asset = it->second->LoadAsset( a_MetaData );
			if ( asset )
				asset->SetHandle( a_MetaData.Handle );

			return asset;
		}

		LOG( LogCategory::Asset, Warn, "No asset loader found for asset type {0}", AssetTypeToString( a_MetaData.AssetType ) );
		return nullptr;
	}

	SharedPtr<IAssetLoaderOld> AssetFactoryOld::GetAssetLoader( EAssetTypeOld a_Type )
	{
		auto it = s_AssetFactories.find( a_Type );
		if ( it != s_AssetFactories.end() )
		{
			return it->second;
		}

		LOG( LogCategory::Asset, Warn, "No asset loader found for asset type {0}", AssetTypeToString( a_Type ) );
		return nullptr;
	}
}