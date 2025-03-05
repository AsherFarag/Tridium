#include "tripch.h"
#include "OpenGLShaderOld.h"
#include <fstream>
#include <Tridium/Utils/StringHelpers.h>

namespace Tridium {

	static bool IsValidShaderType( const std::string& a_Type )
	{
		std::string type = Util::ToLowerCase( a_Type );

		if ( type == "vertex" )
			return true;
		if ( type == "fragment" )
			return true;
		if ( type == "geometry" )
			return true;

		return false;
	}

	static GLenum ShaderTypeFromString( const std::string& a_Type )
	{
		std::string type = Util::ToLowerCase( a_Type );

		if ( type == "vertex" )
			return GL_VERTEX_SHADER;
		if ( type == "fragment" )
			return GL_FRAGMENT_SHADER;
		if ( type == "geometry" )
			return GL_GEOMETRY_SHADER;

		ASSERT_LOG( false, "Invalid shader type!" );
		return 0;
	}

	OpenGLShaderOld::OpenGLShaderOld( const std::string& a_VertexSource, const std::string& a_FragmentSource )
	{
		ShaderSources sources;
		sources[ GL_VERTEX_SHADER ] = a_VertexSource;
		sources[ GL_FRAGMENT_SHADER ] = a_FragmentSource;
		Compile( sources );
	}

	std::string OpenGLShaderOld::ReadFile( const std::string& filePath )
	{
		std::string result;
		std::ifstream in( filePath, std::ios::in | std::ios::binary );
		if ( !in )
		{
			LOG( LogCategory::Rendering, Error, "Could not open file '{0}'", filePath );
			return "";
		}

		in.seekg( 0, std::ios::end );
		result.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &result[ 0 ], result.size() );

		in.close();

		return result;
	}

	ShaderSources OpenGLShaderOld::PreProcess( const std::string& source )
	{
		TODO( "Unsafe preprocess if the source isn't perfect!" );

		ShaderSources shaderSources;

		const char* typeToken = "#pragma type";
		const size_t typeTokenLength = strlen( typeToken );
		size_t pos = source.find( typeToken, 0 );

		while ( pos != std::string::npos )
		{
			size_t eol = source.find_first_of( "\r\n", pos );
			ASSERT( eol != std::string::npos );

			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr( begin, eol - begin );
			ASSERT_LOG( IsValidShaderType(type), "Invalid shader type specification!");

			size_t nextLinePos = source.find_first_not_of( "\r\n", eol );
			pos = source.find( typeToken, nextLinePos );
			size_t endOfSource = pos - ( nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos );
			shaderSources[ ShaderTypeFromString( type ) ] = source.substr( nextLinePos, endOfSource );
		}

		return shaderSources;
	}

	void OpenGLShaderOld::Compile( const ShaderSources& shaderSources )
	{
		GLuint program = glCreateProgram();
		ASSERT_LOG( shaderSources.size() <= 3, "Only 3 shaders are supported currently!" );
		std::array<GLenum, 3> glShaderIDs;

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

				LOG( LogCategory::Rendering, Error, "Shader complilation failure! Type: '{0}'", key );
				LOG( LogCategory::Rendering, Error, "{0}", infoLog.data() );
				LOG( LogCategory::Rendering, Error, "- End of Shader Error -" );

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

			if ( maxLength == 0 )
			{
				ASSERT( false );
				return;
			}

			std::vector<GLchar> infoLog;
			infoLog.resize( maxLength );
			glGetProgramInfoLog( program, maxLength, &maxLength, &infoLog[ 0 ] );

			glDeleteProgram( program );

			for ( auto id : glShaderIDs )
			{
				glDeleteShader( id );
			}

			LOG( LogCategory::Rendering, Error, "Shader Link Failure!" );
			LOG( LogCategory::Rendering, Error, "{0}", infoLog.data() );
			LOG( LogCategory::Rendering, Error, "- End of Shader Error -" );

			return;
		}

		for ( auto id : glShaderIDs )
		{
			glDetachShader( program, id );
		}

		// Shader was successfully compiled and linked!
		m_RendererID = program;
	}

	OpenGLShaderOld::~OpenGLShaderOld()
	{
		glDeleteProgram( m_RendererID );
	}

	bool OpenGLShaderOld::Compile( const FilePath& a_Path )
	{
		glDeleteProgram( m_RendererID );

		std::string source = ReadFile( a_Path );
		auto shaderSources = PreProcess( source );
		Compile( shaderSources );
		return true;
	}

	void OpenGLShaderOld::Bind() const
	{
		glUseProgram( m_RendererID );
	}

	void OpenGLShaderOld::Unbind() const
	{
		glUseProgram( 0 );
	}

#pragma region Uniform Setters

	bool OpenGLShaderOld::SetInt( const char* name, const int val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1i( i, val );
		return true;
	}

	bool OpenGLShaderOld::SetInt2( const char* name, const iVector2& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2i( i, val.x, val.y );
		return true;
	}

	bool OpenGLShaderOld::SetInt3( const char* name, const iVector3& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}
		glUniform3i( i, val.x, val.y, val.z );
		return true;
	}

	bool OpenGLShaderOld::SetInt4( const char* name, const iVector4& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4i( i, val.x, val.y, val.z, val.w );
		return true;
	}

	bool OpenGLShaderOld::SetFloat( const char* name, const float val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1f( i, val );
		return true;
	}

	bool OpenGLShaderOld::SetFloat2( const char* name, const Vector2& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2f( i, val.x, val.y );
		return true;
	}

	bool OpenGLShaderOld::SetFloat3( const char* name, const Vector3& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3f( i, val.x, val.y, val.z );
		return true;
	}

	bool OpenGLShaderOld::SetFloat4( const char* name, const Vector4& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4f( i, val.x, val.y, val.z, val.w );
		return true;
	}

	bool OpenGLShaderOld::SetMatrix2( const char* name, const Matrix2& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix2fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true; 
	}

	bool OpenGLShaderOld::SetMatrix3( const char* name, const Matrix3& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix3fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true;
	}

	bool OpenGLShaderOld::SetMatrix4( const char* name, const Matrix4& val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix4fv( i, 1, GL_FALSE, &val[ 0 ][ 0 ] );
		return true;
	}

	bool OpenGLShaderOld::SetInt( const char* name, const uint32_t count, const int* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1iv( i, count, val );
		return true;
	}

	bool OpenGLShaderOld::SetInt2( const char* name, const uint32_t count, const iVector2* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShaderOld::SetInt3( const char* name, const uint32_t count, const iVector3* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShaderOld::SetInt4( const char* name, const uint32_t count, const iVector4* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4iv( i, count, (int*)val );
		return true;
	}

	bool OpenGLShaderOld::SetFloat( const char* name, const uint32_t count, const float* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform1fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetFloat2( const char* name, const uint32_t count, const Vector2* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform2fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetFloat3( const char* name, const uint32_t count, const Vector3* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform3fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetFloat4( const char* name, const uint32_t count, const Vector4* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniform4fv( i, count, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetMatrix2( const char* name, const uint32_t count, const Matrix2* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix2fv( i, count, GL_FALSE, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetMatrix3( const char* name, const uint32_t count, const Matrix3* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix3fv( i, count, GL_FALSE, (float*)val );
		return true;
	}

	bool OpenGLShaderOld::SetMatrix4( const char* name, const uint32_t count, const Matrix4* val )
	{
		ASSERT_LOG( m_RendererID > 0, "Invalid shader program" );
		int i = glGetUniformLocation( m_RendererID, name );
		if ( i < 0 ) {
			LOG( LogCategory::Rendering, Error, "Shader uniform {0} not found! Is it being used?", name );
			return false;
		}

		glUniformMatrix4fv( i, count, GL_FALSE, (float*)val );
		return true;
	}

#pragma endregion
}