#include "tripch.h"
#include "AssetMetaData.h"
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT( AssetMetaData )
		PROPERTY( Handle )
		PROPERTY( AssetType )
		PROPERTY( Path )
		PROPERTY( Name )
		PROPERTY( IsAssetLoaded )
	END_REFLECT( AssetMetaData )

	const AssetMetaData AssetMetaData::s_InvalidMetaData = 
	{ 
		AssetHandle::InvalidID,
		EAssetType::None,
		{}, // Path
		"", // Name
		false};
}