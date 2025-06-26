#include "tripch.h"
#include "Tridium/Graphics/oldRendering/VertexArray.h"

#include <Tridium/Graphics/oldRendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>

namespace Tridium {

	SharedPtr<VertexArray> VertexArray::Create()
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeShared<OpenGLVertexArray>();
			break;
		default:
			return nullptr;
			break;
		}

	}
}