#include "tripch.h"
#include "TextureAsset.h"
#include <Tridium/Graphics/RHI/RHI.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

namespace Tridium {

	SharedPtr<Texture> Texture::Create( Array<uint8_t>&& a_Data, TextureSpecification a_Spec )
	{
		auto textureAsset = Create();
		textureAsset->UpdateTexture( a_Data, a_Spec );
		return textureAsset;
	}

	SharedPtr<Texture> Texture::Create( Span<const uint8_t> a_Data, TextureSpecification a_Spec )
	{
		return Create( Array<uint8_t>( a_Data ), a_Spec );
	}

	SharedPtr<Texture> Texture::Create( 
		Array<uint8_t>&& a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize, ERHITextureDimension a_Dimension, ERHIFormat a_Format )
	{
		TextureSpecification spec{ .Width = a_Width, .Height = a_Height, .DepthOrArraySize = a_DepthOrArraySize, .Dimension = a_Dimension, .Format = a_Format };

		return Create( a_Data, spec );
	}

	SharedPtr<Texture> Texture::Create( 
		Span<const uint8_t> a_Data,
		uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
		ERHITextureDimension a_Dimension, ERHIFormat a_Format )
	{
		return Create( Array<uint8_t>( a_Data ), a_Width, a_Height, a_DepthOrArraySize, a_Dimension, a_Format );
	}

	Optional<Color> Texture::TryGetPixel( uint32_t a_X, uint32_t a_Y, uint32_t a_Z ) const
	{
		if ( m_PixelData.Empty() || a_X >= m_Specification.Width || a_Y >= m_Specification.Height || a_Z >= m_Specification.DepthOrArraySize )
		{
			return std::nullopt;
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( m_Specification.Format );
		const size_t pixelSize = formatInfo.Bytes();
		const size_t rowStride = m_Specification.Width * pixelSize;
		const size_t depthStride = m_Specification.DepthOrArraySize * rowStride;
		const size_t offset = (a_Z * depthStride) + (a_Y * rowStride) + (a_X * pixelSize);
		if ( offset + pixelSize > m_PixelData.Size() )
		{
			return std::nullopt; // Out of bounds
		}

		Span<const uint8_t> pixelData{ m_PixelData.Data() + offset, pixelSize };
		return formatInfo.ConvertToColor( pixelData );
	}

	void Texture::UpdateTexture( Array<uint8_t>&& a_Data, TextureSpecification a_Desc )
	{
		RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Desc.Format );
		const size_t expectedTexSize = a_Desc.Width * a_Desc.Height * a_Desc.DepthOrArraySize * formatInfo.Bytes();
		ASSERT( a_Data.Size() == expectedTexSize, "Texture data size does not match expected size based on RHI texture description." );

		m_PixelData = std::move( a_Data );
		m_Specification = a_Desc;
		m_RHITexture.reset();

		//if ( a_CommitToRHI )
		//{
		//	RHITextureSubresourceData subresourceData;
		//	subresourceData.Data = m_PixelData.Data();
		//	subresourceData.RowStride = m_Specification.Width * formatInfo.Bytes();
		//	subresourceData.DepthStride = m_Specification.Depth * subresourceData.RowStride;
		//	m_RHITexture = RHI::CreateTexture( m_Specification, subresourceData );
		//	ASSERT( m_RHITexture->Valid(), "Failed to create RHI texture from asset data." );
		//}
	}

	Expected<SharedPtr<Texture>, String> Texture::Load( const char* a_FilePath )
	{
		int width, height, channels;
		const bool isHDR = stbi_is_hdr( a_FilePath );
		stbi_info( a_FilePath, &width, &height, &channels );
		const int desiredChannels = channels == 1 ? 1 : (channels == 2 ? 2 : 4); // Force to 1, 2 or 4 channels

		uint8_t* data = nullptr;
		if ( isHDR )
			data = ReinterpretCast<uint8_t*>( stbi_loadf( a_FilePath, &width, &height, &channels, desiredChannels ) );
		else 
			data = stbi_load( a_FilePath, &width, &height, &channels, desiredChannels );

		if ( !data )
			return Unexpected( std::format( "STB Load Failure: {}", stbi_failure_reason() ) );

		channels = 4;

		// Since we create a copy of the data, we need to ensure it is freed properly using a scope guard.
		ScopeGuard stbiFreeGuard( [&data]() { stbi_image_free( data ); } );

		TextureSpecification texSpec;
		texSpec.Width = Cast<uint32_t>( width );
		texSpec.Height = Cast<uint32_t>( height );
		texSpec.DepthOrArraySize = 1;

		if ( isHDR )
		{
			switch ( channels )
			{
			case 1: texSpec.Format = ERHIFormat::R32_FLOAT; break;
			case 2: texSpec.Format = ERHIFormat::RG32_FLOAT; break;
			case 4: texSpec.Format = ERHIFormat::RGBA32_FLOAT; break;
			default: return Unexpected( "Unsupported 32-bit texture format!" );
			}
		}
		else
		{
			switch ( channels )
			{
			case 1: texSpec.Format = ERHIFormat::R8_UNORM; break;
			case 2: texSpec.Format = ERHIFormat::RG8_UNORM; break;
			case 4: texSpec.Format = ERHIFormat::RGBA8_UNORM; break;
			default: return Unexpected( "Unsupported 8-Bit texture format!" );
			}
		}

		TODO( "Make Array be able to take in a raw pointer and size and own that instead of copying" );
		const size_t texDataSize = width * height * channels * (isHDR ? sizeof( float ) : sizeof( uint8_t ));
		Span<uint8_t> texData{ data, texDataSize };
		SharedPtr<Texture> textureAsset = Create( texData, texSpec );
		textureAsset->m_AssetFlags.RemoveFlag( EAssetFlags::MemoryOnly ); // Ensure it's not memory-only
		return textureAsset;
	}

}
