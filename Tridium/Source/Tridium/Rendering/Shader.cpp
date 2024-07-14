#include "tripch.h"
#include "Shader.h"

#include <Tridium/Core/Application.h>
#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	Ref<Shader> Shader::Load( const std::string& path )
	{
		Ref<Shader> shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeRef<OpenGLShader>( path );
			break;
		default:
			return nullptr;
			break;
		}

		return shader;
	}

	Ref<Shader> Shader::Create( const std::string& name, const std::string& vertex, const std::string& frag )
	{
		Ref<Shader> shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeRef<OpenGLShader>( name, vertex, frag );
			break;
		default:
			return nullptr;
			break;
		}
		
		shader->m_Path = name;
		shader->m_GUID = GUID::Create();
		return shader;
	} 

}