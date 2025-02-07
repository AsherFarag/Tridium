#include "tripch.h"
#include "Framebuffer.h"

#include <Tridium/Graphics/Rendering/RenderingAPI.h>

// Platforms
#include <Platform/OpenGL/OpenGLFramebuffer.h>

namespace Tridium {

    SharedPtr<Framebuffer> Framebuffer::Create( const FramebufferSpecification& spec )
    {
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLFramebuffer>( spec );
			break;
		default:
			return nullptr;
			break;
		}
    }

	SharedPtr<RenderBuffer> RenderBuffer::Create( uint32_t a_Width, uint32_t a_Height, EFramebufferTextureFormat a_Format )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLRenderBuffer>( a_Width, a_Height, a_Format );
			break;
		default:
			return nullptr;
			break;
		}
	}

}