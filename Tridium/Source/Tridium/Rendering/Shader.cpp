#include "tripch.h"
#include "Shader.h"

#include <Tridium/Core/Application.h>
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

		shader->_SetHandle( ShaderHandle::Create() );
		ShaderLibrary::AddShader( filePath, shader );
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
		
		shader->_SetPath( name );
		shader->_SetHandle( ShaderHandle::Create() );
		ShaderLibrary::AddShader( name, shader );
		return shader;
	}

	void ShaderLibrary::RecompileAll()
	{
		for ( auto it = Get().m_Library.begin(); it != Get().m_Library.end(); ++it )
			it->second->Recompile();
	}

	void ShaderLibrary::Init()
	{
		auto defaultShader = Shader::Create( ( Application::GetAssetDirectory() / "Engine" / "Shaders" / "Default.glsl" ).string() );
		m_Default = defaultShader->GetGUID();
		AddAsset( defaultShader->GetPath(), defaultShader );

		auto spriteShader = Shader::Create( ( Application::GetAssetDirectory() / "Engine" / "Shaders" / "Sprite.glsl" ).string() );
		m_Sprite = spriteShader->GetGUID();
		AddAsset( spriteShader->GetPath(), spriteShader );
	}

}