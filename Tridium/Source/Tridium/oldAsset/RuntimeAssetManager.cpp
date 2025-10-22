#include "tripch.h"
#include "RuntimeAssetManager.h"

namespace Tridium {
	SharedPtr<Asset> RuntimeAssetManager::GetAsset( OldAssetHandle a_Handle )
	{
		return SharedPtr<Asset>();
	}
	SharedPtr<Asset> RuntimeAssetManager::GetAsset( const FilePath& a_Path )
	{
		return SharedPtr<Asset>();
	}
	SharedPtr<Asset> RuntimeAssetManager::GetMemoryOnlyAsset( OldAssetHandle a_Handle )
	{
		return SharedPtr<Asset>();
	}
	AssetStorageIterator RuntimeAssetManager::GetAssets()
	{
		return AssetStorageIterator( m_LoadedAssets, m_MemoryAssets );
	}
	bool RuntimeAssetManager::AddMemoryOnlyAsset( OldAssetHandle a_Handle, SharedPtr<Asset> a_Asset )
	{
		return false;
	}
	bool RuntimeAssetManager::HasAsset( OldAssetHandle a_Handle )
	{
		return false;
	}
	void RuntimeAssetManager::RemoveAsset( OldAssetHandle a_Handle )
	{
	}
	EAssetTypeOld RuntimeAssetManager::GetAssetType( OldAssetHandle a_Handle )
	{
		return EAssetTypeOld();
	}
	bool RuntimeAssetManager::IsMemoryAsset( OldAssetHandle a_Handle )
	{
		return false;
	}
	void RuntimeAssetManager::RegisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency )
	{
	}
	void RuntimeAssetManager::UnregisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency )
	{
	}
} // namespace Tridium