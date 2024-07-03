#include "tripch.h"
#include "OpenGLShader.h"
#include <fstream>
#include <Tridium/Utils/StringHelpers.h>

namespace Tridium {

	static bool IsValidShaderType( const std::string& a_Type )
	{
		std::string type = Util::ToLowerCase( a_Type );

		if ( type == "vertex" )
			return true;
		if ( type == "frag" )
			return true;

		return false;
	}

	static GLenum ShaderTypeFromString( const std::string& a_Type )
	{
		std::string type = Util::ToLowerCase( a_Type );

		if ( type == "vertex" )
			return GL_VERTEX_SHADER;
		if ( type == "frag" )
			return GL_VERTEX_SHADER;

		TE_CORE_ASSERT( false, "Invalid shader type!" );
		return 0;
	}

	OpenGLShader::OpenGLShader( const std::string& filePath )
	{
		std::string source = ReadFile( filePath );
		auto shaderSources = PreProcess( source );
		Compile( shaderSources );
	}

	OpenGLShader::OpenGLShader( const std::string& name, const std::string& vertexSource, const std::string& fragmentSource )
		: m_Name( name )
	{
		ShaderSources sources;
		sources[ GL_VERTEX_SHADER ] = vertexSource;
		sources[ GL_FRAGMENT_SHADER ] = fragmentSource;
		Compile( sources );
	}

	std::string OpenGLShader::ReadFile( const std::string& filePath )
	{
		std::string result;
		std::ifstream in( filePath, std::ios::in | std::ios::binary );
		if ( !in )
		{
			TE_CORE_ERROR( "Could not open file '{0}'", filePath );
			return "";
		}

		in.seekg( 0, std::ios::end );
		result.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &result[ 0 ], result.size() );

		in.close();

		return result;
	}

	ShaderSources OpenGLShader::PreProcess( const std::string& source )
	{
		TODO( "Unsafe preprocess if the source isn't perfect!" );

		ShaderSources shaderSources;

		const char* typeToken = "#type";
		const size_t typeTokenLength = strlen( typeToken );
		size_t pos = source.find( typeToken, 0 );

		while ( pos != std::string::npos )
		{
			size_t eol = source.find_first_of( "\r\n", pos );
			TE_CORE_ASSERT( eol != std::string::npos );

			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr( begin, eol - begin );
			TE_CORE_ASSERT( IsValidShaderType(type), "Invalid shader type specification!");

			size_t nextLinePos = source.find_first_not_of( "\r\n", eol );
			pos = source.find( typeToken, nextLinePos );
			size_t endOfSource = pos - ( nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos );
			shaderSources[ ShaderTypeFromString( type ) ] = source.substr( nextLinePos, endOfSource );
		}

		return shaderSources;
	}

	void OpenGLShader::Compile( const ShaderSources& shaderSources )
	{
		GLuint program = glCreateProgram();
		TE_CORE_ASSERT( shaderSources.size() <= 2, "Only 2 shaders are supported currently!" );
		std::array<GLenum, 2> glShaderIDs;

		uint32_t glShaderIDIndex = 0;
		for ( auto&& [key, value] : shaderSources )
		{
			GLenum type = key;
			const std::string& source = value;

			GLuint shader = glCreateShader( type );

			const GLchar* sourceCstr = source.c_str();
			glShaderSource( shader, 1, &sourceCstr, 0 );

			glCompileShader( shader );

			GLint isCompiled = 0;
			glGetShaderiv( shader, GL_COMPILE_STATUS, &isCompiled );
			if ( isCompiled == GL_FALSE )
			{
				GLint maxLength = 0;
				glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

				std::vector<GLchar> infoLog;
				infoLog.resize( maxLength );
				glGetShaderInfoLog( shader, maxLength, &maxLength, &infoLog[ 0 ] );

				glDeleteShader( shader );

				TE_CORE_ERROR( "Shader complilation failure! Type: '{0}'", key );
				TE_CORE_ERROR( "{0}", infoLog.data() );
				TE_CORE_ERROR( "- End of Shader Error -" );

				break;
			}

			glAttachShader( program, shader );
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram( program );

		GLint isLinked = 0;
		glGetProgramiv( program, GL_LINK_STATUS, (int*)&isLinked );
		if ( isLinked == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

			std::vector<GLchar> infoLog;
			infoLog.reserve( maxLength );
			glGetProgramInfoLog( program, maxLength, &maxLength, &infoLog[ 0 ] );

			glDeleteProgram( program );

			for ( auto id : glShaderIDs )
			{
				glDeleteShader( id );
			}

			TE_CORE_ERROR( "Shader Link Failure!" );
			TE_CORE_ERROR( "{0}", infoLog.data() );
			TE_CORE_ERROR( "- End of Shader Error -" );

			return;
		}

		for ( auto id : glShaderIDs )
		{
			glDetachShader( program, id );
		}

		// Shader was successfully compiled and linked!
		m_RendererID = program;
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