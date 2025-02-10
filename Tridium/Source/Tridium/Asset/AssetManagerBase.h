#pragma once
#include "Asset.h"
#include <Tridium/IO/FilePath.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Core/Containers/Map.h>

namespace Tridium {
	using AssetStorageType = UnorderedMap<AssetHandle, SharedPtr<Asset>>;
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

		virtual SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) = 0;
		virtual SharedPtr<Asset> GetAsset( const FilePath& a_Path ) = 0;
		virtual SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) = 0;
		virtual AssetStorageIterator GetAssets() = 0;
		virtual bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset ) = 0;
		virtual bool HasAsset( AssetHandle a_Handle ) = 0;
		virtual void RemoveAsset( AssetHandle a_Handle ) = 0;
		virtual EAssetType GetAssetType( AssetHandle a_Handle ) = 0;
		virtual bool IsMemoryAsset( AssetHandle a_Handle ) = 0;
		virtual void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) = 0;
		virtual void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) = 0;
	};

} // namespace Tridium