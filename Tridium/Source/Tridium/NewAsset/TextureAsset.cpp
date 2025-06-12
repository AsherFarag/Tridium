#include "tripch.h"
#include "TextureAsset.h"
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium::T {

	SharedPtr<Texture> Texture::Create( Array<uint8_t>&& a_Data, const RHITextureDesc& a_RHIDesc, bool a_CommitToRHI )
	{
		auto textureAsset = MakeShared<Texture>( Private{} );
		textureAsset->UpdateTexture( a_Data, a_RHIDesc, a_CommitToRHI );
		return textureAsset;
	}

	SharedPtr<Texture> Texture::Create( Span<const uint8_t> a_Data, const RHITextureDesc& a_RHIDesc, bool a_CommitToRHI )
	{
		return Create( Array<uint8_t>( a_Data ), a_RHIDesc, a_CommitToRHI );
	}

	SharedPtr<Texture> Texture::Create( Array<uint8_t>&& a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize, ERHITextureDimension a_Dimension, ERHIFormat a_Format, StringView a_Name, bool a_CommitToRHI )
	{
		RHITextureDesc desc(
			a_Name, a_Dimension, a_Width, a_Height, a_DepthOrArraySize, a_Format );
		desc.BindFlags = ERHIBindFlags::ShaderResource;

		return Create( a_Data, desc, a_CommitToRHI );
	}

	SharedPtr<Texture> Texture::Create( 
		Span<const uint8_t> a_Data,
		uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
		ERHITextureDimension a_Dimension, ERHIFormat a_Format, StringView a_Name, bool a_CommitToRHI )
	{
		return Create( Array<uint8_t>( a_Data ), a_Width, a_Height, a_DepthOrArraySize, a_Dimension, a_Format, a_Name, a_CommitToRHI );
	}

	Optional<Color> Texture::TryGetPixel( uint32_t a_X, uint32_t a_Y, uint32_t a_Z ) const
	{
		if ( m_PixelData.Empty() || a_X >= m_RHIDesc.Width || a_Y >= m_RHIDesc.Height || a_Z >= m_RHIDesc.Depth )
		{
			return std::nullopt;
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( m_RHIDesc.Format );
		const size_t pixelSize = formatInfo.Bytes();
		const size_t rowStride = m_RHIDesc.Width * pixelSize;
		const size_t depthStride = m_RHIDesc.Depth * rowStride;
		const size_t offset = (a_Z * depthStride) + (a_Y * rowStride) + (a_X * pixelSize);
		if ( offset + pixelSize > m_PixelData.Size() )
		{
			return std::nullopt; // Out of bounds
		}

		Span<const uint8_t> pixelData{ m_PixelData.Data() + offset, pixelSize };
		return formatInfo.ConvertToColor( pixelData );
	}

	void Texture::UpdateTexture( Array<uint8_t>&& a_Data, RHITextureDesc a_Desc, bool a_CommitToRHI )
	{
		RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Desc.Format );
		const size_t expectedTexSize = a_Desc.Width * a_Desc.Height * a_Desc.Depth * formatInfo.Bytes();
		ASSERT( a_Data.Size() == expectedTexSize, "Texture data size does not match expected size based on RHI texture description." );

		m_PixelData = std::move( a_Data );
		m_RHIDesc = a_Desc;
		m_RHITexture.reset();

		if ( a_CommitToRHI )
		{
			RHITextureSubresourceData subresourceData;
			subresourceData.Data = m_PixelData.Data();
			subresourceData.RowStride = m_RHIDesc.Width * formatInfo.Bytes();
			subresourceData.DepthStride = m_RHIDesc.Depth * subresourceData.RowStride;
			m_RHITexture = RHI::CreateTexture( m_RHIDesc, subresourceData );
			ASSERT( m_RHITexture->Valid(), "Failed to create RHI texture from asset data." );
		}
	}

}
