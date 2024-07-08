#include "tripch.h"
#include "Shader.h"

#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {
	Ref<Shader> Shader::Create( const std::string& filePath )
	{
		Ref<Shader> shader = nullptr;
		//TE_CORE_ASSERT( ShaderLibrary::Get()->m_Shaders.find( name ) == ShaderLibrary::Get()->m_Shaders.end(), "Shader already exists with that name!" );

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeRef<OpenGLShader>( filePath );
			break;
		default:
			return nullptr;
			break;
		}

		//ShaderLibrary::Get()->m_Shaders.insert( { name, shader } );
		return shader;
	}

	Ref<Shader> Shader::Create( const std::string& name, const std::string& vertex, const std::string& frag )
	{
		Ref<Shader> shader = nullptr;
		TE_CORE_ASSERT( !ShaderLibrary::HasShaderHandle(name), "Shader already exists with that name!");

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeRef<OpenGLShader>( name, vertex, frag );
			break;
		default:
			return nullptr;
			break;
		}

		shader->_SetHandle( ShaderHandle::Create() );
		ShaderLibrary::AddShader( name, shader );
		return shader;
	}

}