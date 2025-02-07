#pragma once
#include "AssetLoader.h"
#include <Tridium/Graphics/Rendering/Shader.h>

namespace Tridium {

	class ShaderLoader : public IAssetLoader
	{
	public:

		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

}