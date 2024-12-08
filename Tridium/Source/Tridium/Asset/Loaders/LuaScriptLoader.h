#pragma once
#include "AssetLoader.h"

namespace Tridium {
	class LuaScriptLoader : public IAssetLoader
	{
	public:
		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};
}