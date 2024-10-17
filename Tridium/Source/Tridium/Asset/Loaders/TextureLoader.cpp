#include "tripch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Tridium {

	void TextureLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
	}

	SharedPtr<Asset> TextureLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
        TextureSpecification specification;

        int width, height, channels;
        stbi_set_flip_vertically_on_load( 1 );
        stbi_uc* data = stbi_load( a_MetaData.Path.ToString().c_str(), &width, &height, &channels, 0 );

        if ( !data )
            return nullptr;

        specification.Width = static_cast<uint32_t>( width );
        specification.Height = static_cast<uint32_t>( height );
        specification.DataFormat = static_cast<EDataFormat>( channels );

        Texture* tex = Texture::Create( specification );
        tex->SetData( data, width * height * channels );

        stbi_image_free( data );

        return SharedPtr<Asset>( tex );
	}

}