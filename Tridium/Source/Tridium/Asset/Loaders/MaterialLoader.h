#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium {

	class MaterialLoader : public IAssetLoader
	{
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

}