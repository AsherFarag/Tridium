#include "tripch.h"
#include "Framebuffer.h"

#include "Tridium/Rendering/RenderingAPI.h"

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

}