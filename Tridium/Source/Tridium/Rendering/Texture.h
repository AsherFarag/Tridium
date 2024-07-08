#pragma once
#include "Tridium/Core/Asset.h"

namespace Tridium {

	using TextureHandle = AssetHandle;

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

	class Texture : public Asset<TextureHandle>
	{
		friend class TextureLoader;

	public:
		virtual ~Texture() = default;

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

	class TextureLoader
	{
	public:
		static Ref<Texture> Import( const std::string& path );
	};

	class TextureLibrary : public AssetLibrary<TextureLibrary, TextureHandle, Texture>
	{
	public:
		static inline Ref<Texture> GetTexture( const TextureHandle& textureHandle ) { return GetAsset( textureHandle ); }
		static inline bool GetTextureHandle( const std::string& path, TextureHandle& outTextureHandle ) { return GetHandle( path, outTextureHandle ); }
		static inline TextureHandle GetTextureHandle( const std::string& path ) { return GetHandle( path ); }
		static inline bool HasTextureHandle( const std::string& path ) { return HasHandle( path ); }
		static inline bool AddTexture( const std::string& path, const Ref<Texture>& texture ) { return AddAsset( path, texture ); }
		static inline bool RemoveTexture( const TextureHandle& textureHandle ) { return RemoveAsset( textureHandle ); }
	};

}