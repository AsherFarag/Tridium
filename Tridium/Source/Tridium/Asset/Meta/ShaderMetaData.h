#pragma once

#include "AssetMetaData.h"

namespace Tridium {

	struct ShaderMetaData : public AssetMetaData
	{
		ShaderMetaData() : AssetMetaData( EAssetType::Shader ) {}
	};

}