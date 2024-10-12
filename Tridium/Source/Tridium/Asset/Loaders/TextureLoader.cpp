#include "tripch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Tridium {

    Texture* TextureLoader::Load( const IO::FilePath& a_Path )
    {
		static TextureLoader loader;
#if ASSET_USE_RUNTIME
		return static_cast<Texture*>( loader.RuntimeLoad( a_Path ) );
#else
		return static_cast<Texture*>( loader.DebugLoad( a_Path, nullptr ) );
#endif // ASSET_USE_RUNTIME
    }

    AssetMetaData* TextureLoader::LoadAssetMetaData( const YAML::Node & a_Node ) const
    {
        return nullptr;
    }

    AssetMetaData* TextureLoader::ConstructAssetMetaData() const
    {
        return new TextureMetaData();
    }

    Asset* TextureLoader::RuntimeLoad( const IO::FilePath& a_Path ) const
    {
		NOT_IMPLEMENTED;
        return nullptr;
    }

    Asset* TextureLoader::DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const
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

    bool TextureLoader::Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const
    {
        return false;
    }
}