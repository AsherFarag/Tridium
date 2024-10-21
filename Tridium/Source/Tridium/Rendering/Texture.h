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
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F,
		SRGB,
		SRGBA,
	};

	struct TextureSpecification
	{
		uint32_t Width = 1u;
		uint32_t Height = 1u;
		uint32_t Depth = 0u;
		ETextureFormat TextureFormat = ETextureFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Texture )
		virtual ~Texture() = default;

		static Texture* Create( const TextureSpecification& a_Specification );
		static Texture* Create( const TextureSpecification& a_Specification, void* a_TextureData );

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetRendererID() const = 0;

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

		static CubeMap* Create( const TextureSpecification& a_Specification );
		static CubeMap* Create( const TextureSpecification& a_Specification, void** a_CubeMapData );

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData( void** data, uint32_t size ) = 0;

		virtual void Bind( uint32_t slot = 0 ) const = 0;
		virtual void Unbind( uint32_t slot = 0 ) const = 0;

		virtual bool operator==( const CubeMap& other ) const = 0;
	};
}