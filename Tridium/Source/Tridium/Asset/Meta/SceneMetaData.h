#pragma once
#include "AssetMetaData.h"

namespace Tridium {

	struct SceneMetaData : public AssetMetaData
	{
		SceneMetaData() : AssetMetaData( EAssetType::Scene ) {}
	};

}