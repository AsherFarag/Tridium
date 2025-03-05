#include "tripch.h"
#include "OpenGLShader.h"

namespace Tridium {

	bool OpenGLShaderModule::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHIShaderModuleDescriptor>( a_Params );
		if ( desc == nullptr )
		{
			return false;
		}

		m_ShaderID = OpenGL2::CreateShader( ToOpenGL::GetShaderType( desc->Type ) );
		OpenGL4::ShaderBinary( 1, &m_ShaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, desc->Bytecode.data(), desc->Bytecode.size_bytes() );
		OpenGL4::SpecializeShader( m_ShaderID, desc->EntryPoint.data(), 0, nullptr, nullptr );

	#if RHI_DEBUG_ENABLED
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			OpenGL4::ObjectLabel( GL_SHADER, m_ShaderID, desc->Name.size(), static_cast<const GLchar*>( desc->Name.data() ) );
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