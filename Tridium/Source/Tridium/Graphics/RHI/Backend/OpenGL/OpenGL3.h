#pragma once
#include "OpenGL2.h"

namespace Tridium {

	class OpenGL3 : public OpenGL2
    {
    public:
    #pragma region OpenGL 3.0

		static FORCEINLINE void ColorMaski( GLuint a_Index, GLboolean a_R, GLboolean a_G, GLboolean a_B, GLboolean a_A )
		{
			glColorMaski( a_Index, a_R, a_G, a_B, a_A );
		}

		static FORCEINLINE void GetBooleani_v( GLenum a_Target, GLuint a_Index, GLboolean* a_Data )
		{
			glGetBooleani_v( a_Target, a_Index, a_Data );
		}

		//static FORCEINLINE void GetIntegeri_v( GLenum a_Target, GLuint a_Index, GLint* a_Data )
		//{
		//	glGetIntegeri_v( a_Target, a_Index, a_Data );
		//}

		static FORCEINLINE void Enablei( GLenum a_Target, GLuint a_Index )
		{
			glEnablei( a_Target, a_Index );
		}

		static FORCEINLINE void Disablei( GLenum a_Target, GLuint a_Index )
		{
			glDisablei( a_Target, a_Index );
		}

		static FORCEINLINE GLboolean IsEnabledi( GLenum a_Target, GLuint a_Index )
		{
			return glIsEnabledi( a_Target, a_Index );
		}

		static FORCEINLINE void BeginTransformFeedback( GLenum a_PrimitiveMode )
		{
			glBeginTransformFeedback( a_PrimitiveMode );
		}

		static FORCEINLINE void EndTransformFeedback( void )
		{
			glEndTransformFeedback();
		}

		//static FORCEINLINE void BindBufferRange( GLenum a_Target, GLuint a_Index, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		//{
		//	glBindBufferRange( a_Target, a_Index, a_Buffer, a_Offset, a_Size );
		//}

		//static FORCEINLINE void BindBufferBase( GLenum a_Target, GLuint a_Index, GLuint a_Buffer )
		//{
		//	glBindBufferBase( a_Target, a_Index, a_Buffer );
		//}

		static FORCEINLINE void TransformFeedbackVaryings( GLuint a_Program, GLsizei a_Count, const GLchar** a_Varyings, GLenum a_BufferMode )
		{
			glTransformFeedbackVaryings( a_Program, a_Count, a_Varyings, a_BufferMode );
		}

		static FORCEINLINE void GetTransformFeedbackVarying( GLuint a_Program, GLuint a_Index, GLsizei a_BufSize, GLsizei* a_Length, GLsizei* a_Size, GLenum* a_Type, GLchar* a_Name )
		{
			glGetTransformFeedbackVarying( a_Program, a_Index, a_BufSize, a_Length, a_Size, a_Type, a_Name );
		}

		static FORCEINLINE void ClampColor( GLenum a_Target, GLenum a_Clamp )
		{
			glClampColor( a_Target, a_Clamp );
		}

		static FORCEINLINE void BeginConditionalRender( GLuint a_Id, GLenum a_Mode )
		{
			glBeginConditionalRender( a_Id, a_Mode );
		}

		static FORCEINLINE void EndConditionalRender( void )
		{
			glEndConditionalRender();
		}

		static FORCEINLINE void VertexAttribIPointer( GLuint a_Index, GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Pointer )
		{
			glVertexAttribIPointer( a_Index, a_Size, a_Type, a_Stride, a_Pointer );
		}

		static FORCEINLINE void GetVertexAttribIiv( GLuint a_Index, GLenum a_PName, GLint* a_Params )
		{
			glGetVertexAttribIiv( a_Index, a_PName, a_Params );
		}

		static FORCEINLINE void GetVertexAttribIuiv( GLuint a_Index, GLenum a_PName, GLuint* a_Params )
		{
			glGetVertexAttribIuiv( a_Index, a_PName, a_Params );
		}

		static FORCEINLINE void VertexAttribI1i( GLuint a_Index, GLint a_X )
		{
			glVertexAttribI1i( a_Index, a_X );
		}

		static FORCEINLINE void VertexAttribI2i( GLuint a_Index, GLint a_X, GLint a_Y )
		{
			glVertexAttribI2i( a_Index, a_X, a_Y );
		}

		static FORCEINLINE void VertexAttribI3i( GLuint a_Index, GLint a_X, GLint a_Y, GLint a_Z )
		{
			glVertexAttribI3i( a_Index, a_X, a_Y, a_Z );
		}

		static FORCEINLINE void VertexAttribI4i( GLuint a_Index, GLint a_X, GLint a_Y, GLint a_Z, GLint a_W )
		{
			glVertexAttribI4i( a_Index, a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void VertexAttribI1ui( GLuint a_Index, GLuint a_X )
		{
			glVertexAttribI1ui( a_Index, a_X );
		}

		static FORCEINLINE void VertexAttribI2ui( GLuint a_Index, GLuint a_X, GLuint a_Y )
		{
			glVertexAttribI2ui( a_Index, a_X, a_Y );
		}

		static FORCEINLINE void VertexAttribI3ui( GLuint a_Index, GLuint a_X, GLuint a_Y, GLuint a_Z )
		{
			glVertexAttribI3ui( a_Index, a_X, a_Y, a_Z );
		}

		static FORCEINLINE void VertexAttribI4ui( GLuint a_Index, GLuint a_X, GLuint a_Y, GLuint a_Z, GLuint a_W )
		{
			glVertexAttribI4ui( a_Index, a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void VertexAttribI1iv( GLuint a_Index, const GLint* a_V )
		{
			glVertexAttribI1iv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI2iv( GLuint a_Index, const GLint* a_V )
		{
			glVertexAttribI2iv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI3iv( GLuint a_Index, const GLint* a_V )
		{
			glVertexAttribI3iv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4iv( GLuint a_Index, const GLint* a_V )
		{
			glVertexAttribI4iv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI1uiv( GLuint a_Index, const GLuint* a_V )
		{
			glVertexAttribI1uiv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI2uiv( GLuint a_Index, const GLuint* a_V )
		{
			glVertexAttribI2uiv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI3uiv( GLuint a_Index, const GLuint* a_V )
		{
			glVertexAttribI3uiv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4uiv( GLuint a_Index, const GLuint* a_V )
		{
			glVertexAttribI4uiv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4bv( GLuint a_Index, const GLbyte* a_V )
		{
			glVertexAttribI4bv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4sv( GLuint a_Index, const GLshort* a_V )
		{
			glVertexAttribI4sv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4ubv( GLuint a_Index, const GLubyte* a_V )
		{
			glVertexAttribI4ubv( a_Index, a_V );
		}

		static FORCEINLINE void VertexAttribI4usv( GLuint a_Index, const GLushort* a_V )
		{
			glVertexAttribI4usv( a_Index, a_V );
		}

		static FORCEINLINE void GetUniformuiv( GLuint a_Program, GLint a_Location, GLuint* a_Params )
		{
			glGetUniformuiv( a_Program, a_Location, a_Params );
		}

		static FORCEINLINE void BindFragDataLocation( GLuint a_Program, GLuint a_Color, const GLchar* a_Name )
		{
			glBindFragDataLocation( a_Program, a_Color, a_Name );
		}

		static FORCEINLINE GLint GetFragDataLocation( GLuint a_Program, const GLchar* a_Name )
		{
			return glGetFragDataLocation( a_Program, a_Name );
		}

		static FORCEINLINE void Uniform1ui( GLint a_Location, GLuint a_V0 )
		{
			glUniform1ui( a_Location, a_V0 );
		}

		static FORCEINLINE void Uniform2ui( GLint a_Location, GLuint a_V0, GLuint a_V1 )
		{
			glUniform2ui( a_Location, a_V0, a_V1 );
		}

		static FORCEINLINE void Uniform3ui( GLint a_Location, GLuint a_V0, GLuint a_V1, GLuint a_V2 )
		{
			glUniform3ui( a_Location, a_V0, a_V1, a_V2 );
		}

		static FORCEINLINE void Uniform4ui( GLint a_Location, GLuint a_V0, GLuint a_V1, GLuint a_V2, GLuint a_V3 )
		{
			glUniform4ui( a_Location, a_V0, a_V1, a_V2, a_V3 );
		}

		static FORCEINLINE void Uniform1uiv( GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glUniform1uiv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform2uiv( GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glUniform2uiv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform3uiv( GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glUniform3uiv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void Uniform4uiv( GLint a_Location, GLsizei a_Count, const GLuint* a_Value )
		{
			glUniform4uiv( a_Location, a_Count, a_Value );
		}

		static FORCEINLINE void TexParameterIiv( GLenum a_Target, GLenum a_PName, const GLint* a_Params )
		{
			glTexParameterIiv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void TexParameterIuiv( GLenum a_Target, GLenum a_PName, const GLuint* a_Params )
		{
			glTexParameterIuiv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexParameterIiv( GLenum a_Target, GLenum a_PName, GLint* a_Params )
		{
			glGetTexParameterIiv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexParameterIuiv( GLenum a_Target, GLenum a_PName, GLuint* a_Params )
		{
			glGetTexParameterIuiv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void ClearBufferiv( GLenum a_Buffer, GLint a_DrawBuffer, const GLint* a_Value )
		{
			glClearBufferiv( a_Buffer, a_DrawBuffer, a_Value );
		}

		static FORCEINLINE void ClearBufferuiv( GLenum a_Buffer, GLint a_DrawBuffer, const GLuint* a_Value )
		{
			glClearBufferuiv( a_Buffer, a_DrawBuffer, a_Value );
		}

		static FORCEINLINE void ClearBufferfv( GLenum a_Buffer, GLint a_DrawBuffer, const GLfloat* a_Value )
		{
			glClearBufferfv( a_Buffer, a_DrawBuffer, a_Value );
		}

		static FORCEINLINE void ClearBufferfi( GLenum a_Buffer, GLint a_DrawBuffer, GLfloat a_Depth, GLint a_Stencil )
		{
			glClearBufferfi( a_Buffer, a_DrawBuffer, a_Depth, a_Stencil );
		}

		static FORCEINLINE const GLubyte* GetStringi( GLenum a_Name, GLuint a_Index )
		{
			return glGetStringi( a_Name, a_Index );
		}

		static FORCEINLINE GLboolean IsRenderbuffer( GLuint a_Renderbuffer )
		{
			return glIsRenderbuffer( a_Renderbuffer );
		}

		static FORCEINLINE void BindRenderbuffer( GLenum a_Target, GLuint a_Renderbuffer )
		{
			glBindRenderbuffer( a_Target, a_Renderbuffer );
		}

		static FORCEINLINE void DeleteRenderbuffers( GLsizei a_N, const GLuint* a_Renderbuffers )
		{
			glDeleteRenderbuffers( a_N, a_Renderbuffers );
		}

		static FORCEINLINE void GenRenderbuffers( GLsizei a_N, GLuint* a_Renderbuffers )
		{
			glGenRenderbuffers( a_N, a_Renderbuffers );
		}

		static FORCEINLINE void RenderbufferStorage( GLenum a_Target, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glRenderbufferStorage( a_Target, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void GetRenderbufferParameteriv( GLenum a_Target, GLenum a_Pname, GLint* a_Params )
		{
			glGetRenderbufferParameteriv( a_Target, a_Pname, a_Params );
		}

		static FORCEINLINE GLboolean IsFramebuffer( GLuint a_Framebuffer )
		{
			return glIsFramebuffer( a_Framebuffer );
		}

		static FORCEINLINE void BindFramebuffer( GLenum a_Target, GLuint a_Framebuffer )
		{
			glBindFramebuffer( a_Target, a_Framebuffer );
		}

		static FORCEINLINE void DeleteFramebuffers( GLsizei a_N, const GLuint* a_Framebuffers )
		{
			glDeleteFramebuffers( a_N, a_Framebuffers );
		}

		static FORCEINLINE void GenFramebuffers( GLsizei a_N, GLuint* a_Framebuffers )
		{
			glGenFramebuffers( a_N, a_Framebuffers );
		}

		static FORCEINLINE GLenum CheckFramebufferStatus( GLenum a_Target )
		{
			return glCheckFramebufferStatus( a_Target );
		}

		static FORCEINLINE void FramebufferTexture1D( GLenum a_Target, GLenum a_Attachment, GLenum a_Textarget, GLuint a_Texture, GLint a_Level )
		{
			glFramebufferTexture1D( a_Target, a_Attachment, a_Textarget, a_Texture, a_Level );
		}

		static FORCEINLINE void FramebufferTexture2D( GLenum a_Target, GLenum a_Attachment, GLenum a_Textarget, GLuint a_Texture, GLint a_Level )
		{
			glFramebufferTexture2D( a_Target, a_Attachment, a_Textarget, a_Texture, a_Level );
		}

		static FORCEINLINE void FramebufferTexture3D( GLenum a_Target, GLenum a_Attachment, GLenum a_Textarget, GLuint a_Texture, GLint a_Level, GLint a_Zoffset )
		{
			glFramebufferTexture3D( a_Target, a_Attachment, a_Textarget, a_Texture, a_Level, a_Zoffset );
		}

		static FORCEINLINE void FramebufferRenderbuffer( GLenum a_Target, GLenum a_Attachment, GLenum a_Renderbuffertarget, GLuint a_Renderbuffer )
		{
			glFramebufferRenderbuffer( a_Target, a_Attachment, a_Renderbuffertarget, a_Renderbuffer );
		}

		static FORCEINLINE void GetFramebufferAttachmentParameteriv( GLenum a_Target, GLenum a_Attachment, GLenum a_Pname, GLint* a_Params )
		{
			glGetFramebufferAttachmentParameteriv( a_Target, a_Attachment, a_Pname, a_Params );
		}

		static FORCEINLINE void GenerateMipmap( GLenum a_Target )
		{
			glGenerateMipmap( a_Target );
		}

		static FORCEINLINE void BlitFramebuffer( GLint a_SrcX0, GLint a_SrcY0, GLint a_SrcX1, GLint a_SrcY1, GLint a_DstX0, GLint a_DstY0, GLint a_DstX1, GLint a_DstY1, GLbitfield a_BufferMask, GLenum a_Filter )
		{
			glBlitFramebuffer( a_SrcX0, a_SrcY0, a_SrcX1, a_SrcY1, a_DstX0, a_DstY0, a_DstX1, a_DstY1, a_BufferMask, a_Filter );
		}

		static FORCEINLINE void RenderbufferStorageMultisample( GLenum a_Target, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height )
		{
			glRenderbufferStorageMultisample( a_Target, a_Samples, a_Internalformat, a_Width, a_Height );
		}

		static FORCEINLINE void FramebufferTextureLayer( GLenum a_Target, GLenum a_Attachment, GLuint a_Texture, GLint a_Level, GLint a_Layer )
		{
			glFramebufferTextureLayer( a_Target, a_Attachment, a_Texture, a_Level, a_Layer );
		}

		static FORCEINLINE void MapBufferRange( GLenum a_Target, GLintptr a_Offset, GLsizeiptr a_Length, GLbitfield a_Access )
		{
			glMapBufferRange( a_Target, a_Offset, a_Length, a_Access );
		}

		static FORCEINLINE void FlushMappedBufferRange( GLenum a_Target, GLintptr a_Offset, GLsizeiptr a_Length )
		{
			glFlushMappedBufferRange( a_Target, a_Offset, a_Length );
		}

		static FORCEINLINE void BindVertexArray( GLuint a_Array )
		{
			glBindVertexArray( a_Array );
		}

		static FORCEINLINE void DeleteVertexArrays( GLsizei a_N, const GLuint* a_Arrays )
		{
			glDeleteVertexArrays( a_N, a_Arrays );
		}

		static FORCEINLINE void GenVertexArrays( GLsizei a_N, GLuint* a_Arrays )
		{
			glGenVertexArrays( a_N, a_Arrays );
		}

		static FORCEINLINE GLboolean IsVertexArray( GLuint a_Array )
		{
			return glIsVertexArray( a_Array );
		}

    #pragma endregion

	#pragma region OpenGL 3.1

		static FORCEINLINE void DrawArraysInstanced( GLenum a_Mode, GLint a_First, GLsizei a_Count, GLsizei a_Instancecount )
		{
			glDrawArraysInstanced( a_Mode, a_First, a_Count, a_Instancecount );
		}
		static FORCEINLINE void DrawElementsInstanced( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLsizei a_Instancecount )
		{
			glDrawElementsInstanced( a_Mode, a_Count, a_Type, a_Indices, a_Instancecount );
		}
		static FORCEINLINE void TexBuffer( GLenum a_Target, GLenum a_Internalformat, GLuint a_Buffer )
		{
			glTexBuffer( a_Target, a_Internalformat, a_Buffer );
		}
		static FORCEINLINE void PrimitiveRestartIndex( GLuint a_Index )
		{
			glPrimitiveRestartIndex( a_Index );
		}

		static FORCEINLINE void CopyBufferSubData( GLenum a_ReadTarget, GLenum a_WriteTarget, GLintptr a_ReadOffset, GLintptr a_WriteOffset, GLsizeiptr a_Size )
		{
			glCopyBufferSubData( a_ReadTarget, a_WriteTarget, a_ReadOffset, a_WriteOffset, a_Size );
		}

		static FORCEINLINE void GetUniformIndices( GLuint a_Program, GLsizei a_UniformCount, const GLchar** a_UniformNames, GLuint* a_UniformIndices )
		{
			glGetUniformIndices( a_Program, a_UniformCount, a_UniformNames, a_UniformIndices );
		}

		static FORCEINLINE void GetActiveUniformsiv( GLuint a_Program, GLsizei a_UniformCount, const GLuint* a_UniformIndices, GLenum a_Pname, GLint* a_Params )
		{
			glGetActiveUniformsiv( a_Program, a_UniformCount, a_UniformIndices, a_Pname, a_Params );
		}

		static FORCEINLINE GLuint GetUniformBlockIndex( GLuint a_Program, const GLchar* a_UniformBlockName )
		{
			return glGetUniformBlockIndex( a_Program, a_UniformBlockName );
		}

		static FORCEINLINE void GetActiveUniformBlockiv( GLuint a_Program, GLuint a_UniformBlockIndex, GLenum a_Pname, GLint* a_Params )
		{
			glGetActiveUniformBlockiv( a_Program, a_UniformBlockIndex, a_Pname, a_Params );
		}

		static FORCEINLINE void GetActiveUniformBlockName( GLuint a_Program, GLuint a_UniformBlockIndex, GLsizei a_BufSize, GLsizei* a_Length, GLchar* a_UniformBlockName )
		{
			glGetActiveUniformBlockName( a_Program, a_UniformBlockIndex, a_BufSize, a_Length, a_UniformBlockName );
		}

		static FORCEINLINE void UniformBlockBinding( GLuint a_Program, GLuint a_UniformBlockIndex, GLuint a_UniformBlockBinding )
		{
			glUniformBlockBinding( a_Program, a_UniformBlockIndex, a_UniformBlockBinding );
		}

		static FORCEINLINE void BindBufferRange( GLenum a_Target, GLuint a_Index, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		{
			glBindBufferRange( a_Target, a_Index, a_Buffer, a_Offset, a_Size );
		}

		static FORCEINLINE void BindBufferBase( GLenum a_Target, GLuint a_Index, GLuint a_Buffer )
		{
			glBindBufferBase( a_Target, a_Index, a_Buffer );
		}

		static FORCEINLINE void GetIntegeri_v( GLenum a_Target, GLuint a_Index, GLint* a_Data )
		{
			glGetIntegeri_v( a_Target, a_Index, a_Data );
		}

	#pragma endregion

	#pragma region OpenGL 3.2

		static FORCEINLINE void DrawElementsBaseVertex( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLint a_Basevertex )
		{
			glDrawElementsBaseVertex( a_Mode, a_Count, a_Type, a_Indices, a_Basevertex );
		}

		static FORCEINLINE void DrawRangeElementsBaseVertex( GLenum a_Mode, GLuint a_Start, GLuint a_End, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLint a_Basevertex )
		{
			glDrawRangeElementsBaseVertex( a_Mode, a_Start, a_End, a_Count, a_Type, a_Indices, a_Basevertex );
		}

		static FORCEINLINE void DrawElementsInstancedBaseVertex( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices, GLsizei a_Instancecount, GLint a_Basevertex )
		{
			glDrawElementsInstancedBaseVertex( a_Mode, a_Count, a_Type, a_Indices, a_Instancecount, a_Basevertex );
		}

		static FORCEINLINE void MultiDrawElementsBaseVertex( GLenum a_Mode, const GLsizei* a_Count, GLenum a_Type, const GLvoid** a_Indices, GLsizei a_Drawcount, const GLint* a_Basevertex )
		{
			glMultiDrawElementsBaseVertex( a_Mode, a_Count, a_Type, a_Indices, a_Drawcount, a_Basevertex );
		}

		static FORCEINLINE void ProvokingVertex( GLenum a_Mode )
		{
			glProvokingVertex( a_Mode );
		}

		static FORCEINLINE GLsync FenceSync( GLenum a_Condition, GLbitfield a_Flags )
		{
			return glFenceSync( a_Condition, a_Flags );
		}

		static FORCEINLINE GLboolean IsSync( GLsync a_Sync )
		{
			return glIsSync( a_Sync );
		}

		static FORCEINLINE void DeleteSync( GLsync a_Sync )
		{
			glDeleteSync( a_Sync );
		}

		static FORCEINLINE GLenum ClientWaitSync( GLsync a_Sync, GLbitfield a_Flags, GLuint64 a_Timeout )
		{
			return glClientWaitSync( a_Sync, a_Flags, a_Timeout );
		}

		static FORCEINLINE void WaitSync( GLsync a_Sync, GLbitfield a_Flags, GLuint64 a_Timeout )
		{
			glWaitSync( a_Sync, a_Flags, a_Timeout );
		}

		static FORCEINLINE void GetInteger64v( GLenum a_PName, GLint64* a_Data )
		{
			glGetInteger64v( a_PName, a_Data );
		}

		static FORCEINLINE void GetSynciv( GLsync a_Sync, GLenum a_PName, GLsizei a_BufSize, GLsizei* a_Length, GLint* a_Values )
		{
			glGetSynciv( a_Sync, a_PName, a_BufSize, a_Length, a_Values );
		}

		static FORCEINLINE void GetInteger64i_v( GLenum a_Target, GLuint a_Index, GLint64* a_Data )
		{
			glGetInteger64i_v( a_Target, a_Index, a_Data );
		}

		static FORCEINLINE void GetBufferParameteri64v( GLenum a_Target, GLenum a_Pname, GLint64* a_Params )
		{
			glGetBufferParameteri64v( a_Target, a_Pname, a_Params );
		}

		static FORCEINLINE void FramebufferTexture( GLenum a_Target, GLenum a_Attachment, GLuint a_Texture, GLint a_Level )
		{
			glFramebufferTexture( a_Target, a_Attachment, a_Texture, a_Level );
		}

		static FORCEINLINE void TexImage2DMultisample( GLenum a_Target, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLboolean a_Fixedsamplelocations )
		{
			glTexImage2DMultisample( a_Target, a_Samples, a_Internalformat, a_Width, a_Height, a_Fixedsamplelocations );
		}

		static FORCEINLINE void TexImage3DMultisample( GLenum a_Target, GLsizei a_Samples, GLenum a_Internalformat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLboolean a_Fixedsamplelocations )
		{
			glTexImage3DMultisample( a_Target, a_Samples, a_Internalformat, a_Width, a_Height, a_Depth, a_Fixedsamplelocations );
		}

		static FORCEINLINE void GetMultisamplefv( GLenum a_PName, GLuint a_Index, GLfloat* a_Val )
		{
			glGetMultisamplefv( a_PName, a_Index, a_Val );
		}

		static FORCEINLINE void SampleMaski( GLuint a_Index, GLbitfield a_Mask )
		{
			glSampleMaski( a_Index, a_Mask );
		}

	#pragma endregion
	
	#pragma region OpenGL 3.3

		static FORCEINLINE void BindFragDataLocationIndexed( GLuint a_Program, GLuint a_ColorNumber, GLuint a_Index, const GLchar* a_Name )
		{
			glBindFragDataLocationIndexed( a_Program, a_ColorNumber, a_Index, a_Name );
		}

		static FORCEINLINE GLint GetFragDataIndex( GLuint a_Program, const GLchar* a_Name )
		{
			return glGetFragDataIndex( a_Program, a_Name );
		}

		static FORCEINLINE void GenSamplers( GLsizei a_Count, GLuint* a_Samplers )
		{
			glGenSamplers( a_Count, a_Samplers );
		}

		static FORCEINLINE void DeleteSamplers( GLsizei a_Count, const GLuint* a_Samplers )
		{
			glDeleteSamplers( a_Count, a_Samplers );
		}

		static FORCEINLINE GLboolean IsSampler( GLuint a_Sampler )
		{
			return glIsSampler( a_Sampler );
		}

		static FORCEINLINE void BindSampler( GLuint a_Unit, GLuint a_Sampler )
		{
			glBindSampler( a_Unit, a_Sampler );
		}

		static FORCEINLINE void SamplerParameteri( GLuint a_Sampler, GLenum a_Pname, GLint a_Param )
		{
			glSamplerParameteri( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void SamplerParameteriv( GLuint a_Sampler, GLenum a_Pname, const GLint* a_Param )
		{
			glSamplerParameteriv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void SamplerParameterf( GLuint a_Sampler, GLenum a_Pname, GLfloat a_Param )
		{
			glSamplerParameterf( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void SamplerParameterfv( GLuint a_Sampler, GLenum a_Pname, const GLfloat* a_Param )
		{
			glSamplerParameterfv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void SamplerParameterIiv( GLuint a_Sampler, GLenum a_Pname, const GLint* a_Param )
		{
			glSamplerParameterIiv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void SamplerParameterIuiv( GLuint a_Sampler, GLenum a_Pname, const GLuint* a_Param )
		{
			glSamplerParameterIuiv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void GetSamplerParameteriv( GLuint a_Sampler, GLenum a_Pname, GLint* a_Param )
		{
			glGetSamplerParameteriv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void GetSamplerParameterIiv( GLuint a_Sampler, GLenum a_Pname, GLint* a_Param )
		{
			glGetSamplerParameterIiv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void GetSamplerParameterfv( GLuint a_Sampler, GLenum a_Pname, GLfloat* a_Param )
		{
			glGetSamplerParameterfv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void GetSamplerParameterIuiv( GLuint a_Sampler, GLenum a_Pname, GLuint* a_Param )
		{
			glGetSamplerParameterIuiv( a_Sampler, a_Pname, a_Param );
		}

		static FORCEINLINE void QueryCounter( GLuint a_Id, GLenum a_Target )
		{
			glQueryCounter( a_Id, a_Target );
		}

		static FORCEINLINE void GetQueryObjecti64v( GLuint a_Id, GLenum a_Pname, GLint64* a_Params )
		{
			glGetQueryObjecti64v( a_Id, a_Pname, a_Params );
		}

		static FORCEINLINE void GetQueryObjectui64v( GLuint a_Id, GLenum a_Pname, GLuint64* a_Params )
		{
			glGetQueryObjectui64v( a_Id, a_Pname, a_Params );
		}

		static FORCEINLINE void VertexAttribDivisor( GLuint a_Index, GLuint a_Divisor )
		{
			glVertexAttribDivisor( a_Index, a_Divisor );
		}

		static FORCEINLINE void VertexAttribP1ui( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, GLuint a_Value )
		{
			glVertexAttribP1ui( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP1uiv( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, const GLuint* a_Value )
		{
			glVertexAttribP1uiv( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP2ui( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, GLuint a_Value )
		{
			glVertexAttribP2ui( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP2uiv( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, const GLuint* a_Value )
		{
			glVertexAttribP2uiv( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP3ui( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, GLuint a_Value )
		{
			glVertexAttribP3ui( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP3uiv( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, const GLuint* a_Value )
		{
			glVertexAttribP3uiv( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP4ui( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, GLuint a_Value )
		{
			glVertexAttribP4ui( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexAttribP4uiv( GLuint a_Index, GLenum a_Type, GLboolean a_Normalized, const GLuint* a_Value )
		{
			glVertexAttribP4uiv( a_Index, a_Type, a_Normalized, a_Value );
		}

		static FORCEINLINE void VertexP2ui( GLenum a_Type, GLuint a_Value )
		{
			glVertexP2ui( a_Type, a_Value );
		}

		static FORCEINLINE void VertexP2uiv( GLenum a_Type, const GLuint* a_Value )
		{
			glVertexP2uiv( a_Type, a_Value );
		}

		static FORCEINLINE void VertexP3ui( GLenum a_Type, GLuint a_Value )
		{
			glVertexP3ui( a_Type, a_Value );
		}

		static FORCEINLINE void VertexP3uiv( GLenum a_Type, const GLuint* a_Value )
		{
			glVertexP3uiv( a_Type, a_Value );
		}

		static FORCEINLINE void VertexP4ui( GLenum a_Type, GLuint a_Value )
		{
			glVertexP4ui( a_Type, a_Value );
		}

		static FORCEINLINE void VertexP4uiv( GLenum a_Type, const GLuint* a_Value )
		{
			glVertexP4uiv( a_Type, a_Value );
		}

		static FORCEINLINE void TexCoordP1ui( GLenum a_Type, GLuint a_Coord )
		{
			glTexCoordP1ui( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP1uiv( GLenum a_Type, const GLuint* a_Coord )
		{
			glTexCoordP1uiv( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP2ui( GLenum a_Type, GLuint a_Coord )
		{
			glTexCoordP2ui( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP2uiv( GLenum a_Type, const GLuint* a_Coord )
		{
			glTexCoordP2uiv( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP3ui( GLenum a_Type, GLuint a_Coord )
		{
			glTexCoordP3ui( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP3uiv( GLenum a_Type, const GLuint* a_Coord )
		{
			glTexCoordP3uiv( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP4ui( GLenum a_Type, GLuint a_Coord )
		{
			glTexCoordP4ui( a_Type, a_Coord );
		}

		static FORCEINLINE void TexCoordP4uiv( GLenum a_Type, const GLuint* a_Coord )
		{
			glTexCoordP4uiv( a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP1ui( GLenum a_Texture, GLenum a_Type, GLuint a_Coord )
		{
			glMultiTexCoordP1ui( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP1uiv( GLenum a_Texture, GLenum a_Type, const GLuint* a_Coord )
		{
			glMultiTexCoordP1uiv( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP2ui( GLenum a_Texture, GLenum a_Type, GLuint a_Coord )
		{
			glMultiTexCoordP2ui( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP2uiv( GLenum a_Texture, GLenum a_Type, const GLuint* a_Coord )
		{
			glMultiTexCoordP2uiv( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP3ui( GLenum a_Texture, GLenum a_Type, GLuint a_Coord )
		{
			glMultiTexCoordP3ui( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP3uiv( GLenum a_Texture, GLenum a_Type, const GLuint* a_Coord )
		{
			glMultiTexCoordP3uiv( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP4ui( GLenum a_Texture, GLenum a_Type, GLuint a_Coord )
		{
			glMultiTexCoordP4ui( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void MultiTexCoordP4uiv( GLenum a_Texture, GLenum a_Type, const GLuint* a_Coord )
		{
			glMultiTexCoordP4uiv( a_Texture, a_Type, a_Coord );
		}

		static FORCEINLINE void NormalP3ui( GLenum a_Type, GLuint a_Normal )
		{
			glNormalP3ui( a_Type, a_Normal );
		}

		static FORCEINLINE void NormalP3uiv( GLenum a_Type, const GLuint* a_Normal )
		{
			glNormalP3uiv( a_Type, a_Normal );
		}

		static FORCEINLINE void ColorP3ui( GLenum a_Type, GLuint a_Color )
		{
			glColorP3ui( a_Type, a_Color );
		}

		static FORCEINLINE void ColorP3uiv( GLenum a_Type, const GLuint* a_Color )
		{
			glColorP3uiv( a_Type, a_Color );
		}

		static FORCEINLINE void ColorP4ui( GLenum a_Type, GLuint a_Color )
		{
			glColorP4ui( a_Type, a_Color );
		}

		static FORCEINLINE void ColorP4uiv( GLenum a_Type, const GLuint* a_Color )
		{
			glColorP4uiv( a_Type, a_Color );
		}

		static FORCEINLINE void SecondaryColorP3ui( GLenum a_Type, GLuint a_Color )
		{
			glSecondaryColorP3ui( a_Type, a_Color );
		}

		static FORCEINLINE void SecondaryColorP3uiv( GLenum a_Type, const GLuint* a_Color )
		{
			glSecondaryColorP3uiv( a_Type, a_Color );
		}

	#pragma endregion

    };


} // namespace Tridium