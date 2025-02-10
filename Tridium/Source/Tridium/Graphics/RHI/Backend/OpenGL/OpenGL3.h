#pragma once
#include "OpenGLCommon.h"
#include "OpenGLState.h"
#include <Tridium/Core/Assert.h>

#define FORCEINLINE __forceinline

namespace Tridium {

    class OpenGL3 : public OpenGLAPI
    {
    public:
        static FORCEINLINE void QueryTimestampCounter( GLuint a_QueryID )
        {
            glQueryCounter( a_QueryID, GL_TIMESTAMP );
        }

        static FORCEINLINE void BeginQuery( GLenum a_QueryType, GLuint a_a_QueryID )
        {
            glBeginQuery( a_QueryType, a_a_QueryID );
        }

        static FORCEINLINE void EndQuery( GLenum a_QueryType )
        {
            glEndQuery( a_QueryType );
        }

		static FORCEINLINE void GetQueryObject( GLuint a_QueryID, GLenum a_QueryName, GLuint* o_Result )
		{
			glGetQueryObjectuiv( a_QueryID, a_QueryName, o_Result );
		}

        static FORCEINLINE void ReadBuffer( GLenum a_Mode )
        {
            glReadBuffer( a_Mode );
        }

        static FORCEINLINE void DrawBuffer( GLenum a_Mode )
        {
            glDrawBuffer( a_Mode );
        }

        static FORCEINLINE void DeleteSync( GLsync a_Sync )
        {
            glDeleteSync( a_Sync );
        }

        static FORCEINLINE GLsync FenceSync( GLenum a_Condition, GLbitfield a_Flags )
        {
            return glFenceSync( a_Condition, a_Flags );
        }

        static FORCEINLINE bool IsSync( GLsync a_Sync )
        {
            return ( glIsSync( a_Sync ) == GL_TRUE ) ? true : false;
        }

        static FORCEINLINE EFenceResult ClientWaitSync( GLsync a_Sync, GLbitfield a_Flags, GLuint64 a_Timeout )
        {
            GLenum Result = glClientWaitSync( a_Sync, a_Flags, a_Timeout );
            switch ( Result )
            {
            case GL_ALREADY_SIGNALED: return EFenceResult::AlreadySignaled;
            case GL_TIMEOUT_EXPIRED: return EFenceResult::TimeoutExpired;
            case GL_CONDITION_SATISFIED: return EFenceResult::ConditionSatisfied;
            }
            return EFenceResult::WaitFailed;
        }

        static FORCEINLINE void GenSamplers( GLsizei a_Count, GLuint* a_Samplers )
        {
            glGenSamplers( a_Count, a_Samplers );
        }

        static FORCEINLINE void DeleteSamplers( GLsizei a_Count, GLuint* a_Samplers )
        {
            glDeleteSamplers( a_Count, a_Samplers );
        }

        static FORCEINLINE void SetSamplerParameter( GLuint a_Sampler, GLenum a_Parameter, GLint a_Value )
        {
            glSamplerParameteri( a_Sampler, a_Parameter, a_Value );
        }

        static FORCEINLINE void BindSampler( GLuint a_Unit, GLuint a_Sampler )
        {
            glBindSampler( a_Unit, a_Sampler );
        }

        static FORCEINLINE void PolygonMode( GLenum a_Face, GLenum a_Mode )
        {
            glPolygonMode( a_Face, a_Mode );
        }

        static FORCEINLINE void* MapBufferRange( GLenum a_Type, uint32_t a_Offset, uint32_t a_Size, EResourceLockMode a_LockMode )
        {
            GLenum Access;
            switch ( a_LockMode )
            {
                using enum EResourceLockMode;
            case ReadOnly: Access = GL_MAP_READ_BIT; break;
            case ReadOnlyPersistent: Access = ( GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ); break;
            case WriteOnly: Access = ( GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT ); break;
            case WriteOnlyUnsynchronized: Access = ( GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT ); break;
            case WriteOnlyPersistent: Access = ( GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ); break;
            case ReadWrite:
            default: Access = ( GL_MAP_READ_BIT | GL_MAP_WRITE_BIT );
            }
            return glMapBufferRange( a_Type, a_Offset, a_Size, Access );
        }

        static FORCEINLINE void UnmapBuffer( GLenum a_Type )
        {
            glUnmapBuffer( a_Type );
        }

        static FORCEINLINE void UnmapBufferRange( GLenum a_Type, uint32_t a_Offset, uint32_t a_Size )
        {
            UnmapBuffer( a_Type );
        }

        static FORCEINLINE void GenQueries( GLsizei a_NumQueries, GLuint* a_QueryIDs )
        {
            glGenQueries( a_NumQueries, a_QueryIDs );
        }

        static FORCEINLINE void DeleteQueries( GLsizei a_NumQueries, const GLuint* a_QueryIDs )
        {
            glDeleteQueries( a_NumQueries, a_QueryIDs );
        }

        static FORCEINLINE void GetQueryObject( GLuint a_QueryID, EQueryMode a_QueryMode, GLuint* o_Result )
        {
            GLenum queryName = ( a_QueryMode == EQueryMode::Result ) ? GL_QUERY_RESULT : GL_QUERY_RESULT_AVAILABLE;
            glGetQueryObjectuiv( a_QueryID, queryName, o_Result );
        }

        static FORCEINLINE void BindBufferBase( GLenum a_Target, GLuint a_Index, GLuint a_Buffer )
        {
            glBindBufferBase( a_Target, a_Index, a_Buffer );
        }

        static FORCEINLINE void BindBufferRange( GLenum a_Target, GLuint a_Index, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
        {
            glBindBufferRange( a_Target, a_Index, a_Buffer, a_Offset, a_Size );
        }

        static FORCEINLINE GLuint GetUniformBlockIndex( GLuint a_Program, const GLchar* a_UniformBlockName )
        {
            return glGetUniformBlockIndex( a_Program, a_UniformBlockName );
        }

        static FORCEINLINE void UniformBlockBinding( GLuint a_Program, GLuint a_UniformBlockIndex, GLuint a_UniformBlockBinding )
        {
            glUniformBlockBinding( a_Program, a_UniformBlockIndex, a_UniformBlockBinding );
        }

        static FORCEINLINE void BindFragDataLocation( GLuint a_Program, GLuint a_Color, const GLchar* a_Name )
        {
            glBindFragDataLocation( a_Program, a_Color, a_Name );
        }

        static FORCEINLINE void TexParameter( GLenum a_Target, GLenum a_Parameter, GLint a_Value )
        {
            glTexParameteri( a_Target, a_Parameter, a_Value );
        }

        static FORCEINLINE void FramebufferTexture( GLenum a_Target, GLenum a_Attachment, GLuint a_Texture, GLint a_Level )
        {
            glFramebufferTexture( a_Target, a_Attachment, a_Texture, a_Level );
        }

        static FORCEINLINE void FramebufferTexture3D( GLenum a_Target, GLenum a_Attachment, GLenum a_TexTarget, GLuint a_Texture, GLint a_Level, GLint a_ZOffset )
        {
            glFramebufferTexture3D( a_Target, a_Attachment, a_TexTarget, a_Texture, a_Level, a_ZOffset );
        }

        static FORCEINLINE void FramebufferTextureLayer( GLenum a_Target, GLenum a_Attachment, GLuint a_Texture, GLint a_Level, GLint a_Layer )
        {
            glFramebufferTextureLayer( a_Target, a_Attachment, a_Texture, a_Level, a_Layer );
        }

        static FORCEINLINE void Uniform4uiv( GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
        {
            glUniform4uiv( a_Location, a_Count, a_Value );
        }

        static FORCEINLINE void BlitFramebuffer( GLint a_SrcX0, GLint a_SrcY0, GLint a_SrcX1, GLint a_SrcY1, GLint a_DstX0, GLint a_DstY0, GLint a_DstX1, GLint a_DstY1, GLbitfield a_Mask, GLenum a_Filter )
        {
            glBlitFramebuffer( a_SrcX0, a_SrcY0, a_SrcX1, a_SrcY1, a_DstX0, a_DstY0, a_DstX1, a_DstY1, a_Mask, a_Filter );
        }

        static FORCEINLINE void DrawBuffers( GLsizei a_NumBuffers, const GLenum* a_Buffers )
        {
            glDrawBuffers( a_NumBuffers, a_Buffers );
        }

        static FORCEINLINE void DepthRange( GLdouble a_Near, GLdouble a_Far )
        {
            glDepthRange( a_Near, a_Far );
        }

        static FORCEINLINE void EnableIndexed( GLenum a_Parameter, GLuint a_Index )
        {
            glEnablei( a_Parameter, a_Index );
        }

        static FORCEINLINE void DisableIndexed( GLenum a_Parameter, GLuint a_Index )
        {
            glDisablei( a_Parameter, a_Index );
        }

        static FORCEINLINE void ColorMaskIndexed( GLuint a_Index, GLboolean a_Red, GLboolean a_Green, GLboolean a_Blue, GLboolean a_Alpha )
        {
            glColorMaski( a_Index, a_Red, a_Green, a_Blue, a_Alpha );
        }

        static FORCEINLINE void VertexAttribPointer( GLuint a_Index, GLint a_Size, GLenum a_Type, GLboolean a_Normalized, GLsizei a_Stride, const GLvoid* a_Pointer )
        {
            glVertexAttribPointer( a_Index, a_Size, a_Type, a_Normalized, a_Stride, a_Pointer );
        }

        static FORCEINLINE void VertexAttribIPointer( GLuint a_Index, GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Pointer )
        {
            glVertexAttribIPointer( a_Index, a_Size, a_Type, a_Stride, a_Pointer );
        }

        static FORCEINLINE void VertexAttribDivisor( GLuint a_Index, GLuint a_Divisor )
        {
            glVertexAttribDivisor( a_Index, a_Divisor );
        }

        static FORCEINLINE void EnableVertexAttribArray( GLuint a_Index )
        {
            glEnableVertexAttribArray( a_Index );
        }

        static FORCEINLINE void DisableVertexAttribArray( GLuint a_Index )
        {
            glDisableVertexAttribArray( a_Index );
        }

        static FORCEINLINE void DrawElementsInstanced( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLsizei a_InstanceCount )
        {
            glDrawElementsInstanced( a_Mode, a_Count, a_Type, a_Indices, a_InstanceCount );
        }

        static FORCEINLINE void DrawArraysInstanced( GLenum a_Mode, GLint a_First, GLsizei a_Count, GLsizei a_InstanceCount )
        {
            glDrawArraysInstanced( a_Mode, a_First, a_Count, a_InstanceCount );
        }

        static FORCEINLINE void DrawElementsBaseVertex( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLint a_BaseVertex )
        {
            glDrawElementsBaseVertex( a_Mode, a_Count, a_Type, a_Indices, a_BaseVertex );
        }

        static FORCEINLINE void DrawRangeElements( GLenum a_Mode, GLuint a_Start, GLuint a_End, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices )
        {
            glDrawRangeElements( a_Mode, a_Start, a_End, a_Count, a_Type, a_Indices );
        }

        static FORCEINLINE void DrawRangeElementsBaseVertex( GLenum a_Mode, GLuint a_Start, GLuint a_End, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLint a_BaseVertex )
        {
            glDrawRangeElementsBaseVertex( a_Mode, a_Start, a_End, a_Count, a_Type, a_Indices, a_BaseVertex );
        }

        static FORCEINLINE void DrawArraysIndirect( GLenum a_Mode, const GLvoid* a_Indirect )
        {
            glDrawArraysIndirect( a_Mode, a_Indirect );
        }

        static FORCEINLINE void DrawElementsIndirect( GLenum a_Mode, GLenum a_Type, const GLvoid* a_Indirect )
        {
            glDrawElementsIndirect( a_Mode, a_Type, a_Indirect );
        }

        static FORCEINLINE void MultiDrawArrays( GLenum a_Mode, const GLint* a_First, const GLsizei* a_Count, GLsizei a_DrawCount )
        {
            glMultiDrawArrays( a_Mode, a_First, a_Count, a_DrawCount );
        }

        static FORCEINLINE void MultiDrawElements( GLenum a_Mode, const GLsizei* a_Count, GLenum a_Type, const GLvoid* const* a_Indices, GLsizei a_DrawCount )
        {
            glMultiDrawElements( a_Mode, a_Count, a_Type, a_Indices, a_DrawCount );
        }

        static FORCEINLINE void GenVertexArrays( GLsizei a_NumArrays, GLuint* a_Arrays )
        {
            glGenVertexArrays( a_NumArrays, a_Arrays );
        }

        static FORCEINLINE void DeleteVertexArrays( GLsizei a_NumArrays, const GLuint* a_Arrays )
        {
            glDeleteVertexArrays( a_NumArrays, a_Arrays );
        }

        static FORCEINLINE void BindVertexArray( GLuint a_Array )
        {
			OpenGLState::s_BoundVAO = a_Array;
            glBindVertexArray( a_Array );
        }

		static FORCEINLINE void GenBuffers( GLsizei a_NumBuffers, GLuint* a_Buffers )
		{
			glGenBuffers( a_NumBuffers, a_Buffers );
		}

        static FORCEINLINE void DeleteBuffers( GLsizei a_NumBuffers, const GLuint* a_Buffers )
        {
            glDeleteBuffers( a_NumBuffers, a_Buffers );
        }

		static FORCEINLINE void BindBuffer( GLenum a_Target, GLuint a_Buffer )
		{
			glBindBuffer( a_Target, a_Buffer );
		}

        static FORCEINLINE void BufferData( GLenum a_Target, GLsizeiptr a_Size, const GLvoid* a_Data, GLenum a_Usage )
        {
			glBufferData( a_Target, a_Size, a_Data, a_Usage );
        }

		static FORCEINLINE void BufferSubData( GLenum a_Target, GLintptr a_Offset, GLsizeiptr a_Size, const GLvoid* a_Data )
		{
			glBufferSubData( a_Target, a_Offset, a_Size, a_Data );
		}

		static FORCEINLINE void ClearBuffer( GLenum a_Buffer, GLint a_DrawBuffer, const GLfloat* a_Value )
		{
			glClearBufferfv( a_Buffer, a_DrawBuffer, a_Value );
		}

		static FORCEINLINE void ClearBuffer( GLenum a_Buffer, GLint a_DrawBuffer, const GLint* a_Value )
		{
			glClearBufferiv( a_Buffer, a_DrawBuffer, a_Value );
		}

		static FORCEINLINE void ClearBuffer( GLenum a_Buffer, GLint a_DrawBuffer, const GLuint* a_Value )
		{
			glClearBufferuiv( a_Buffer, a_DrawBuffer, a_Value );
		}

    };

} // namespace Tridium