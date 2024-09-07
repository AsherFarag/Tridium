#include "tripch.h"
#include "AssetFactory.h"
#include "Asset.h"
#include "Loaders/AssetLoader.h"
#include <Tridium/Asset/AssetFileExtensions.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	void AssetFactory::RegisterLoader( EAssetType a_AssetType, IAssetLoaderInterface& a_AssetLoader )
	{
		if ( m_Loaders.find( a_AssetType ) != m_Loaders.end() )
			return;

		m_Loaders[a_AssetType] = &a_AssetLoader;
	}

	IAssetLoaderInterface* AssetFactory::GetLoader( EAssetType a_AssetType )
	{
		auto it = m_Loaders.find( a_AssetType );
		return it != m_Loaders.end() ? it->second : nullptr;
	}

	AssetRef<Asset> AssetFactory::LoadAsset( EAssetType a_AssetType, const IO::FilePath& a_Path )
	{
		IAssetLoaderInterface* loader = GetLoader( a_AssetType );
		return loader ? loader->Load( a_Path ) : nullptr;
	}

	AssetMetaData* AssetFactory::ConstructAssetMetaData( EAssetType a_AssetType )
	{
		IAssetLoaderInterface* loader = GetLoader( a_AssetType );
		return loader ? loader->ConstructAssetMetaData() : nullptr;
	}

	bool AssetFactory::SaveAsset( const IO::FilePath& a_Path, AssetRef<Asset> a_Asset )
	{
		IAssetLoaderInterface* loader = GetLoader( a_Asset->AssetType() );
		return loader ? loader->Save( a_Path, a_Asset.Get() ) : false;
	}
}