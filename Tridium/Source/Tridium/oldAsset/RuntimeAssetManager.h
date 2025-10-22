#pragma once
#include "AssetManager.h"

namespace Tridium {

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:

		// Inherited via AssetManagerBase
		SharedPtr<Asset> GetAsset( OldAssetHandle a_Handle ) override;
		SharedPtr<Asset> GetAsset( const FilePath& a_Path ) override;
		SharedPtr<Asset> GetMemoryOnlyAsset( OldAssetHandle a_Handle ) override;
		AssetStorageIterator GetAssets() override;
		bool AddMemoryOnlyAsset( OldAssetHandle a_Handle, SharedPtr<Asset> a_Asset ) override;
		bool HasAsset( OldAssetHandle a_Handle ) override;
		void RemoveAsset( OldAssetHandle a_Handle ) override;
		EAssetTypeOld GetAssetType( OldAssetHandle a_Handle ) override;
		bool IsMemoryAsset( OldAssetHandle a_Handle ) override;
		void RegisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) override;
		void UnregisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) override;

	private:
		AssetStorageType m_LoadedAssets;
		AssetStorageType m_MemoryAssets;
	};

}