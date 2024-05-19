#include "tripch.h"
#include "OpenGLShader.h"

namespace Tridium {

	OpenGLShader::OpenGLShader( const std::string& vertexSource, const std::string& fragmentSource )
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSource.c_str();
		glShaderSource( vertexShader, 1, &source, 0 );

		// Compile the vertex shader
		glCompileShader( vertexShader );

		GLint isCompiled = 0;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &isCompiled );
		if ( isCompiled == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &maxLength );

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog( maxLength );
			glGetShaderInfoLog( vertexShader, maxLength, &maxLength, &infoLog[ 0 ] );

			// We don't need the shader anymore.
			glDeleteShader( vertexShader );

			TE_CORE_ERROR( "Vertex shader complilation failure!" );
			TE_CORE_ERROR( "{0}", infoLog.data() );

			// In this simple program, we'll just leave
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSource.c_str();
		glShaderSource( fragmentShader, 1, &source, 0 );

		// Compile the fragment shader
		glCompileShader( fragmentShader );

		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &isCompiled );
		if ( isCompiled == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &maxLength );

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog( maxLength );
			glGetShaderInfoLog( fragmentShader, maxLength, &maxLength, &infoLog[ 0 ] );

			// We don't need the shader anymore.
			glDeleteShader( fragmentShader );
			// Either of them. Don't leak shaders.
			glDeleteShader( vertexShader );

			TE_CORE_ERROR( "Fragment shader complilation failure!" );
			TE_CORE_ERROR( "{0}", infoLog.data() );

			// In this simple program, we'll just leave
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader( m_RendererID, vertexShader );
		glAttachShader( m_RendererID, fragmentShader );

		// Link our program
		glLinkProgram( m_RendererID );

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv( m_RendererID, GL_LINK_STATUS, (int*)&isLinked );
		if ( isLinked == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetProgramiv( m_RendererID, GL_INFO_LOG_LENGTH, &maxLength );

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog( maxLength );
			glGetProgramInfoLog( m_RendererID, maxLength, &maxLength, &infoLog[ 0 ] );

			// We don't need the program anymore.
			glDeleteProgram( m_RendererID );
			// Don't leak shaders either.
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );

			TE_CORE_ERROR( "Shader Link Failure!" );
			TE_CORE_ERROR( "{0}", infoLog.data() );

			// In this simple program, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader( m_RendererID, vertexShader );
		glDetachShader( m_RendererID, fragmentShader );
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram( m_RendererID );
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram( m_RendererID );
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram( 0 );
	}

	bool OpenGLShader::SetInt( const char* name, const int val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1i( i, val );
		return true;
	}

	bool OpenGLShader::SetInt2( const char* name, const iVector2& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2i( i, val.x, val.y );
		return true;
	}

	bool OpenGLShader::SetInt3( const char* name, const iVector3& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3i( i, val.x, val.y, val.z );
		return true;
	}

	bool OpenGLShader::SetInt4( const char* name, const iVector4& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4i( i, val.x, val.y, val.z, val.w );
		return true;
	}

	bool OpenGLShader::SetFloat( const char* name, const float val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1f( i, val );
		return true;
	}

	bool OpenGLShader::SetFloat2( const char* name, const Vector2& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2f( i, val.x, val.y );
		return true;
	}

	bool OpenGLShader::SetFloat3( const char* name, const Vector3& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3f( i, val.x, val.y, val.z );
		return true;
	}

	bool OpenGLShader::SetFloat4( const char* name, const Vector4& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4f( i, val.x, val.y, val.z, val.w );
		return true;
	}

	bool OpenGLShader::SetMatrix2( const char* name, const Matrix2& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix2fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true; 
	}

	bool OpenGLShader::SetMatrix3( const char* name, const Matrix3& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix3fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true;
	}

	bool OpenGLShader::SetMatrix4( const char* name, const Matrix4& val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix4fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true;
	}

	bool OpenGLShader::SetInt( const char* name, const uint32_t count, const int* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1iv( i, count, val );
		return true;
	}

	bool OpenGLShader::SetInt2( const char* name, const uint32_t count, const iVector2* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShader::SetInt3( const char* name, const uint32_t count, const iVector3* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShader::SetInt4( const char* name, const uint32_t count, const iVector4* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShader::SetFloat( const char* name, const uint32_t count, const float* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShader::SetFloat2( const char* name, const uint32_t count, const Vector2* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShader::SetFloat3( const char* name, const uint32_t count, const Vector3* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShader::SetFloat4( const char* name, const uint32_t count, const Vector4* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShader::SetMatrix2( const char* name, const uint32_t count, const Matrix2* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix2fv( i, count, GL_FALSE, (float*)val );
		return true;
	}

	bool OpenGLShader::SetMatrix3( const char* name, const uint32_t count, const Matrix3* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix3fv( i, count, GL_FALSE, (float*)val );
		return true;
	}

	bool OpenGLShader::SetMatrix4( const char* name, const uint32_t count, const Matrix4* val )
	{
		TE_CORE_ASSERT( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			TE_CORE_ERROR( "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix4fv( i, count, GL_FALSE, (float*)val );
		return true;
	}
}