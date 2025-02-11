#pragma once
#include "OpenGL3.h"

namespace Tridium {

    class OpenGL4 : public OpenGL3
    {
    public:
        static FORCEINLINE void BlendFuncSeparatei( GLuint a_Buf, GLenum a_SrcRGB, GLenum a_DstRGB, GLenum a_SrcAlpha, GLenum a_DstAlpha )
        {
            glBlendFuncSeparatei( a_Buf, a_SrcRGB, a_DstRGB, a_SrcAlpha, a_DstAlpha );
        }

        static FORCEINLINE void BindFragDataLocationIndexed( GLuint program, GLuint colorNumber, GLuint index, const GLchar* name )
        {
            glBindFragDataLocationIndexed( program, colorNumber, index, name );
        }

        static FORCEINLINE void TexStorage2D( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height )
        {
            glTexStorage2D( target, levels, internalFormat, width, height );
        }

        static FORCEINLINE void TexStorage3D( GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth )
        {
            glTexStorage3D( target, levels, internalFormat, width, height, depth );
        }

        static FORCEINLINE void ClearBufferfv( GLenum buffer, GLint drawbuffer, const GLfloat* value )
        {
            glClearBufferfv( buffer, drawbuffer, value );
        }

        static FORCEINLINE void ClearBufferiv( GLenum buffer, GLint drawbuffer, const GLint* value )
        {
            glClearBufferiv( buffer, drawbuffer, value );
        }

        static FORCEINLINE void ClearBufferuiv( GLenum buffer, GLint drawbuffer, const GLuint* value )
        {
            glClearBufferuiv( buffer, drawbuffer, value );
        }

        static FORCEINLINE void GetInternalformativ( GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params )
        {
            glGetInternalformativ( target, internalformat, pname, bufSize, params );
        }

        static FORCEINLINE void DispatchCompute( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z )
        {
            glDispatchCompute( num_groups_x, num_groups_y, num_groups_z );
        }

        static FORCEINLINE void DispatchComputeIndirect( GLintptr indirect )
        {
            glDispatchComputeIndirect( indirect );
        }

        static FORCEINLINE void CopyImageSubData( 
            GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
            GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ,
            GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth )
        {
            glCopyImageSubData( srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth );
        }

        static FORCEINLINE void FramebufferTextureLayer( GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer )
        {
            glFramebufferTextureLayer( target, attachment, texture, level, layer );
        }

        static FORCEINLINE void BindImageTexture( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format )
        {
            glBindImageTexture( unit, texture, level, layered, layer, access, format );
        }

        static FORCEINLINE void MemoryBarrier( GLbitfield barriers )
        {
            glMemoryBarrier( barriers );
        }

        static FORCEINLINE void TexBuffer( GLenum target, GLenum internalformat, GLuint buffer )
        {
            glTexBuffer( target, internalformat, buffer );
        }

        static FORCEINLINE void GetSynciv( GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values )
        {
            glGetSynciv( sync, pname, bufSize, length, values );
        }

        static FORCEINLINE void DrawTransformFeedback( GLenum mode, GLuint id )
        {
            glDrawTransformFeedback( mode, id );
        }

        static FORCEINLINE void DrawTransformFeedbackStream( GLenum mode, GLuint id, GLuint stream )
        {
            glDrawTransformFeedbackStream( mode, id, stream );
        }

        static FORCEINLINE void BeginQueryIndexed( GLenum target, GLuint index, GLuint id )
        {
            glBeginQueryIndexed( target, index, id );
        }

        static FORCEINLINE void EndQueryIndexed( GLenum target, GLuint index )
        {
            glEndQueryIndexed( target, index );
        }

        static FORCEINLINE void GetQueryIndexediv( GLenum target, GLuint index, GLenum pname, GLint* params )
        {
            glGetQueryIndexediv( target, index, pname, params );
        }

        static FORCEINLINE void MultiDrawArraysIndirect( GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride )
        {
            glMultiDrawArraysIndirect( mode, indirect, drawcount, stride );
        }

        static FORCEINLINE void MultiDrawElementsIndirect( GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride )
        {
            glMultiDrawElementsIndirect( mode, type, indirect, drawcount, stride );
        }

		//////////////////////////////////////////////////////////////////////////
        // DEBUG
		//////////////////////////////////////////////////////////////////////////

		static FORCEINLINE void DebugMessageCallback( GLDEBUGPROC callback, const void* userParam )
		{
			glDebugMessageCallback( callback, userParam );
		}
		static FORCEINLINE void DebugMessageControl( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled )
		{
			glDebugMessageControl( source, type, severity, count, ids, enabled );
		}
		static FORCEINLINE void DebugMessageInsert( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf )
		{
			glDebugMessageInsert( source, type, id, severity, length, buf );
		}
		static FORCEINLINE void PushDebugGroup( GLenum source, GLuint id, GLsizei length, const GLchar* message )
		{
			glPushDebugGroup( source, id, length, message );
		}
		static FORCEINLINE void PopDebugGroup()
		{
			glPopDebugGroup();
		}
		static FORCEINLINE void ObjectLabel( GLenum identifier, GLuint name, GLsizei length, const GLchar* label )
		{
			glObjectLabel( identifier, name, length, label );
		}
		static FORCEINLINE void GetObjectLabel( GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label )
		{
			glGetObjectLabel( identifier, name, bufSize, length, label );
		}
		static FORCEINLINE void ObjectPtrLabel( const void* ptr, GLsizei length, const GLchar* label )
		{
			glObjectPtrLabel( ptr, length, label );
		}
		static FORCEINLINE void GetObjectPtrLabel( const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label )
		{
			glGetObjectPtrLabel( ptr, bufSize, length, label );
		}
		static FORCEINLINE void GetPointerv( GLenum pname, void** params )
		{
			glGetPointerv( pname, params );
		}
		static FORCEINLINE void GetProgramInterfaceiv( GLuint program, GLenum programInterface, GLenum pname, GLint* params )
		{
			glGetProgramInterfaceiv( program, programInterface, pname, params );
		}
		static FORCEINLINE GLuint GetProgramResourceIndex( GLuint program, GLenum programInterface, const GLchar* name )
		{
			return glGetProgramResourceIndex( program, programInterface, name );
		}
        static FORCEINLINE void GetProgramResourceName( GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name )
        {
			glGetProgramResourceName( program, programInterface, index, bufSize, length, name );
        }
    };


} // namespace Tridium