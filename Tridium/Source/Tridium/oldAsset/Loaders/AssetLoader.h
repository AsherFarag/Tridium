#pragma once
#include <Tridium/oldAsset/AssetType.h>
#include <Tridium/oldAsset/AssetFactoryOld.h>
#include <Tridium/oldAsset/AssetMetaData.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

namespace Tridium {

	class IAssetLoaderOld
	{
	public:
		virtual void SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) = 0;
		virtual SharedPtr<Asset> LoadAsset( const OldAssetMetaData& a_MetaData ) = 0;
	};

}