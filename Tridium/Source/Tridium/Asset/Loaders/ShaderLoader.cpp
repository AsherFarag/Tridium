#include "tripch.h"
#include "ShaderLoader.h"
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

    AssetMetaData* ShaderLoader::LoadAssetMetaData( const YAML::Node & a_Node ) const
    {
        Scope<ShaderMetaData> metaData = new ShaderMetaData();

		Refl::Internal::DeserializeFunc deserializeFunc;
        if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass<ShaderMetaData>( deserializeFunc, Refl::Internal::YAMLDeserializeFuncID ) )
            return nullptr;

        Refl::MetaAny asAny( *metaData );
        deserializeFunc( a_Node, asAny );

		metaData.Retire();
        return metaData.Get();
    }

    AssetMetaData* ShaderLoader::ConstructAssetMetaData() const
    {
        return new ShaderMetaData();
    }

    Asset* ShaderLoader::RuntimeLoad( const IO::FilePath& a_Path ) const
    {
        return nullptr;
    }

    Asset* ShaderLoader::DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const
    {
        Shader* shader = Shader::Create();
        CHECK( shader );

        shader->m_Path = a_Path.ToString();
        shader->Recompile();

        return shader;
    }

    bool ShaderLoader::Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const
    {
        return false;
    }
}