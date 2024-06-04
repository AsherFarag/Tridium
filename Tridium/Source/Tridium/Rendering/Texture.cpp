#include "tripch.h"
#include "Texture.h"

#include <Tridium/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>

namespace Tridium {

	Ref<Texture2D> Texture2D::Create( const TextureSpecification& specification )
	{
		switch ( Renderer::GetAPI() )
		{
		case RendererAPI::API::None:    TE_CORE_ASSERT( false, "RendererAPI::None is currently not supported!" ); return nullptr;
		case RendererAPI::API::OpenGL:  return MakeRef<OpenGLTexture2D>( specification );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create( const std::string& path )
	{
		auto foundTexture = Texture2DLibrary::GetTexture( path );
		if ( foundTexture )
			return foundTexture;

		Ref<Texture2D> texture = nullptr;

		switch ( Renderer::GetAPI() )
		{
		case RendererAPI::API::None:    TE_CORE_ASSERT( false, "RendererAPI::None is currently not supported!" ); return nullptr;
		case RendererAPI::API::OpenGL:  texture = MakeRef<OpenGLTexture2D>( path );
		}

		if ( texture )
			Texture2DLibrary::Add( texture, path );
		else
			TE_CORE_ASSERT( false, "Unknown RendererAPI!" );

		return texture;
	}

	Texture2DLibrary* Texture2DLibrary::Get()
	{
		static Texture2DLibrary* s_Instance = new Texture2DLibrary();
		return s_Instance;
	}

	Ref<Texture2D> Texture2DLibrary::GetTexture( const std::string& a_Path )
	{
		auto it = Get()->m_Library.find( a_Path );
		return it != Get()->m_Library.end() ? it->second : nullptr;
	}

	bool Texture2DLibrary::Has( const std::string& a_Path )
	{
		return Get()->m_Library.find( a_Path ) != Get()->m_Library.end();
	}

	void Texture2DLibrary::Add( const Ref<Texture2D>& a_Texture, const std::string& a_Path )
	{
		Get()->m_Library.emplace( a_Path, a_Texture );
	}

}