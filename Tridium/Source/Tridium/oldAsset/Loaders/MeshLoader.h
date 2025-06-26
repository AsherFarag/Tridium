#pragma once
#include "AssetLoader.h"
#include <Tridium/Graphics/oldRendering/Mesh.h>

namespace Tridium {

	class MeshSourceLoader : public IAssetLoaderOld
	{
		// Inherited via IAssetLoaderOld
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override {}
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

	class StaticMeshLoader : public IAssetLoaderOld
	{
		// Inherited via IAssetLoaderOld
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

}