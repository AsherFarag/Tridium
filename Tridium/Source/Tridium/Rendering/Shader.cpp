#include "tripch.h"
#include "Shader.h"

#include <Tridium/Core/Application.h>
#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	Shader* Shader::Create( const std::string& a_Vertex, const std::string& a_Frag )
	{
		Shader* shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = new OpenGLShader( a_Vertex, a_Frag );
			break;
		default:
			return nullptr;
			break;
		}
		
		shader->m_Handle = AssetHandle::Create();
		return shader;
	} 

	Shader* Shader::Create()
	{
		switch ( RendererAPI::GetAPI() )
		{
			using enum RendererAPI::API;
		case OpenGL:
			return new OpenGLShader();
		default:
			return nullptr;
		}
	}

}