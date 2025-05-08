#include "tripch.h"
#include "RenderingContext.h"
#include "RenderingAPI.h"
#include <Platform/OpenGL/OpenGLContext.h>

namespace Tridium {
    UniquePtr<RenderingContext> RenderingContext::Create( GLFWwindow* window )
    {
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeUnique<OpenGLContext>( window );
		}

		ASSERT( false, "Unknown RendererAPI!" );
		return nullptr;
    }
}
