#pragma once
#include "AssetLoader.h"
#include <Tridium/Graphics/Rendering/Mesh.h>

namespace Tridium {

	class MeshSourceLoader : public IAssetLoader
	{
		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override {}
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

	class StaticMeshLoader : public IAssetLoader
	{
		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

}