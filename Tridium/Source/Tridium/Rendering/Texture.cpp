#include "tripch.h"
#include "Texture.h"

#include <Tridium/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>

#include <stb_image.h>

namespace Tridium {
	AssetRef<Texture> Texture::Load( const std::string& path )
	{
		TextureSpecification specification;

		int width, height, channels;
		stbi_set_flip_vertically_on_load( 1 );
		stbi_uc* data = stbi_load( path.c_str(), &width, &height, &channels, 0 );

		if ( !data )
			return nullptr;

		specification.Width = static_cast<uint32_t>( width );
		specification.Height = static_cast<uint32_t>( height );

		switch ( channels )
		{
		case 1:
		{
			specification.ImageFormat = EImageFormat::R;
			specification.DataFormat = EDataFormat::R8;
			break;
		}
		case 2:
		{
			specification.ImageFormat = EImageFormat::RG;
			specification.DataFormat = EDataFormat::RG8;
			break;
		}
		case 3:
		{
			specification.ImageFormat = EImageFormat::RGB;
			specification.DataFormat = EDataFormat::RGB8;
			break;
		}
		case 4:
		{
			specification.ImageFormat = EImageFormat::RGBA;
			specification.DataFormat = EDataFormat::RGBA8;
			break;
		}
		}

		AssetRef<Texture> tex = Texture::Create( specification );
		tex->m_Path = path;
		tex->m_Loaded = true;
		tex->SetData( data, width * height * channels );

		stbi_image_free( data );

		return tex;
	}

	AssetRef<Texture> Texture::Create( const TextureSpecification& specification )
	{
		switch ( Renderer::GetAPI() )
		{
		case RendererAPI::API::None:    TE_CORE_ASSERT( false, "RendererAPI::None is currently not supported!" ); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLTexture(specification);
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}
}