#include "tripch.h"
#include "Tridium/Rendering/VertexArray.h"

#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>

namespace Tridium {

	Ref<VertexArray> VertexArray::Create()
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeRef<OpenGLVertexArray>();
			break;
		default:
			return nullptr;
			break;
		}

	}
}