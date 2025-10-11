#include "tripch.h"

#if USE_ASSET_IMPORTERS

#include "TextureImporter.h"
#include <Tridium/Asset/TextureAsset.h>

#include <stb_image.h>
#include <imageinfo/imageinfo.hpp>


namespace Tridium {

	REGISTER_ASSET_IMPORTER( TextureImporter );

	bool TextureImporter::OnImport( AssetImportContext& a_Context )
	{
		NOT_IMPLEMENTED;
		return false;
	}

	Expected<void, String> TextureImporter::LoadFromFile( const char* a_FilePath,
						   Array<byte_t>& o_Data, uint32_t& o_Width, uint32_t& o_Height, ERHIFormat& o_Format, bool& o_IsFloat,
						   bool a_FlipOnLoad, int a_DesiredChannels, bool a_LoadAsSRGB )
	{
		PROFILE_FUNCTION( ProfilerCategory::AssetStreaming );

		o_Width = 0;
		o_Height = 0;
		o_Format = ERHIFormat::Unknown;
		o_IsFloat = false;

		stbi_set_flip_vertically_on_load( a_FlipOnLoad );

		// Get info about the texture
		int width, height, channels;
		if ( !stbi_info( a_FilePath, &width, &height, &channels ) )
		{
			return Unexpected( std::format( "Failed to get info for texture file '{}': {}", a_FilePath, stbi_failure_reason() ) );
		}
		channels = a_DesiredChannels == 0 ? channels : a_DesiredChannels;

		o_IsFloat = stbi_is_hdr( a_FilePath );

		if ( o_IsFloat )
		{
			int dummy; // For some reason, stbi_loadf requires this parameter
			float* data = stbi_loadf( a_FilePath, &width, &height, &dummy, a_DesiredChannels );

			if ( !data )
			{
				return Unexpected( std::format( "Failed to load HDR texture file '{}': {}", a_FilePath, stbi_failure_reason() ) );
			}

			o_Width = Cast<uint32_t>( width );
			o_Height = Cast<uint32_t>( height );

			switch ( channels )
			{
				case 1: o_Format = ERHIFormat::R32_FLOAT; break;
				case 2: o_Format = ERHIFormat::RG32_FLOAT; break;
				case 3: o_Format = ERHIFormat::RGB32_FLOAT; break;
				case 4: o_Format = ERHIFormat::RGBA32_FLOAT; break;
				default: return Unexpected( std::format( "Unsupported number of channels in HDR texture: {}", channels ) );
			}

			const size_t dataSize = o_Width * o_Height * channels * sizeof( float );
			o_Data.Resize( dataSize );
			memcpy( o_Data.Data(), data, dataSize );

			stbi_image_free( data );
		}
		// Otherwise, assume it's a standard 8-bit texture
		else
		{
			// We don't support 3-channel textures, so we convert them to 4-channel
			const int desiredChannels = channels == 3 ? 4 : channels;
			channels = desiredChannels;
			uint8_t* data = stbi_load( a_FilePath, &width, &height, nullptr, desiredChannels );

			if ( !data )
			{
				return Unexpected( std::format( "Failed to load texture file '{}': {}", a_FilePath, stbi_failure_reason() ) );
			}

			o_Width = Cast<uint32_t>( width );
			o_Height = Cast<uint32_t>( height );
			o_IsFloat = false;

			switch ( channels )
			{
				case 1: o_Format = ERHIFormat::R8_UNORM; break;
				case 2: o_Format = ERHIFormat::RG8_UNORM; break;
				case 4: o_Format = a_LoadAsSRGB ? ERHIFormat::SRGBA8_UNORM : ERHIFormat::RGBA8_UNORM; break;
				default: return Unexpected( std::format( "Unsupported number of channels in texture: {}", channels ) );
			}

			const size_t dataSize = o_Width * o_Height * channels * sizeof( uint8_t );
			o_Data.Resize( dataSize );
			memcpy( o_Data.Data(), data, dataSize );

			stbi_image_free( data );
		}

		return {};
	}

} // namespace Tridium

#endif // USE_ASSET_IMPORTERS