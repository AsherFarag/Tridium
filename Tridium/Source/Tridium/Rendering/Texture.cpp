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
		switch ( Renderer::GetAPI() )
		{
		case RendererAPI::API::None:    TE_CORE_ASSERT( false, "RendererAPI::None is currently not supported!" ); return nullptr;
		case RendererAPI::API::OpenGL:  return MakeRef<OpenGLTexture2D>( path );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

}