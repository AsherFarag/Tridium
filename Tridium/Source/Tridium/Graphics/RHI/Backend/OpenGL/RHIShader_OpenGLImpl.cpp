#include "tripch.h"
#include "RHI_OpenGLImpl.h"
namespace Tridium::OpenGL {

	RHIShaderModule_OpenGLImpl::RHIShaderModule_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIShaderModule( a_Device, a_Desc )
	{


		// Create the shader and compile the GLSL source
		m_ShaderID = OpenGL2::CreateShader( Translate( a_Desc.Type ) );
		const GLchar* glslSource = ReinterpretCast<const GLchar*>( a_Desc.Bytecode.data() );
		const GLsizei glslLength = Cast<GLsizei>( a_Desc.Bytecode.size_bytes() );
		OpenGL2::ShaderSource( m_ShaderID, 1, &glslSource, &glslLength );
		OpenGL2::CompileShader( m_ShaderID );

	#if RHI_DEBUG_ENABLED
		std::cout << "RHIShaderModule_OpenGLImpl::Commit: Compiling shader '\n" << StringView( glslSource, glslLength ) << "'" << std::endl;

		// Check for compilation errors
		GLint success = 0;
		OpenGL2::GetShaderiv( m_ShaderID, GL_COMPILE_STATUS, &success );
		if ( success == GL_FALSE )
		{
			GLint length = 0;
			OpenGL2::GetShaderiv( m_ShaderID, GL_INFO_LOG_LENGTH, &length );
			Array<GLchar> infoLog( length );
			OpenGL2::GetShaderInfoLog( m_ShaderID, length, &length, infoLog.Data() );
			LOG( LogCategory::OpenGL, Error, "RHIShaderModule_OpenGLImpl::Commit: Failed to compile shader '{0}' - Error: {1}", a_Desc.Name, infoLog.Data() );
			Release();
		}

		if ( RHI::IsDebug() && !a_Desc.Name.empty() )
		{
			OpenGL4::ObjectLabel( GL_SHADER, m_ShaderID, a_Desc.Name.size(), Cast<const GLchar*>( a_Desc.Name.data() ) );
		}
	#endif
	}

	bool RHIShaderModule_OpenGLImpl::Release()
	{
		if ( Valid() )
		{
			OpenGL2::DeleteShader( m_ShaderID );
		}

		return true;
	}

} // namespace Tridium