#include "tripch.h"
#include "AssetManager.h"
#include "Asset.h"

namespace Tridium {
    AssetManager* AssetManager::s_Instance = nullptr;

	AssetRef<Asset> AssetManager::GetAsset( const AssetHandle& a_AssetHandle, bool a_ShouldLoad )
	{
		if ( auto asset = s_Instance->GetAssetImpl( a_AssetHandle ) )
			return asset;

		if ( a_ShouldLoad )
		    return s_Instance->LoadAssetImpl( a_AssetHandle );

		return nullptr;
	}

	AssetRef<Asset> AssetManager::GetAsset( const IO::FilePath& a_Path, bool a_ShouldLoad )
	{ 
		if ( auto asset = s_Instance->GetAssetImpl( a_Path ) )
			return asset;

		if ( a_ShouldLoad )
			return s_Instance->LoadAssetImpl( a_Path );

		return nullptr;
	}

	AssetRef<Asset> AssetManager::LoadAsset( const AssetHandle& a_AssetHandle )
	{
		if ( auto asset = s_Instance->GetAssetImpl( a_AssetHandle ) )
			return asset;

		return s_Instance->LoadAssetImpl( a_AssetHandle );
	}

	AssetRef<Asset> AssetManager::LoadAsset( const IO::FilePath& a_Path )
	{
		if ( auto asset = s_Instance->GetAssetImpl( a_Path ) )
			return asset;

		return s_Instance->LoadAssetImpl( a_Path );
	}
}