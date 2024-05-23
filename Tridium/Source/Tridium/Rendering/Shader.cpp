#include "tripch.h"
#include "Shader.h"

#include <Tridium/Rendering/RenderingAPI.h>
#include <Platform/OpenGL/OpenGLShader.h>

namespace Tridium {

	Ref<Shader> Shader::Create( const std::string& vertex, const std::string& frag, const std::string& name )
	{
		Ref<Shader> shader = nullptr;
		TE_CORE_ASSERT( ShaderLibrary::Get()->m_Shaders.find( name ) == ShaderLibrary::Get()->m_Shaders.end(), "Shader already exists with that name!" );

		switch ( RendererAPI::GetAPI() )
		{
		case RendererAPI::API::OpenGL:
			shader = MakeRef<OpenGLShader>( vertex, frag );
			break;
		default:
			return nullptr;
			break;
		}

		ShaderLibrary::Get()->m_Shaders.insert( { name, shader } );
		return shader;
	}

	ShaderLibrary* ShaderLibrary::Get()
	{
		static ShaderLibrary* instance = new ShaderLibrary();

		return instance;
	}

	Ref<Shader> ShaderLibrary::GetShader( const std::string& name )
	{
		auto it = Get()->m_Shaders.find( name );
		return it != Get()->m_Shaders.end() ? it->second : nullptr;
	}

}