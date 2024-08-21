#include "tripch.h"
#include "TextureLoader.h"
#include <Tridium/Rendering/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Tridium {
	Texture* TextureLoader::Load( const IO::FilePath& a_Path )
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
	Texture* TextureLoader::Load( const IO::FilePath& a_Path, const TextureMetaData& a_MetaData )
    {
		TextureSpecification specification;
		std::string stringPath = a_Path.ToString();

		int width, height, channels;
		stbi_set_flip_vertically_on_load( 1 );
		stbi_uc* data = stbi_load( stringPath.c_str(), &width, &height, &channels, 0);

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

}