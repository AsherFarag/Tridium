#include "tripch.h"
#include "ShaderLoader.h"
#include <Tridium/Asset/Meta/ShaderMetaData.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

    Shader* ShaderLoader::Load( const fs::path& a_Path, const ShaderMetaData& a_MetaData )
    {
        Shader* shader = Shader::Create();
        CHECK( shader );

        shader->m_Path = a_Path.string();
        shader->Recompile();

        return shader;
    }

}