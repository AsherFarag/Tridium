#include "tripch.h"
#include "ShaderLoader.h"
#include <Tridium/Graphics/Rendering/Shader.h>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

    void ShaderLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
		NOT_IMPLEMENTED;
    }

    SharedPtr<Asset> ShaderLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
        SharedPtr<Shader> shader;
        shader.reset( Shader::Create() );

        CORE_CHECK( shader != nullptr );

        shader->Compile( a_MetaData.Path );

        return SharedPtr<Asset>( shader );
    }

}