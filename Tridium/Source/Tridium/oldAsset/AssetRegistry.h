#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/oldAsset/AssetMetaData.h>

namespace Tridium {

	struct AssetRegistry
	{
		std::unordered_map<OldAssetHandle, OldAssetMetaData> AssetMetaData;
		// Key: Dependent, Value: Dependencies
		std::unordered_map<OldAssetHandle, std::unordered_set<OldAssetHandle>> AssetDependencies;
	};

}