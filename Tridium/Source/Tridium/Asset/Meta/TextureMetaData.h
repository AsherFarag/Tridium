#pragma once
#include "AssetMetaData.h"

namespace Tridium {

	struct TextureMetaData : public AssetMetaData
	{
		TextureMetaData() : AssetMetaData( EAssetType::Texture ) {}
	};

}