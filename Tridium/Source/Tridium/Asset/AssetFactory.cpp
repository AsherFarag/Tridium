#include "tripch.h"
#include "AssetFactory.h"
#include "Asset.h"
#include "Loaders/AssetLoader.h"
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Asset/Loaders/ShaderLoader.h>
#include <Tridium/Asset/Loaders/MaterialLoader.h>
#include <Tridium/Asset/Loaders/MeshLoader.h>

namespace Tridium {

	std::unordered_map<EAssetType, SharedPtr<IAssetLoader>> AssetFactory::s_AssetFactories;

	void AssetFactory::Init()
	{
		s_AssetFactories[EAssetType::Texture] = MakeShared<TextureLoader>();
		s_AssetFactories[EAssetType::Shader] = MakeShared<ShaderLoader>();
		s_AssetFactories[EAssetType::Material] = MakeShared<MaterialLoader>();
		s_AssetFactories[EAssetType::MeshSource] = MakeShared<MeshSourceLoader>();
		s_AssetFactories[EAssetType::StaticMesh] = MakeShared<StaticMeshLoader>();
	}

	void AssetFactory::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
		auto it = s_AssetFactories.find( a_MetaData.AssetType );
		if ( it != s_AssetFactories.end() )
		{
			it->second->SaveAsset( a_MetaData, a_Asset );
		}
		else
		{
			TE_CORE_WARN( "No asset loader found for asset type {0}", AssetTypeToString( a_MetaData.AssetType ) );
		}
	}

	SharedPtr<Asset> AssetFactory::LoadAsset( const AssetMetaData& a_MetaData )
	{
		auto it = s_AssetFactories.find( a_MetaData.AssetType );
		if ( it != s_AssetFactories.end() )
		{
			SharedPtr<Asset> asset = it->second->LoadAsset( a_MetaData );
			if ( asset )
				asset->SetHandle( a_MetaData.Handle );

			return asset;
		}

		TE_CORE_WARN( "No asset loader found for asset type {0}", AssetTypeToString( a_MetaData.AssetType ) );
		return nullptr;
	}

	SharedPtr<IAssetLoader> AssetFactory::GetAssetLoader( EAssetType a_Type )
	{
		auto it = s_AssetFactories.find( a_Type );
		if ( it != s_AssetFactories.end() )
		{
			return it->second;
		}

		TE_CORE_WARN( "No asset loader found for asset type {0}", AssetTypeToString( a_Type ) );
		return nullptr;
	}
}