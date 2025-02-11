#pragma once
#include "OpenGLCommon.h"
#include "OpenGLState.h"
#include <Tridium/Core/Assert.h>

#define FORCEINLINE __forceinline

namespace Tridium {

	class OpenGL3 : public OpenGLAPI
    {
    public:
        static FORCEINLINE void BlendFunc( GLenum a_Src, GLenum a_Dst )
        {
            glBlendFunc( a_Src, a_Dst );
        }

        static FORCEINLINE void ClearColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
        {
            glClearColor( r, g, b, a );
        }

        static FORCEINLINE void Clear( GLbitfield mask )
        {
            glClear( mask );
        }

        static FORCEINLINE void Enable( GLenum cap )
        {
            glEnable( cap );
        }

        static FORCEINLINE void Disable( GLenum cap )
        {
            glDisable( cap );
        }

        static FORCEINLINE void UseProgram( GLuint program )
        {
            glUseProgram( program );
        }

        static FORCEINLINE void GenBuffers( GLsizei n, GLuint* buffers )
        {
            glGenBuffers( n, buffers );
        }

        static FORCEINLINE void GenVertexArrays( GLsizei n, GLuint* arrays )
        {
            glGenVertexArrays( n, arrays );
        }

        static FORCEINLINE void BindBuffer( GLenum target, GLuint buffer )
        {
            glBindBuffer( target, buffer );
        }

        static FORCEINLINE void BindVertexArray( GLuint array )
        {
            glBindVertexArray( array );
        }

        static FORCEINLINE void BufferData( GLenum target, GLsizeiptr size, const void* data, GLenum usage )
        {
            glBufferData( target, size, data, usage );
        }

		static FORCEINLINE void BufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const void* data )
		{
			glBufferSubData( target, offset, size, data );
		}

        static FORCEINLINE void VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer )
        {
            glVertexAttribPointer( index, size, type, normalized, stride, pointer );
        }

        static FORCEINLINE void EnableVertexAttribArray( GLuint index )
        {
            glEnableVertexAttribArray( index );
        }

        static FORCEINLINE void DisableVertexAttribArray( GLuint index )
        {
            glDisableVertexAttribArray( index );
        }

        static FORCEINLINE void DrawArrays( GLenum mode, GLint first, GLsizei count )
        {
            glDrawArrays( mode, first, count );
        }

        static FORCEINLINE void DrawElements( GLenum mode, GLsizei count, GLenum type, const void* indices )
        {
            glDrawElements( mode, count, type, indices );
        }

        static FORCEINLINE void GenTextures( GLsizei n, GLuint* textures )
        {
            glGenTextures( n, textures );
        }

        static FORCEINLINE void BindTexture( GLenum target, GLuint texture )
        {
            glBindTexture( target, texture );
        }

        static FORCEINLINE void TexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels )
        {
            glTexImage2D( target, level, internalFormat, width, height, border, format, type, pixels );
        }

        static FORCEINLINE void TexParameteri( GLenum target, GLenum pname, GLint param )
        {
            glTexParameteri( target, pname, param );
        }

        static FORCEINLINE void ActiveTexture( GLenum texture )
        {
            glActiveTexture( texture );
        }

        static FORCEINLINE void ClearDepth( GLdouble depth )
        {
            glClearDepth( depth );
        }

        static FORCEINLINE void ClearStencil( GLint s )
        {
            glClearStencil( s );
        }

        static FORCEINLINE void StencilFunc( GLenum func, GLint ref, GLuint mask )
        {
            glStencilFunc( func, ref, mask );
        }

        static FORCEINLINE void StencilOp( GLenum sfail, GLenum dpfail, GLenum dppass )
        {
            glStencilOp( sfail, dpfail, dppass );
        }

        static FORCEINLINE void DepthFunc( GLenum func )
        {
            glDepthFunc( func );
        }

        static FORCEINLINE void DepthMask( GLboolean flag )
        {
            glDepthMask( flag );
        }

        static FORCEINLINE void FrontFace( GLenum mode )
        {
            glFrontFace( mode );
        }

        static FORCEINLINE void CullFace( GLenum mode )
        {
            glCullFace( mode );
        }

        static FORCEINLINE void LineWidth( GLfloat width )
        {
            glLineWidth( width );
        }

        static FORCEINLINE void PointSize( GLfloat size )
        {
            glPointSize( size );
        }

        static FORCEINLINE void PolygonMode( GLenum face, GLenum mode )
        {
            glPolygonMode( face, mode );
        }

        static FORCEINLINE void BindFramebuffer( GLenum target, GLuint framebuffer )
        {
            glBindFramebuffer( target, framebuffer );
        }

        static FORCEINLINE void GenFramebuffers( GLsizei n, GLuint* framebuffers )
        {
            glGenFramebuffers( n, framebuffers );
        }

        static FORCEINLINE void FramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
        {
            glFramebufferTexture2D( target, attachment, textarget, texture, level );
        }

        static FORCEINLINE void FramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
        {
            glFramebufferRenderbuffer( target, attachment, renderbuffertarget, renderbuffer );
        }

        static FORCEINLINE void CheckFramebufferStatus( GLenum target )
        {
            glCheckFramebufferStatus( target );
        }

        static FORCEINLINE void DeleteBuffers( GLsizei n, const GLuint* buffers )
        {
            glDeleteBuffers( n, buffers );
        }

        static FORCEINLINE void DeleteVertexArrays( GLsizei n, const GLuint* arrays )
        {
            glDeleteVertexArrays( n, arrays );
        }

        static FORCEINLINE void DeleteTextures( GLsizei n, const GLuint* textures )
        {
            glDeleteTextures( n, textures );
        }

        static FORCEINLINE void DeleteFramebuffers( GLsizei n, const GLuint* framebuffers )
        {
            glDeleteFramebuffers( n, framebuffers );
        }

        static FORCEINLINE void DeleteRenderbuffers( GLsizei n, const GLuint* renderbuffers )
        {
            glDeleteRenderbuffers( n, renderbuffers );
        }

        static FORCEINLINE void Flush()
        {
            glFlush();
        }

        static FORCEINLINE void Finish()
        {
            glFinish();
        }

        static FORCEINLINE void GetError()
        {
            glGetError();
        }

        static FORCEINLINE const GLubyte* GetString( GLenum name )
        {
            return glGetString( name );
        }

        static FORCEINLINE void GetIntegerv( GLenum pname, GLint* params )
        {
            glGetIntegerv( pname, params );
        }

        static FORCEINLINE void GetFloatv( GLenum pname, GLfloat* params )
        {
            glGetFloatv( pname, params );
        }

        static FORCEINLINE void GetBooleanv( GLenum pname, GLboolean* params )
        {
            glGetBooleanv( pname, params );
        }

        static FORCEINLINE void GetInteger64v( GLenum pname, GLint64* params )
        {
            glGetInteger64v( pname, params );
        }

        static FORCEINLINE void GetDoublev( GLenum pname, GLdouble* params )
        {
            glGetDoublev( pname, params );
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

        static FORCEINLINE void GetVertexAttribiv( GLuint index, GLenum pname, GLint* params )
        {
            glGetVertexAttribiv( index, pname, params );
        }

        static FORCEINLINE void GetVertexAttribPointerv( GLuint index, GLenum pname, void** pointer )
        {
            glGetVertexAttribPointerv( index, pname, pointer );
        }

        static FORCEINLINE void VertexAttrib1f( GLuint index, GLfloat x )
        {
            glVertexAttrib1f( index, x );
        }

        static FORCEINLINE void VertexAttrib2f( GLuint index, GLfloat x, GLfloat y )
        {
            glVertexAttrib2f( index, x, y );
        }

        static FORCEINLINE void VertexAttrib3f( GLuint index, GLfloat x, GLfloat y, GLfloat z )
        {
            glVertexAttrib3f( index, x, y, z );
        }

        static FORCEINLINE void VertexAttrib4f( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
        {
            glVertexAttrib4f( index, x, y, z, w );
        }

        static FORCEINLINE void StencilOpSeparate( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass )
        {
            glStencilOpSeparate( face, sfail, dpfail, dppass );
        }

        static FORCEINLINE void StencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask )
        {
            glStencilFuncSeparate( face, func, ref, mask );
        }

        static FORCEINLINE void StencilMaskSeparate( GLenum face, GLuint mask )
        {
            glStencilMaskSeparate( face, mask );
        }

        static FORCEINLINE void AttachShader( GLuint program, GLuint shader )
        {
            glAttachShader( program, shader );
        }

        static FORCEINLINE void DetachShader( GLuint program, GLuint shader )
        {
            glDetachShader( program, shader );
        }

        static FORCEINLINE void LinkProgram( GLuint program )
        {
            glLinkProgram( program );
        }

        static FORCEINLINE void ValidateProgram( GLuint program )
        {
            glValidateProgram( program );
        }

        static FORCEINLINE void UseProgramStages( GLuint pipeline, GLbitfield stages, GLuint program )
        {
            glUseProgramStages( pipeline, stages, program );
        }

        static FORCEINLINE void ActiveShaderProgram( GLuint pipeline, GLuint program )
        {
            glActiveShaderProgram( pipeline, program );
        }

        static FORCEINLINE GLuint CreateProgramPipelines( void )
        {
            GLuint pipeline;
            glGenProgramPipelines( 1, &pipeline );
            return pipeline;
        }

        static FORCEINLINE void DeleteProgramPipelines( GLsizei n, const GLuint* pipelines )
        {
            glDeleteProgramPipelines( n, pipelines );
        }

        static FORCEINLINE void BindProgramPipeline( GLuint pipeline )
        {
            glBindProgramPipeline( pipeline );
        }

        static FORCEINLINE void GetProgramPipelineiv( GLuint pipeline, GLenum pname, GLint* params )
        {
            glGetProgramPipelineiv( pipeline, pname, params );
        }

        static FORCEINLINE void ProgramUniform1f( GLuint program, GLint location, GLfloat v0 )
        {
            glProgramUniform1f( program, location, v0 );
        }

        static FORCEINLINE void ProgramUniform2f( GLuint program, GLint location, GLfloat v0, GLfloat v1 )
        {
            glProgramUniform2f( program, location, v0, v1 );
        }

        static FORCEINLINE void ProgramUniform3f( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
        {
            glProgramUniform3f( program, location, v0, v1, v2 );
        }

        static FORCEINLINE void ProgramUniform4f( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
        {
            glProgramUniform4f( program, location, v0, v1, v2, v3 );
        }

        static FORCEINLINE void ProgramUniform1i( GLuint program, GLint location, GLint v0 )
        {
            glProgramUniform1i( program, location, v0 );
        }

        static FORCEINLINE void ProgramUniform2i( GLuint program, GLint location, GLint v0, GLint v1 )
        {
            glProgramUniform2i( program, location, v0, v1 );
        }

        static FORCEINLINE void ProgramUniform3i( GLuint program, GLint location, GLint v0, GLint v1, GLint v2 )
        {
            glProgramUniform3i( program, location, v0, v1, v2 );
        }

        static FORCEINLINE void ProgramUniform4i( GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
        {
            glProgramUniform4i( program, location, v0, v1, v2, v3 );
        }

        static FORCEINLINE void ProgramUniform1fv( GLuint program, GLint location, GLsizei count, const GLfloat* value )
        {
            glProgramUniform1fv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform2fv( GLuint program, GLint location, GLsizei count, const GLfloat* value )
        {
            glProgramUniform2fv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform3fv( GLuint program, GLint location, GLsizei count, const GLfloat* value )
        {
            glProgramUniform3fv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform4fv( GLuint program, GLint location, GLsizei count, const GLfloat* value )
        {
            glProgramUniform4fv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform1iv( GLuint program, GLint location, GLsizei count, const GLint* value )
        {
            glProgramUniform1iv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform2iv( GLuint program, GLint location, GLsizei count, const GLint* value )
        {
            glProgramUniform2iv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform3iv( GLuint program, GLint location, GLsizei count, const GLint* value )
        {
            glProgramUniform3iv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform4iv( GLuint program, GLint location, GLsizei count, const GLint* value )
        {
            glProgramUniform4iv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform1uiv( GLuint program, GLint location, GLsizei count, const GLuint* value )
        {
            glProgramUniform1uiv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform2uiv( GLuint program, GLint location, GLsizei count, const GLuint* value )
        {
            glProgramUniform2uiv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform3uiv( GLuint program, GLint location, GLsizei count, const GLuint* value )
        {
            glProgramUniform3uiv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniform4uiv( GLuint program, GLint location, GLsizei count, const GLuint* value )
        {
            glProgramUniform4uiv( program, location, count, value );
        }

        static FORCEINLINE void ProgramUniformMatrix2fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix2fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix3fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix3fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix4fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix4fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix2x3fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix2x3fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix3x2fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix3x2fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix2x4fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix2x4fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix4x2fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix4x2fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix3x4fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix3x4fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void ProgramUniformMatrix4x3fv( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
        {
            glProgramUniformMatrix4x3fv( program, location, count, transpose, value );
        }

        static FORCEINLINE void GetProgramBinary( GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary )
        {
            glGetProgramBinary( program, bufSize, length, binaryFormat, binary );
        }

        static FORCEINLINE void ProgramBinary( GLuint program, GLenum binaryFormat, const void* binary, GLsizei length )
        {
            glProgramBinary( program, binaryFormat, binary, length );
        }

        static FORCEINLINE void GetActiveAtomicCounterBufferiv( GLuint program, GLuint bufferIndex, GLenum pname, GLint* params )
        {
            glGetActiveAtomicCounterBufferiv( program, bufferIndex, pname, params );
        }

        static FORCEINLINE void BindBufferBase( GLenum target, GLuint index, GLuint buffer )
        {
            glBindBufferBase( target, index, buffer );
        }

        static FORCEINLINE void BindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
        {
            glBindBufferRange( target, index, buffer, offset, size );
        }

        static FORCEINLINE void BindImageTexture( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format )
        {
            glBindImageTexture( unit, texture, level, layered, layer, access, format );
        }

        static FORCEINLINE void BindSamplers( GLuint first, GLsizei count, const GLuint* samplers )
        {
            glBindSamplers( first, count, samplers );
        }

        static FORCEINLINE void BindTextures( GLuint first, GLsizei count, const GLuint* textures )
        {
            glBindTextures( first, count, textures );
        }

        static FORCEINLINE void BindVertexBuffers( GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* sizes )
        {
            glBindVertexBuffers( first, count, buffers, offsets, sizes );
        }

		static FORCEINLINE void GetBufferParameteriv( GLenum target, GLenum pname, GLint* params )
		{
			glGetBufferParameteriv( target, pname, params );
		}
    };


} // namespace Tridium