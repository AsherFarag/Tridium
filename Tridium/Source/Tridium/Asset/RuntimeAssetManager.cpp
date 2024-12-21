#include "tripch.h"
#include "RuntimeAssetManager.h"

namespace Tridium {
	SharedPtr<Asset> RuntimeAssetManager::GetAsset( AssetHandle a_Handle )
	{
		return SharedPtr<Asset>();
	}
	SharedPtr<Asset> RuntimeAssetManager::GetAsset( const IO::FilePath& a_Path )
	{
		return SharedPtr<Asset>();
	}
	SharedPtr<Asset> RuntimeAssetManager::GetMemoryOnlyAsset( AssetHandle a_Handle )
	{
		return SharedPtr<Asset>();
	}
	AssetStorageIterator RuntimeAssetManager::GetAssets()
	{
		return AssetStorageIterator( m_LoadedAssets, m_MemoryAssets );
	}
	bool RuntimeAssetManager::AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset )
	{
		return false;
	}
	bool RuntimeAssetManager::HasAsset( AssetHandle a_Handle )
	{
		return false;
	}
	void RuntimeAssetManager::RemoveAsset( AssetHandle a_Handle )
	{
	}
	EAssetType RuntimeAssetManager::GetAssetType( AssetHandle a_Handle )
	{
		return EAssetType();
	}
	bool RuntimeAssetManager::IsMemoryAsset( AssetHandle a_Handle )
	{
		return false;
	}
	void RuntimeAssetManager::RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency )
	{
	}
	void RuntimeAssetManager::UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency )
	{
	}
} // namespace Tridium