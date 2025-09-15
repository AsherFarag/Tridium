#include "tripch.h"
#include "ShaderLoader.h"
#include <Tridium/Graphics/oldRendering/Shader.h>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

    void ShaderLoader::SaveAsset( const OldAssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
		NOT_IMPLEMENTED;
    }

    SharedPtr<Asset> ShaderLoader::LoadAsset( const OldAssetMetaData& a_MetaData )
    {
        SharedPtr<Shader> shader;
        shader.reset( Shader::Create() );

        CHECK( shader != nullptr );

        shader->Compile( a_MetaData.Path );

        return SharedPtr<Asset>( shader );
    }

}