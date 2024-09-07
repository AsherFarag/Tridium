#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

    Material* MaterialLoader::Load( const IO::FilePath& a_Path )
    {
    #if ASSET_USE_RUNTIME
        return RuntimeLoad( a_Path );
    #else
        return DebugLoad( a_Path, nullptr );
    #endif
    }

    MaterialMetaData* MaterialLoader::ConstructMetaData()
    {
        return new MaterialMetaData();
    }

    Material* MaterialLoader::RuntimeLoad( const IO::FilePath& a_Path )
    {
        return nullptr;
    }

    Material* MaterialLoader::DebugLoad( const IO::FilePath& a_Path, const MaterialMetaData* a_MetaData )
    {
        return nullptr;
    }

    bool MaterialLoader::Save( const IO::FilePath& a_Path, const Material* a_Asset )
    {
        return false;
    }

    class MaterialLoaderInterface : public IAssetLoaderInterface
    {
    public:
        MaterialLoaderInterface()
        {
            AssetFactory::RegisterLoader( EAssetType::Material, *this );
        }

        AssetMetaData* ConstructAssetMetaData() const override
        {
            return MaterialLoader::ConstructMetaData();
        }

        Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override
        {
            return MaterialLoader::RuntimeLoad( a_Path );
        }

        Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override
        {
            return MaterialLoader::DebugLoad( a_Path, static_cast<const MaterialMetaData*>( a_MetaData ) );
        }

        bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override
        {
            return MaterialLoader::Save( a_Path, static_cast<const Material*>( a_Asset ) );
        }
    };

    static MaterialLoaderInterface s_Instance;
}