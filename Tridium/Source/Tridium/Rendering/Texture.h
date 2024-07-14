#pragma once
#include "Tridium/Core/Asset.h"

namespace Tridium {

	enum class EImageFormat
	{
		None = 0,
		R,
		RG,
		RGB,
		RGBA
	};

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
		EImageFormat ImageFormat = EImageFormat::RGBA;
		EDataFormat DataFormat = EDataFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Texture )
		virtual ~Texture() = default;

		static Ref<Texture> Load( const std::string& path );
		static Ref<Texture> Create( const TextureSpecification& specification );

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData( void* data, uint32_t size ) = 0;

		virtual void Bind( uint32_t slot = 0 ) const = 0;
		virtual void Unbind( uint32_t slot = 0 ) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==( const Texture& other ) const = 0;

	protected:
		virtual void SetIsLoaded( bool loaded ) = 0;
	};
}