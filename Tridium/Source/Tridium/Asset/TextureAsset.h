#pragma once
#include "Asset.h"
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Core/Core.h>

namespace Tridium {

	struct TextureSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t DepthOrArraySize = 1; // For 3D textures or texture arrays
		ERHITextureDimension Dimension = ERHITextureDimension::Texture2D;
		ERHIFormat Format = ERHIFormat::RGBA8_UNORM;
	};

	//==============================================================================
	// Texture Asset
	//  A texture asset is a resource containing an array of pixel data, which can be uploaded to the RHI.
	//  Provides methods for creation, data management, and querying texture properties such as dimensions and format.
	//  A texture can be stored on both the CPU and GPU(via RHI) and is valid if it has either.
	class Texture : public IAsset
	{
	public:
		static SharedPtr<Texture> Create() { return MakeShared<EnableMakeShared<Texture>>(); }
		static SharedPtr<Texture> Create( Array<uint8_t>&& a_Data, TextureSpecification a_Spec );
		static SharedPtr<Texture> Create( Span<const uint8_t> a_Data, TextureSpecification a_Spec );
		static SharedPtr<Texture> Create( Array<uint8_t>&& a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
			ERHITextureDimension a_Dimension, ERHIFormat a_Format );
		static SharedPtr<Texture> Create( Span<const uint8_t> a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
			ERHITextureDimension a_Dimension, ERHIFormat a_Format );
		// Attempts to load a texture from the specified file path, returns a error message if it fails.
		static Expected<SharedPtr<Texture>, String> Load( const char* a_FilePath );

		static constexpr EAssetType StaticType() { return EAssetType::Texture; }
		EAssetType Type() const override { return StaticType(); }
		bool Valid() const override { return m_RHITexture || !m_PixelData.Empty(); }

		const auto& PixelData() const { return m_PixelData; }
		uint32_t Width() const { return m_Specification.Width; }
		uint32_t Height() const { return m_Specification.Height; }
		uint32_t DepthOrArraySize() const { return m_Specification.DepthOrArraySize; }
		ERHITextureDimension Dimension() const { return m_Specification.Dimension; }
		ERHIFormat Format() const { return m_Specification.Format; }
		TextureSpecification Specification() const { return m_Specification; }
		const RHITextureRef& RHITexture() const { return m_RHITexture; }

		// Returns the pixel color at the specified coordinates.
		Optional<Color> TryGetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const;
		// Returns the pixel color at the specified coordinates, Black if out of bounds or the texture is not stored on the CPU.
		Color GetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const { return TryGetPixel( a_X, a_Y, a_Z ).value_or( Color::Black() ); }

		void UpdateTexture( Array<uint8_t>&& a_Data, TextureSpecification a_Desc );
		void UpdateTexture( Span<const uint8_t> a_Data, TextureSpecification a_Desc ) { UpdateTexture( Array<uint8_t>( a_Data ), a_Desc ); }

		// Clears the pixel data stored on the CPU, but does not modify the RHI texture.
		void ClearPixelData() { m_PixelData.Clear(); }

	private:
		Array<uint8_t> m_PixelData;
		TextureSpecification m_Specification;
		RHITextureRef m_RHITexture;
	};

} // namespace Tridium