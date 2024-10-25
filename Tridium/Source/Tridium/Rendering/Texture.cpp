#include "tripch.h"
#include "Texture.h"

#include <Tridium/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>

namespace Tridium {

	Texture* Texture::Create( const TextureSpecification& a_Specification )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL: 
			return new OpenGLTexture( a_Specification );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	Texture* Texture::Create( const TextureSpecification& a_Specification, void* a_TextureData )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLTexture( a_Specification, a_TextureData );
		}

		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	CubeMap* CubeMap::Create( const TextureSpecification& a_Specification, const SharedPtr<Texture>& a_Texture )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLCubeMap( a_Specification, a_Texture );
		}
		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}

	CubeMap* CubeMap::Create( const TextureSpecification& a_Specification, const std::array<float*, 6>& a_CubeMapData )
	{
		switch ( Renderer::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLCubeMap( a_Specification, a_CubeMapData );
		}
		TE_CORE_ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
	}
}