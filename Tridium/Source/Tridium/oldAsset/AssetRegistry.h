#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/oldAsset/AssetMetaData.h>

namespace Tridium {

	struct AssetRegistry
	{
		std::unordered_map<AssetHandle, OldAssetMetaData> AssetMetaData;
		// Key: Dependent, Value: Dependencies
		std::unordered_map<AssetHandle, std::unordered_set<AssetHandle>> AssetDependencies;
	};

}