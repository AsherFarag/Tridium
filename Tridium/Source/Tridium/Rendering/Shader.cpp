#include "tripch.h"
#include "Shader.h"

#include <Tridium/Core/Application.h>
#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	SharedPtr<Shader> Shader::Load( const std::string& path )
	{
		SharedPtr<Shader> shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeShared<OpenGLShader>( path );
			break;
		default:
			return nullptr;
			break;
		}

		return shader;
	}

	SharedPtr<Shader> Shader::Create( const std::string& name, const std::string& vertex, const std::string& frag )
	{
		SharedPtr<Shader> shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeShared<OpenGLShader>( name, vertex, frag );
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