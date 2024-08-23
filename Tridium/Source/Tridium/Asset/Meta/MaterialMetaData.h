#pragma once
#include "AssetMetaData.h"

namespace Tridium {

	struct MaterialMetaData : public AssetMetaData
	{
		MaterialMetaData() : AssetMetaData( EAssetType::Material ) {}
	};

}