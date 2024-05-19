#include "tripch.h"
#include "Shader.h"

#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	Ref<Shader> Shader::Create( const std::string& vertex, const std::string& frag )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return MakeRef<OpenGLShader>( vertex, frag );
			break;
		default:
			return nullptr;
			break;
		}
	}

}