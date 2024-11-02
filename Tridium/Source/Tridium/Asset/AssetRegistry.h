#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium {

	struct AssetRegistry
	{
		std::unordered_map<AssetHandle, AssetMetaData> AssetMetaData;
		// Key: Dependent, Value: Dependencies
		std::unordered_map<AssetHandle, std::unordered_set<AssetHandle>> AssetDependencies;
	};

}