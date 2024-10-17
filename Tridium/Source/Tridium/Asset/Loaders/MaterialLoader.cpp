#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/Serializer.h>

namespace Tridium {

	void MaterialLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
	}

	SharedPtr<Asset> MaterialLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
		return SharedPtr<Asset>();
	}
}