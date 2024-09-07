#include "tripch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Tridium {

    Texture* TextureLoader::Load( const IO::FilePath& a_Path )
    {
    #if ASSET_USE_RUNTIME
        return RuntimeLoad( a_Path );
    #else
        return DebugLoad( a_Path, nullptr );
    #endif
    }

    TextureMetaData* TextureLoader::ConstructMetaData()
    {
        return new TextureMetaData();
    }

    Texture* TextureLoader::RuntimeLoad( const IO::FilePath& a_Path )
    {
        return nullptr;
    }

    Texture* TextureLoader::DebugLoad( const IO::FilePath& a_Path, const TextureMetaData* a_MetaData )
    {
        TextureSpecification specification;
        std::string stringPath = a_Path.ToString();

        int width, height, channels;
        stbi_set_flip_vertically_on_load( 1 );
        stbi_uc* data = stbi_load( stringPath.c_str(), &width, &height, &channels, 0 );

        if ( !data )
            return nullptr;

        specification.Width = static_cast<uint32_t>( width );
        specification.Height = static_cast<uint32_t>( height );
        specification.DataFormat = static_cast<EDataFormat>( channels );

        Texture* tex = Texture::Create( specification );
        tex->m_Path = stringPath;
        tex->m_Loaded = true;
        tex->SetData( data, width * height * channels );

        stbi_image_free( data );

        return tex;
    }

    bool TextureLoader::Save( const IO::FilePath& a_Path, const Texture* a_Asset )
    {
        return false;
    }

    class TextureLoaderInterface : public IAssetLoaderInterface
    {
    public:
        TextureLoaderInterface()
        {
            AssetFactory::RegisterLoader( EAssetType::Texture, *this );
        }

        AssetMetaData* ConstructAssetMetaData() const override
        {
            return TextureLoader::ConstructMetaData();
        }

        Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override
        {
            return TextureLoader::RuntimeLoad( a_Path );
        }

        Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override
        {
            return TextureLoader::DebugLoad( a_Path, static_cast<const TextureMetaData*>( a_MetaData ) );
        }

        bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override
        {
            return TextureLoader::Save( a_Path, static_cast<const Texture*>( a_Asset ) );
        }
    };

    static TextureLoaderInterface s_Instance;
}