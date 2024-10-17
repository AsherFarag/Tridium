#pragma once
#include <Tridium/Asset/AssetType.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/AssetMetaData.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

namespace Tridium {

	class IAssetLoader
	{
	public:
		virtual void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset ) = 0;
		virtual SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData ) = 0;
	};

}