#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class TextureLoader : public IAssetLoader
	{
	public:
		static SharedPtr<Texture> LoadTexture( const IO::FilePath& a_FilePath )
		{
			AssetMetaData metaData;
			metaData.AssetType = EAssetType::Texture;
			metaData.Path = a_FilePath;

			SharedPtr<IAssetLoader> loader = AssetFactory::GetAssetLoader( metaData.AssetType );
			return SharedPtrCast<Texture>( loader->LoadAsset( metaData ) );
		}

		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

	class CubeMapLoader : public IAssetLoader
	{
	public:
		static SharedPtr<Texture> LoadCubeMap( const IO::FilePath& a_FilePath )
		{
			AssetMetaData metaData;
			metaData.AssetType = EAssetType::CubeMap;
			metaData.Path = a_FilePath;

			SharedPtr<IAssetLoader> loader = AssetFactory::GetAssetLoader( metaData.AssetType );
			return SharedPtrCast<Texture>( loader->LoadAsset( metaData ) );
		}

		// Inherited via IAssetLoader
		void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) override;
	};

}