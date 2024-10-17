#pragma once
#include <Tridium/Utils/Reflection/ReflectionFwd.h>
#include <Tridium/Asset/Asset.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	struct AssetMetaData
	{
		REFLECT(AssetMetaData);

		AssetHandle Handle;
		EAssetType AssetType;
		IO::FilePath Path;
		std::string Name;
		bool IsAssetLoaded;

		static const AssetMetaData Invalid;

		bool IsValid() const { return Handle.Valid(); }
	};
}