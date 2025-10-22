#pragma once
#include "Asset.h"
#include <Tridium/IO/FilePath.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Containers/Map.h>

namespace Tridium {
	using AssetStorageType = UnorderedMap<OldAssetHandle, SharedPtr<Asset>>;
}

#include "AssetStorageIterator.h"

namespace Tridium {

	class AssetManagerBase
	{
	public:
		AssetManagerBase() = default;
		virtual ~AssetManagerBase() = default;

		virtual void Init() {};
		virtual void Shutdown() {};

		virtual SharedPtr<Asset> GetAsset( OldAssetHandle a_Handle ) = 0;
		virtual SharedPtr<Asset> GetAsset( const FilePath& a_Path ) = 0;
		virtual SharedPtr<Asset> GetMemoryOnlyAsset( OldAssetHandle a_Handle ) = 0;
		virtual AssetStorageIterator GetAssets() = 0;
		virtual bool AddMemoryOnlyAsset( OldAssetHandle a_Handle, SharedPtr<Asset> a_Asset ) = 0;
		virtual bool HasAsset( OldAssetHandle a_Handle ) = 0;
		virtual void RemoveAsset( OldAssetHandle a_Handle ) = 0;
		virtual EAssetTypeOld GetAssetType( OldAssetHandle a_Handle ) = 0;
		virtual bool IsMemoryAsset( OldAssetHandle a_Handle ) = 0;
		virtual void RegisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) = 0;
		virtual void UnregisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) = 0;
	};

} // namespace Tridium