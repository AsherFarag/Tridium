#include "tripch.h"
#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Bitmap.h>

namespace Tridium {
	SharedPtr<Texture> TextureLoader::LoadTexture( TextureSpecification a_Specification, const FilePath& a_FilePath )
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load( 1 );
		if ( stbi_is_hdr( a_FilePath.ToString().c_str() ) )
		{
			float* data = stbi_loadf( a_FilePath.ToString().c_str(), &width, &height, &channels, 0 );

			if ( !data )
				return nullptr;

			a_Specification.Width = static_cast<uint32_t>( width );
			a_Specification.Height = static_cast<uint32_t>( height );
			switch ( channels )
			{
			case 3:
				a_Specification.TextureFormat = ETextureFormat::RGB32F;
				break;
			case 4:
				a_Specification.TextureFormat = ETextureFormat::RGBA32F;
				break;
			default:
				TE_CORE_ASSERT( false, "Unknown texture format!" );
				break;
			}

			Texture* tex = Texture::Create( a_Specification );
			tex->SetData( data, width * height * channels );

			return SharedPtr<Texture>( tex );
		}
		else
		{
			stbi_uc* data = stbi_load( a_FilePath.ToString().c_str(), &width, &height, &channels, 0 );

			if ( !data )
				return nullptr;

			a_Specification.Width = static_cast<uint32_t>( width );
			a_Specification.Height = static_cast<uint32_t>( height );
			switch ( channels )
			{
			case 1:
				a_Specification.TextureFormat = ETextureFormat::R8;
				break;
			case 2:
				a_Specification.TextureFormat = ETextureFormat::RG8;
				break;
			case 3:
				a_Specification.TextureFormat = ETextureFormat::RGB8;
				break;
			case 4:
				a_Specification.TextureFormat = ETextureFormat::RGBA8;
				break;
			default:
				TE_CORE_ASSERT( false, "Unknown texture format!" );
				break;
			}

			Texture* tex = Texture::Create( a_Specification );
			tex->SetData( data, width * height * channels );

			//stbi_image_free( data );

			return SharedPtr<Texture>( tex );
		}
	}

	void TextureLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
	}

	SharedPtr<Asset> TextureLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
        TextureSpecification specification;

        int width, height, channels;
        stbi_set_flip_vertically_on_load( 1 );
		if ( stbi_is_hdr( a_MetaData.Path.ToString().c_str() ) )
		{
			float* data = stbi_loadf( a_MetaData.Path.ToString().c_str(), &width, &height, &channels, 0 );

			if ( !data )
				return nullptr;

			specification.Width = static_cast<uint32_t>( width );
			specification.Height = static_cast<uint32_t>( height );
			switch ( channels )
			{
			case 3:
				specification.TextureFormat = ETextureFormat::RGB32F;
				break;
			case 4:
				specification.TextureFormat = ETextureFormat::RGBA32F;
				break;
			default:
				TE_CORE_ASSERT( false, "Unknown texture format!" );
				break;
			}

			Texture* tex = Texture::Create( specification );
			tex->SetData( data, width * height * channels );

			return SharedPtr<Asset>( tex );
		}
		else
		{
			stbi_uc* data = stbi_load( a_MetaData.Path.ToString().c_str(), &width, &height, &channels, 0 );

			if ( !data )
				return nullptr;

				specification.Width = static_cast<uint32_t>( width );
				specification.Height = static_cast<uint32_t>( height );
				switch ( channels )
				{
				case 1:
					specification.TextureFormat = ETextureFormat::R8;
					break;
				case 2:
					specification.TextureFormat = ETextureFormat::RG8;
					break;
				case 3:
					specification.TextureFormat = ETextureFormat::RGB8;
					break;
				case 4:
					specification.TextureFormat = ETextureFormat::RGBA8;
					break;
				default:
					TE_CORE_ASSERT( false, "Unknown texture format!" );
					break;
				}

			Texture* tex = Texture::Create( specification );
			tex->SetData( data, width * height * channels );

			//stbi_image_free( data );

			return SharedPtr<Asset>( tex );
		}
	}

    void CubeMapLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
    }

    SharedPtr<Asset> CubeMapLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
		if ( !stbi_is_hdr( a_MetaData.Path.ToString().c_str() ) )
			return nullptr;

		int width, height, channels;
		stbi_set_flip_vertically_on_load( 0 );
		float* data = stbi_loadf( a_MetaData.Path.ToString().c_str(), &width, &height, &channels, 0 );

		if ( !data )
			return nullptr;

		TextureSpecification specification;
		specification.Width = static_cast<uint32_t>( width );
		specification.Height = static_cast<uint32_t>( height );
		switch ( channels )
		{
		case 3:
			specification.TextureFormat = ETextureFormat::RGB32F;
			break;
		case 4:
			specification.TextureFormat = ETextureFormat::RGBA32F;
			break;
		default:
			TE_CORE_ASSERT( false, "Unknown texture format!" );
			break;
		}

		SharedPtr<Texture> tex( Texture::Create( specification ) );
		tex->SetData( data, width * height * channels );

		constexpr int SIZE = 1024 * 2;
		TextureSpecification spec;
		spec.Width = SIZE;
		spec.Height = SIZE;
		spec.TextureFormat = ETextureFormat::RGB32F;

		SharedPtr<CubeMap> cubemap( CubeMap::Create( spec, tex ) );
		return cubemap;
    }

}