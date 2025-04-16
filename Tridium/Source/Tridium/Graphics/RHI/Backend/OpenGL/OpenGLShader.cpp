#include "tripch.h"
#include "OpenGLShader.h"
#include "spirv_glsl.hpp"

namespace Tridium {

	bool OpenGLShaderModule::Commit( const RHIShaderModuleDescriptor& a_Desc )
	{
		m_Desc = a_Desc;

		// Create GLSL from the SPIR-V bytecode using SPIRV-Cross
		spirv_cross::CompilerGLSL glslCompiler( reinterpret_cast<const uint32_t*>( a_Desc.Bytecode.data() ), a_Desc.Bytecode.size_bytes() / sizeof( uint32_t ) );
		spirv_cross::CompilerGLSL::Options options;
		options.version = 450;
		options.es = false;
		glslCompiler.set_common_options( options );
		glslCompiler.build_combined_image_samplers();

		// Textures and samplers are combined in GLSL, so we need to keep track of them and set the correct names
		auto combinedSamplers = glslCompiler.get_combined_image_samplers();
		UnorderedSet<spirv_cross::VariableID> seenImageIDs;
		seenImageIDs.reserve( combinedSamplers.size() );
		for ( auto& sampler : combinedSamplers )
		{
			if ( seenImageIDs.contains( sampler.image_id ) )
			{
				ASSERT_LOG( false, "Textures used with multiple samplers are not supported!" );
				continue;
			}

			seenImageIDs.insert( sampler.image_id );
			const String& texName = glslCompiler.get_name( sampler.image_id );
			// Set the name of the combined sampler to the texture name.
			// This is helpful for setting Texture Shader Inputs via the OpenGLCommandList.
			glslCompiler.set_name( sampler.combined_id, texName );
		}

		String glsl = glslCompiler.compile();

		// Create the shader and compile the GLSL source
		m_ShaderID = OpenGL2::CreateShader( ToOpenGL::GetShaderType( a_Desc.Type ) );
		const char* source = glsl.c_str();
		OpenGL2::ShaderSource( m_ShaderID, 1, &source, nullptr );
		OpenGL2::CompileShader( m_ShaderID );

	#if RHI_DEBUG_ENABLED
		std::cout << "OpenGLShaderModule::Commit: Compiling shader '\n" << glsl << "'" << std::endl;

		// Check for compilation errors
		GLint success = 0;
		OpenGL2::GetShaderiv( m_ShaderID, GL_COMPILE_STATUS, &success );
		if ( success == GL_FALSE )
		{
			GLint length = 0;
			OpenGL2::GetShaderiv( m_ShaderID, GL_INFO_LOG_LENGTH, &length );
			Array<GLchar> infoLog( length );
			OpenGL2::GetShaderInfoLog( m_ShaderID, length, &length, infoLog.Data() );
			LOG( LogCategory::OpenGL, Error, "OpenGLShaderModule::Commit: Failed to compile shader '{0}' - Error: {1}", a_Desc.Name, infoLog.Data() );
			return false;
		}

		if ( RHIQuery::IsDebug() && !a_Desc.Name.empty() )
		{
			OpenGL4::ObjectLabel( GL_SHADER, m_ShaderID, a_Desc.Name.size(), static_cast<const GLchar*>( a_Desc.Name.data() ) );
		}
	#endif

		return IsValid();
	}

	bool OpenGLShaderModule::Release()
	{
		if ( IsValid() )
		{
			OpenGL2::DeleteShader( m_ShaderID );
		}

		return true;
	}

} // namespace Tridium