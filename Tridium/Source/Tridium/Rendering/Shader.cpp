#include "tripch.h"
#include "Shader.h"

#include <Tridium/Core/Application.h>
#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	AssetRef<Shader> Shader::Load( const std::string& path )
	{
		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLShader( path );
			break;
		default:
			return nullptr;
			break;
		}
	}

	AssetRef<Shader> Shader::Create( const std::string& name, const std::string& vertex, const std::string& frag )
	{
		Shader* shader = nullptr;

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = new OpenGLShader( name, vertex, frag );
			break;
		default:
			return nullptr;
			break;
		}
		
		shader->m_Path = name;
		shader->m_Handle = AssetHandle::Create();
		return { shader };
	} 

}