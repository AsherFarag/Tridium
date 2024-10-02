#include "tripch.h"
#include "RuntimeAssetManager.h"

namespace Tridium {

    bool RuntimeAssetManager::HasAssetImpl( const AssetHandle& a_AssetHandle ) const
    {
        return false;
    }

    AssetRef<Asset> RuntimeAssetManager::GetAssetImpl( const AssetHandle& a_AssetHandle )
    {
        return AssetRef<Asset>();
    }

    AssetRef<Asset> RuntimeAssetManager::GetAssetImpl( const IO::FilePath& a_Path )
    {
        return AssetRef<Asset>();
    }

    AssetRef<Asset> RuntimeAssetManager::LoadAssetImpl( const AssetHandle& a_AssetHandle )
    {
        return AssetRef<Asset>();
    }

    AssetRef<Asset> RuntimeAssetManager::LoadAssetImpl( const IO::FilePath& a_Path )
    {
        return AssetRef<Asset>();
    }

    bool RuntimeAssetManager::ReleaseAssetImpl( const AssetHandle& a_AssetHandle )
    {
        return false;
    }

} // namespace Tridium