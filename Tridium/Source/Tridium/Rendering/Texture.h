#pragma once
#include "Tridium/Asset/Asset.h"

namespace Tridium {

	enum class EDataFormat
	{
		None = 0,
		R8,
		RG8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1u;
		uint32_t Height = 1u;
		uint32_t Depth = 0u;
		EDataFormat DataFormat = EDataFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Texture )
		ASSET_LOADER_TYPE( TextureLoader )
		virtual ~Texture() = default;

		static Texture* Create( const TextureSpecification& a_Specification );

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
}