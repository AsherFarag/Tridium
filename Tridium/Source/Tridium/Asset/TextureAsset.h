#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>

namespace Tridium {

	//=================================================================================================
	// Texture Asset
	//=================================================================================================
	DEFINE_ASSET_TYPE( Texture )
	{
	public:

		//=============================================================================================
		bool Valid() const override { return !m_PixelData.Empty() && m_Width > 0; }

		const auto& PixelData() const { return m_PixelData; }
		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }
		uint32_t DepthOrArraySize() const { return m_DepthOrArraySize; }
		ERHITextureDimension Dimension() const { return m_Dimension; }
		ERHIFormat Format() const { return m_Format; }

		//=============================================================================================
		// Returns the pixel color at the specified coordinates.
		Optional<Color4> TryGetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const;

		//=============================================================================================
		// Returns the pixel color at the specified coordinates,
		// Black if out of bounds or the texture is not stored on the CPU.
		Color4 GetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const { return TryGetPixel( a_X, a_Y, a_Z ).value_or( Color4::Black() ); }

		//=============================================================================================
		// Clears the pixel data stored on the CPU, but does not modify the RHI texture.
		void ClearPixelData() { m_PixelData.Clear(); }

	protected:

		//=============================================================================================
		Array<uint8_t> m_PixelData;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_DepthOrArraySize = 1; // For 3D textures or texture arrays
		ERHITextureDimension m_Dimension = ERHITextureDimension::Texture2D;
		ERHIFormat m_Format = ERHIFormat::RGBA8_UNORM;

	};

} // namespace Tridium