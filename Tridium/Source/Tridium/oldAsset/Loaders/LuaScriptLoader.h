#pragma once
#include "AssetLoader.h"

namespace Tridium {
	class LuaScriptLoader : public IAssetLoaderOld
	{
	public:
		// Inherited via IAssetLoaderOld
		void SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const OldAssetMetaData& a_MetaData ) override;
	};
}