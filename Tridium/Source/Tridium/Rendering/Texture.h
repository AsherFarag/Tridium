#pragma once

#include "Tridium/Core/Core.h"

#include <string>

namespace Tridium {

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void SetData( void* data, uint32_t size ) = 0;

		virtual void Bind( uint32_t slot = 0 ) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==( const Texture& other ) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create( const TextureSpecification& specification );
		static Ref<Texture2D> Create( const std::string& path );
	};

	class Texture2DLibrary
	{
		friend Texture2D;
	public:
		static Texture2DLibrary* Get();
		static Ref<Texture2D> GetTexture( const std::string& a_Path );
		static bool Has( const std::string& a_Path );

	private:
		static void Add( const Ref<Texture2D>& a_Texture, const std::string& a_Path );

	private:
		std::unordered_map<std::string, Ref<Texture2D>> m_Library;
	};

}