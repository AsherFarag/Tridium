#include "tripch.h"
#include "TextureLoader.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#include "HdriToCubemap.hpp"

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

        stbi_image_free( data );

        return SharedPtr<Asset>( tex );
	}

    void CubeMapLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
    }

    SharedPtr<Asset> CubeMapLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
		const int size = 1024 * 2;

		if ( stbi_is_hdr( a_MetaData.Path.ToString().c_str() ) )
		{
            HdriToCubemap<float> cubemap( a_MetaData.Path.ToString().c_str(), size, false );
            TextureSpecification specification;
            specification.Width = size;
            specification.Height = size;
            specification.TextureFormat = ETextureFormat::RGB32F;

            std::array<float*, 6> faces{ cubemap.getRight(), cubemap.getLeft(), cubemap.getDown(), cubemap.getUp(), cubemap.getFront(), cubemap.getBack() };

            SharedPtr<CubeMap> tex( CubeMap::Create( specification ) );
            tex->SetData( (void**)faces.data(), size * size * cubemap.getNumChannels() * 6 );

            return tex;
		}
        else
        {
            HdriToCubemap<unsigned char> cubemap( a_MetaData.Path.ToString().c_str(), size, true );
            TextureSpecification specification;
            specification.Width = size;
            specification.Height = size;
            specification.TextureFormat = ETextureFormat::RGB8;

            std::array<unsigned char*, 6> faces{ cubemap.getRight(), cubemap.getLeft(), cubemap.getDown(), cubemap.getUp(), cubemap.getFront(), cubemap.getBack() };

            SharedPtr<CubeMap> tex( CubeMap::Create( specification ) );
            tex->SetData( (void**)faces.data(), size * size * 4 * 6 );

            return tex;
        }
    }

}