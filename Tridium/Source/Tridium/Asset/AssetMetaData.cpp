#include "tripch.h"
#include <Tridium/Utils/Reflection/Reflection.h>
#include "AssetMetaData.h"
#include <Tridium/Asset/AssetFileExtensions.h>
#include <Tridium/IO/SerializationUtil.h>

namespace Tridium {

    ModelMetaData::ModelMetaData()
        : AssetMetaData( EAssetType::Mesh )
    {
    }

    BEGIN_REFLECT( AssetMetaData );
        PROPERTY( FileFormatVersion );
        PROPERTY( Handle );
        PROPERTY( AssetType );
    END_REFLECT( AssetMetaData );

    BEGIN_REFLECT( ShaderMetaData );
        BASE( AssetMetaData );
    END_REFLECT( ShaderMetaData );

    BEGIN_REFLECT( SceneMetaData );
        BASE( AssetMetaData );
    END_REFLECT( SceneMetaData );

    BEGIN_REFLECT( TextureMetaData );
        BASE( AssetMetaData );
	END_REFLECT( TextureMetaData );

	BEGIN_REFLECT( MaterialMetaData )
		BASE( AssetMetaData )
	END_REFLECT( MaterialMetaData );

    BEGIN_REFLECT( ModelMetaData )
		BASE( AssetMetaData )
        PROPERTY( ImportSettings )
	END_REFLECT( ModelMetaData );
}