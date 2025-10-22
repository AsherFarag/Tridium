#include "tripch.h"
#include "AssetMetaData.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT( OldAssetMetaData )
		PROPERTY( Handle )
		PROPERTY( AssetType )
		PROPERTY( Path )
		PROPERTY( Name )
		PROPERTY( IsAssetLoaded )
	END_REFLECT( OldAssetMetaData )

	const OldAssetMetaData OldAssetMetaData::s_InvalidMetaData = 
	{ 
		OldAssetHandle::InvalidID,
		EAssetTypeOld::None,
		{}, // Path
		"", // Name
		false};
}