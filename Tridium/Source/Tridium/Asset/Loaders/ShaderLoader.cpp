#include "tripch.h"
#include "ShaderLoader.h"
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

    Shader* ShaderLoader::Load( const IO::FilePath& a_Path )
    {
    #if ASSET_USE_RUNTIME
        return RuntimeLoad( a_Path );
    #else
        return DebugLoad( a_Path, nullptr );
    #endif
    }

    ShaderMetaData* ShaderLoader::ConstructMetaData()
    {
        return new ShaderMetaData();
    }

    Shader* ShaderLoader::RuntimeLoad( const IO::FilePath& a_Path )
    {
        return nullptr;
    }

    Shader* ShaderLoader::DebugLoad( const IO::FilePath& a_Path, const ShaderMetaData* a_MetaData )
    {
        Shader* shader = Shader::Create();
        CHECK( shader );

        shader->m_Path = a_Path.ToString();
        shader->Recompile();

        return shader;
    }

    bool ShaderLoader::Save( const IO::FilePath& a_Path, const Shader* a_Asset )
    {
        return false;
    }

    class ShaderLoaderInterface : public IAssetLoaderInterface
    {
    public:
        ShaderLoaderInterface()
        {
            AssetFactory::RegisterLoader( EAssetType::Shader, *this );
        }

        AssetMetaData* ConstructAssetMetaData() const override
        {
            return ShaderLoader::ConstructMetaData();
        }

        Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override
        {
            return ShaderLoader::RuntimeLoad( a_Path );
        }

        Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override
        {
            return ShaderLoader::DebugLoad( a_Path, static_cast<const ShaderMetaData*>( a_MetaData ) );
        }

        bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override
        {
            return ShaderLoader::Save( a_Path, static_cast<const Shader*>( a_Asset ) );
        }
    };

    static ShaderLoaderInterface s_Instance;
}