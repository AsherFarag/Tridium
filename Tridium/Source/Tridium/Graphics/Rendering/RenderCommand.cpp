#include "tripch.h"
#include "RenderCommand.h"

#include <Platform/OpenGL/OpenGLRenderingAPI.h>

namespace Tridium {
	UniquePtr<RendererAPI> RenderCommand::s_RendererAPI = MakeUnique<OpenGLRenderingAPI>();
}