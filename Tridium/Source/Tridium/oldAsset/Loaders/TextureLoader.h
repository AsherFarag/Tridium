#pragma once
#include "AssetLoader.h"
#include <Tridium/Graphics/oldRendering/Texture.h>

namespace Tridium {

	class TextureLoader : public IAssetLoaderOld
	{
	public:
		static SharedPtr<TextureOld> LoadTexture( TextureSpecificationOld a_Specification, const FilePath& a_FilePath );

		static SharedPtr<TextureOld> LoadTexture( const FilePath& a_FilePath )
		{
			OldAssetMetaData metaData;
			metaData.AssetType = EAssetTypeOld::Texture;
			metaData.Path = a_FilePath;

			SharedPtr<IAssetLoaderOld> loader = AssetFactoryOld::GetAssetLoader( metaData.AssetType );
			return SharedPtrCast<TextureOld>( loader->LoadAsset( metaData ) );
		}

		// Inherited via IAssetLoaderOld
		void SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const OldAssetMetaData& a_MetaData ) override;
	};

	class CubeMapLoader : public IAssetLoaderOld
	{
	public:
		static SharedPtr<TextureOld> LoadCubeMap( const FilePath& a_FilePath )
		{
			OldAssetMetaData metaData;
			metaData.AssetType = EAssetTypeOld::CubeMap;
			metaData.Path = a_FilePath;

			SharedPtr<IAssetLoaderOld> loader = AssetFactoryOld::GetAssetLoader( metaData.AssetType );
			return SharedPtrCast<TextureOld>( loader->LoadAsset( metaData ) );
		}

		// Inherited via IAssetLoaderOld
		void SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) override;
		SharedPtr<Asset> LoadAsset( const OldAssetMetaData& a_MetaData ) override;
	};

}