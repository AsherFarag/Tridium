#pragma once
#include "AssetLoader.h"
#include <Tridium/Graphics/oldRendering/Material.h>

namespace Tridium {

	class MaterialLoader : public IAssetLoaderOld
	{
		void SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const OldAssetMetaData& a_MetaData ) override;
	};

}