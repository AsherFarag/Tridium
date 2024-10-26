#pragma once
#include "Tridium/Asset/Asset.h"

namespace Tridium {

	enum class ETextureFormat
	{
		None = 0,
		R8,
		RG8,
		RGB8,
		RGBA8,
		RG16F,
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F,
		SRGB,
		SRGBA,
	};

	enum class ETextureWrap : uint8_t
	{
		None = 0,
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	enum class ETextureFilter : uint8_t
	{
		None = 0,
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear,
	};

	struct TextureSpecification
	{
		uint32_t Width = 1u;
		uint32_t Height = 1u;
		uint32_t Depth = 0u;
		ETextureFormat TextureFormat = ETextureFormat::RGBA8;
		bool GenerateMips = true;
		ETextureFilter MinFilter = ETextureFilter::Linear;
		ETextureFilter MagFilter = ETextureFilter::Linear;
		ETextureWrap WrapS = ETextureWrap::Repeat;
		ETextureWrap WrapT = ETextureWrap::Repeat;
		ETextureWrap WrapR = ETextureWrap::Repeat;
	};

	class Texture : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Texture )
		virtual ~Texture() = default;

		static Texture* Create( const TextureSpecification& a_Specification );
		static Texture* Create( const TextureSpecification& a_Specification, void* a_TextureData );

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual void SetMinFilter( ETextureFilter a_Filter ) = 0;
		virtual void SetMagFilter( ETextureFilter a_Filter ) = 0;
		virtual void SetWrapS( ETextureWrap a_Wrap ) = 0;
		virtual void SetWrapT( ETextureWrap a_Wrap ) = 0;
		virtual void SetWrapR( ETextureWrap a_Wrap ) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetRendererID() const = 0;
		uint32_t GetMaxMipLevel() const { return (uint32_t)std::floor( std::log2( std::max( GetWidth(), GetHeight() ) ) ); }

		virtual void* GetData() const = 0;
		virtual void SetData( void* data, uint32_t size ) = 0;

		virtual void Bind( uint32_t slot = 0 ) const = 0;
		virtual void Unbind( uint32_t slot = 0 ) const = 0;

		virtual bool operator==( const Texture& other ) const = 0;
	};

	class CubeMap : public Asset
	{
	public:
		ASSET_CLASS_TYPE( CubeMap )
		virtual ~CubeMap() = default;

		static CubeMap* Create( const TextureSpecification& a_Specification ) { return Create( a_Specification, nullptr ); }
		static CubeMap* Create( const TextureSpecification& a_Specification, const SharedPtr<Texture>& a_Texture );
		static CubeMap* Create( const TextureSpecification& a_Specification, const std::array<float*, 6>& a_CubeMapData );

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual void SetMinFilter( ETextureFilter a_Filter ) = 0;
		virtual void SetMagFilter( ETextureFilter a_Filter ) = 0;
		virtual void SetWrapS( ETextureWrap a_Wrap ) = 0;
		virtual void SetWrapT( ETextureWrap a_Wrap ) = 0;
		virtual void SetWrapR( ETextureWrap a_Wrap ) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual void GenerateMipMaps() = 0;
		uint32_t GetMaxMipLevels() const { return (uint32_t)std::floor( std::log2( std::max( GetWidth(), GetHeight() ) ) ); }

		virtual void SetData( SharedPtr<Texture> a_Texture, uint32_t a_Size ) = 0;
		virtual void SetData( const std::array<float*, 6>& a_CubeMapData, uint32_t a_Size ) = 0;
		virtual void SetMipLevel( uint32_t a_MipLevel ) = 0;

		virtual void Bind( uint32_t a_Slot = 0 ) const = 0;
		virtual void Unbind( uint32_t a_Slot = 0 ) const = 0;

		virtual bool operator==( const CubeMap& a_Other ) const = 0;
	};

	class TextureFactory
	{
	public:
		static void Init();

		static AssetHandle GetWhiteTexture();
		static AssetHandle GetBlackTexture();
		static AssetHandle GetNormalTexture();
	};
}