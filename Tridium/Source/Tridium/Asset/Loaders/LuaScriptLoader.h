#pragma once
#include "AssetLoader.h"

namespace Tridium {
	class LuaScriptLoader : public IAssetLoaderOld
	{
	public:
		// Inherited via IAssetLoaderOld
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};
}