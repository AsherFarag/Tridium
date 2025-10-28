#include "tripch.h"
#include "TextureAsset.h"

namespace Tridium {

	Optional<Color4> Texture::TryGetPixel( uint32_t a_X, uint32_t a_Y, uint32_t a_Z ) const
	{
		if ( m_PixelData.Empty() || a_X >= m_Width || a_Y >= m_Height || a_Z >= m_DepthOrArraySize )
		{
			return std::nullopt;
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( m_Format );
		const size_t pixelSize = formatInfo.Bytes();
		const size_t rowStride = m_Width * pixelSize;
		const size_t depthStride = m_DepthOrArraySize * rowStride;
		const size_t offset = (a_Z * depthStride) + (a_Y * rowStride) + (a_X * pixelSize);
		if ( offset + pixelSize > m_PixelData.Size() )
		{
			return std::nullopt; // Out of bounds
		}

		Span<const uint8_t> pixelData{ m_PixelData.Data() + offset, pixelSize };
		return formatInfo.ConvertToColor( pixelData );
	}

} // namespace Tridium
