#pragma once
#include <Tridium/Reflection/ReflectionFwd.h>
#include <Tridium/oldAsset/Asset.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	struct OldAssetMetaData
	{
		REFLECT(OldAssetMetaData);

		OldAssetHandle Handle;
		EAssetTypeOld AssetType;
		FilePath Path;
		std::string Name;
		bool IsAssetLoaded = false;

		static const OldAssetMetaData s_InvalidMetaData;

		bool IsValid() const { return Handle.IsValid(); }
	};
}