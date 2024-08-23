#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

    Material* MaterialLoader::Load( const IO::FilePath& a_Path, const MaterialMetaData& a_MetaData )
    {
        Material mat;
        mat.AddProperty( "a_Color", { EPropertyType::Float, 10 } );
        //mat.GetProperty("a_Color")->Value
        MaterialSerializer serializer();
        return nullptr;
    }

}