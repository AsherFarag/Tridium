#pragma once
#include "OpenGL1.h"

namespace Tridium {

	class OpenGL2 : public OpenGL1
	{
	public:
	#pragma region OpenGL 2.0

		static FORCEINLINE void BlendEquationSeparate( GLenum a_ModeRGB, GLenum a_ModeAlpha )
		{
			glBlendEquationSeparate( a_ModeRGB, a_ModeAlpha );
		}

		static FORCEINLINE void DrawBuffers( GLsizei n, const GLenum* bufs )
		{
			glDrawBuffers( n, bufs );
		}

		static FORCEINLINE void StencilOpSeparate( GLenum a_Face, GLenum sfail, GLenum dpfail, GLenum dppass )
		{
			glStencilOpSeparate( a_Face, sfail, dpfail, dppass );
		}

		static FORCEINLINE void StencilFuncSeparate( GLenum a_Face, GLenum func, GLint ref, GLuint mask )
		{
			glStencilFuncSeparate( a_Face, func, ref, mask );
		}

		static FORCEINLINE void StencilMaskSeparate( GLenum a_Face, GLuint mask )
		{
			glStencilMaskSeparate( a_Face, mask );
		}

		static FORCEINLINE void AttachShader( GLuint program, GLuint shader )
		{
			glAttachShader( program, shader );
		}

		static FORCEINLINE void BindAttribLocation( GLuint program, GLuint a_Index, const GLchar* name )
		{
			glBindAttribLocation( program, a_Index, name );
		}

		static FORCEINLINE void CompileShader( GLuint shader )
		{
			glCompileShader( shader );
		}

		static FORCEINLINE GLuint CreateProgram( void )
		{
			return glCreateProgram();
		}

		static FORCEINLINE GLuint CreateShader( GLenum type )
		{
			return glCreateShader( type );
		}

		static FORCEINLINE void DeleteProgram( GLuint program )
		{
			glDeleteProgram( program );
		}

		static FORCEINLINE void DeleteShader( GLuint shader )
		{
			glDeleteShader( shader );
		}

		static FORCEINLINE void DetachShader( GLuint program, GLuint shader )
		{
			glDetachShader( program, shader );
		}

		static FORCEINLINE void DisableVertexAttribArray( GLuint a_Index )
		{
			glDisableVertexAttribArray( a_Index );
		}

		static FORCEINLINE void EnableVertexAttribArray( GLuint a_Index )
		{
			glEnableVertexAttribArray( a_Index );
		}

		static FORCEINLINE void GetActiveAttrib( GLuint program, GLuint a_Index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name )
		{
			glGetActiveAttrib( program, a_Index, bufSize, length, size, type, name );
		}

		static FORCEINLINE void GetActiveUniform( GLuint program, GLuint a_Index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name )
		{
			glGetActiveUniform( program, a_Index, bufSize, length, size, type, name );
		}

		static FORCEINLINE void GetAttachedShaders( GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders )
		{
			glGetAttachedShaders( program, maxCount, count, shaders );
		}

		static FORCEINLINE GLint GetAttribLocation( GLuint program, const GLchar* name )
		{
			return glGetAttribLocation( program, name );
		}

		static FORCEINLINE void GetProgramiv( GLuint program, GLenum pname, GLint* params )
		{
			glGetProgramiv( program, pname, params );
		}

		static FORCEINLINE void GetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
		{
			glGetProgramInfoLog( program, bufSize, length, infoLog );
		}

		static FORCEINLINE void GetShaderiv( GLuint shader, GLenum pname, GLint* params )
		{
			glGetShaderiv( shader, pname, params );
		}

		static FORCEINLINE void GetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
		{
			glGetShaderInfoLog( shader, bufSize, length, infoLog );
		}

		static FORCEINLINE void GetShaderSource( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source )
		{
			glGetShaderSource( shader, bufSize, length, source );
		}

		static FORCEINLINE GLint GetUniformLocation( GLuint program, const GLchar* name )
		{
			return glGetUniformLocation( program, name );
		}

		static FORCEINLINE void GetUniformfv( GLuint program, GLint location, GLfloat* params )
		{
			glGetUniformfv( program, location, params );
		}

		static FORCEINLINE void GetUniformiv( GLuint program, GLint location, GLint* params )
		{
			glGetUniformiv( program, location, params );
		}

		static FORCEINLINE void GetVertexAttribdv( GLuint a_Index, GLenum pname, GLdouble* params )
		{
			glGetVertexAttribdv( a_Index, pname, params );
		}

		static FORCEINLINE void GetVertexAttribfv( GLuint a_Index, GLenum pname, GLfloat* params )
		{
			glGetVertexAttribfv( a_Index, pname, params );
		}

		static FORCEINLINE void GetVertexAttribiv( GLuint a_Index, GLenum pname, GLint* params )
		{
			glGetVertexAttribiv( a_Index, pname, params );
		}

		static FORCEINLINE void GetVertexAttribPointerv( GLuint a_Index, GLenum pname, GLvoid** pointer )
		{
			glGetVertexAttribPointerv( a_Index, pname, pointer );
		}

		static FORCEINLINE GLboolean IsProgram( GLuint program )
		{
			return glIsProgram( program );
		}

		static FORCEINLINE GLboolean IsShader( GLuint shader )
		{
			return glIsShader( shader );
		}

		static FORCEINLINE void LinkProgram( GLuint program )
		{
			glLinkProgram( program );
		}

		static FORCEINLINE void ShaderSource( GLuint shader, GLsizei count, const GLchar** string, const GLint* length )
		{
			glShaderSource( shader, count, string, length );
		}

		static FORCEINLINE void UseProgram( GLuint program )
		{
			glUseProgram( program );
		}

		static FORCEINLINE void Uniform1f( GLint location, GLfloat v0 )
		{
			glUniform1f( location, v0 );
		}

		static FORCEINLINE void Uniform2f( GLint location, GLfloat v0, GLfloat v1 )
		{
			glUniform2f( location, v0, v1 );
		}

		static FORCEINLINE void Uniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
		{
			glUniform3f( location, v0, v1, v2 );
		}

		static FORCEINLINE void Uniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
		{
			glUniform4f( location, v0, v1, v2, v3 );
		}

		static FORCEINLINE void Uniform1i( GLint location, GLint v0 )
		{
			glUniform1i( location, v0 );
		}

		static FORCEINLINE void Uniform2i( GLint location, GLint v0, GLint v1 )
		{
			glUniform2i( location, v0, v1 );
		}


		static FORCEINLINE void Uniform3i( GLint location, GLint v0, GLint v1, GLint v2 )
		{
			glUniform3i( location, v0, v1, v2 );
		}

		static FORCEINLINE void Uniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
		{
			glUniform4i( location, v0, v1, v2, v3 );
		}

		static FORCEINLINE void Uniform1fv( GLint location, GLsizei count, const GLfloat* value )
		{
			glUniform1fv( location, count, value );
		}

		static FORCEINLINE void Uniform2fv( GLint location, GLsizei count, const GLfloat* value )
		{
			glUniform2fv( location, count, value );
		}

		static FORCEINLINE void Uniform3fv( GLint location, GLsizei count, const GLfloat* value )
		{
			glUniform3fv( location, count, value );
		}

		static FORCEINLINE void Uniform4fv( GLint location, GLsizei count, const GLfloat* value )
		{
			glUniform4fv( location, count, value );
		}

		static FORCEINLINE void Uniform1iv( GLint location, GLsizei count, const GLint* value )
		{
			glUniform1iv( location, count, value );
		}

		static FORCEINLINE void Uniform2iv( GLint location, GLsizei count, const GLint* value )
		{
			glUniform2iv( location, count, value );
		}

		static FORCEINLINE void Uniform3iv( GLint location, GLsizei count, const GLint* value )
		{
			glUniform3iv( location, count, value );
		}

		static FORCEINLINE void Uniform4iv( GLint location, GLsizei count, const GLint* value )
		{
			glUniform4iv( location, count, value );
		}

		static FORCEINLINE void UniformMatrix2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix2fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix3fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix4fv( location, count, transpose, value );
		}

		static FORCEINLINE void ValidateProgram( GLuint program )
		{
			glValidateProgram( program );
		}

		static FORCEINLINE void VertexAttrib1d( GLuint a_Index, GLdouble x )
		{
			glVertexAttrib1d( a_Index, x );
		}

		static FORCEINLINE void VertexAttrib1dv( GLuint a_Index, const GLdouble* v )
		{
			glVertexAttrib1dv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib1f( GLuint a_Index, GLfloat x )
		{
			glVertexAttrib1f( a_Index, x );
		}

		static FORCEINLINE void VertexAttrib1fv( GLuint a_Index, const GLfloat* v )
		{
			glVertexAttrib1fv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib1s( GLuint a_Index, GLshort x )
		{
			glVertexAttrib1s( a_Index, x );
		}

		static FORCEINLINE void VertexAttrib1sv( GLuint a_Index, const GLshort* v )
		{
			glVertexAttrib1sv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib2d( GLuint a_Index, GLdouble x, GLdouble y )
		{
			glVertexAttrib2d( a_Index, x, y );
		}

		static FORCEINLINE void VertexAttrib2dv( GLuint a_Index, const GLdouble* v )
		{
			glVertexAttrib2dv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib2f( GLuint a_Index, GLfloat x, GLfloat y )
		{
			glVertexAttrib2f( a_Index, x, y );
		}

		static FORCEINLINE void VertexAttrib2fv( GLuint a_Index, const GLfloat* v )
		{
			glVertexAttrib2fv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib2s( GLuint a_Index, GLshort x, GLshort y )
		{
			glVertexAttrib2s( a_Index, x, y );
		}

		static FORCEINLINE void VertexAttrib2sv( GLuint a_Index, const GLshort* v )
		{
			glVertexAttrib2sv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib3d( GLuint a_Index, GLdouble x, GLdouble y, GLdouble z )
		{
			glVertexAttrib3d( a_Index, x, y, z );
		}

		static FORCEINLINE void VertexAttrib3dv( GLuint a_Index, const GLdouble* v )
		{
			glVertexAttrib3dv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib3f( GLuint a_Index, GLfloat x, GLfloat y, GLfloat z )
		{
			glVertexAttrib3f( a_Index, x, y, z );
		}

		static FORCEINLINE void VertexAttrib3fv( GLuint a_Index, const GLfloat* v )
		{
			glVertexAttrib3fv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib3s( GLuint a_Index, GLshort x, GLshort y, GLshort z )
		{
			glVertexAttrib3s( a_Index, x, y, z );
		}

		static FORCEINLINE void VertexAttrib3sv( GLuint a_Index, const GLshort* v )
		{
			glVertexAttrib3sv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Nbv( GLuint a_Index, const GLbyte* v )
		{
			glVertexAttrib4Nbv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Niv( GLuint a_Index, const GLint* v )
		{
			glVertexAttrib4Niv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Nsv( GLuint a_Index, const GLshort* v )
		{
			glVertexAttrib4Nsv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Nub( GLuint a_Index, GLubyte x, GLubyte y, GLubyte z, GLubyte w )
		{
			glVertexAttrib4Nub( a_Index, x, y, z, w );
		}

		static FORCEINLINE void VertexAttrib4Nubv( GLuint a_Index, const GLubyte* v )
		{
			glVertexAttrib4Nubv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Nuiv( GLuint a_Index, const GLuint* v )
		{
			glVertexAttrib4Nuiv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4Nusv( GLuint a_Index, const GLushort* v )
		{
			glVertexAttrib4Nusv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4bv( GLuint a_Index, const GLbyte* v )
		{
			glVertexAttrib4bv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4d( GLuint a_Index, GLdouble x, GLdouble y, GLdouble z, GLdouble w )
		{
			glVertexAttrib4d( a_Index, x, y, z, w );
		}

		static FORCEINLINE void VertexAttrib4dv( GLuint a_Index, const GLdouble* v )
		{
			glVertexAttrib4dv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4f( GLuint a_Index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
		{
			glVertexAttrib4f( a_Index, x, y, z, w );
		}

		static FORCEINLINE void VertexAttrib4fv( GLuint a_Index, const GLfloat* v )
		{
			glVertexAttrib4fv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4iv( GLuint a_Index, const GLint* v )
		{
			glVertexAttrib4iv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4s( GLuint a_Index, GLshort x, GLshort y, GLshort z, GLshort w )
		{
			glVertexAttrib4s( a_Index, x, y, z, w );
		}

		static FORCEINLINE void VertexAttrib4sv( GLuint a_Index, const GLshort* v )
		{
			glVertexAttrib4sv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4ubv( GLuint a_Index, const GLubyte* v )
		{
			glVertexAttrib4ubv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4uiv( GLuint a_Index, const GLuint* v )
		{
			glVertexAttrib4uiv( a_Index, v );
		}

		static FORCEINLINE void VertexAttrib4usv( GLuint a_Index, const GLushort* v )
		{
			glVertexAttrib4usv( a_Index, v );
		}

		static FORCEINLINE void VertexAttribPointer( GLuint a_Index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
		{
			glVertexAttribPointer( a_Index, size, type, normalized, stride, pointer );
		}

	#pragma endregion

	#pragma region OpenGL 2.1

		static FORCEINLINE void UniformMatrix2x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix2x3fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix3x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix3x2fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix2x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix2x4fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix4x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix4x2fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix3x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix3x4fv( location, count, transpose, value );
		}

		static FORCEINLINE void UniformMatrix4x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
		{
			glUniformMatrix4x3fv( location, count, transpose, value );
		}

	#pragma endregion
	};

} // namespace Tridium