#pragma once
#include "AssetManager.h"

namespace Tridium {

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:

		// Inherited via AssetManagerBase
		SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) override;
		SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) override;
		bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset ) override;
		bool HasAsset( AssetHandle a_Handle ) override;
		void RemoveAsset( AssetHandle a_Handle ) override;
		EAssetType GetAssetType( AssetHandle a_Handle ) override;
		bool IsMemoryAsset( AssetHandle a_Handle ) override;
		void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;
		void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;
	};

}