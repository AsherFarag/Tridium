#pragma once
#include "Asset.h"
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Core/Core.h>

namespace Tridium::T {

	//==============================================================================
	// Texture Asset
	//  A texture asset is a resource containing an array of pixel data, which can be uploaded to the RHI.
	//  Provides methods for creation, data management, and querying texture properties such as dimensions and format.
	//  A texture can be stored on both the CPU and GPU(via RHI) and is valid if it has either.
	class Texture : public IAsset
	{
	public:
		static SharedPtr<Texture> Create() { return MakeShared<EnableMakeShared<Texture>>(); }
		static SharedPtr<Texture> Create( Array<uint8_t>&& a_Data, const RHITextureDesc& a_RHIDesc, bool a_CommitToRHI = true );
		static SharedPtr<Texture> Create( Span<const uint8_t> a_Data, const RHITextureDesc& a_RHIDesc, bool a_CommitToRHI = true );
		static SharedPtr<Texture> Create( Array<uint8_t>&& a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
			ERHITextureDimension a_Dimension, ERHIFormat a_Format, StringView a_Name, bool a_CommitToRHI = true );
		static SharedPtr<Texture> Create( Span<const uint8_t> a_Data, uint32_t a_Width, uint32_t a_Height, uint32_t a_DepthOrArraySize,
			ERHITextureDimension a_Dimension, ERHIFormat a_Format, StringView a_Name, bool a_CommitToRHI = true );

		static constexpr EAssetType StaticType() { return EAssetType::Texture; }
		EAssetType Type() const override { return StaticType(); }
		bool Valid() const override { return m_RHITexture || !m_PixelData.Empty(); }

		const auto& PixelData() const { return m_PixelData; }
		uint32_t Width() const { return RHIDesc().Width; }
		uint32_t Height() const { return RHIDesc().Height; }
		uint32_t Depth() const { return RHIDesc().Depth; }
		uint32_t ArraySize() const { return RHIDesc().ArraySize; }
		ERHITextureDimension Dimension() const { return RHIDesc().Dimension; }
		ERHIFormat Format() const { return RHIDesc().Format; }
		const RHITextureDesc& RHIDesc() const { return m_RHITexture ? m_RHITexture->Desc() : m_RHIDesc; }
		const RHITextureRef& IRHITexture() const { return m_RHITexture; }

		// Returns the pixel color at the specified coordinates.
		Optional<Color> TryGetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const;
		// Returns the pixel color at the specified coordinates, Black if out of bounds or the texture is not stored on the CPU.
		Color GetPixel( uint32_t a_X, uint32_t a_Y = 0, uint32_t a_Z = 0 ) const { return TryGetPixel( a_X, a_Y, a_Z ).value_or( Color::Black() ); }

		void UpdateTexture( Array<uint8_t>&& a_Data, RHITextureDesc a_Desc, bool a_CommitToRHI );
		void UpdateTexture( Span<const uint8_t> a_Data, RHITextureDesc a_Desc, bool a_CommitToRHI ) { UpdateTexture( Array<uint8_t>( a_Data ), a_Desc, a_CommitToRHI ); }

		// Clears the pixel data stored on the CPU, but does not modify the RHI texture.
		void ClearPixelData() { m_PixelData.Clear(); }

	private:
		Array<uint8_t> m_PixelData;
		RHITextureDesc m_RHIDesc;
		RHITextureRef m_RHITexture;
	};

} // namespace Tridium::T