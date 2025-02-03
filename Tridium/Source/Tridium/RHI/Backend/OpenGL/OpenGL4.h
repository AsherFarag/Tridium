#pragma once
#include "OpenGL3.h"

namespace Tridium {

	class OpenGL4 : public OpenGL3
	{
	public:
		// Required
		static FORCEINLINE void BlendFuncSeparatei( GLuint a_Buf, GLenum a_SrcRGB, GLenum a_DstRGB, GLenum a_SrcAlpha, GLenum a_DstAlpha )
		{
			glBlendFuncSeparatei( a_Buf, a_SrcRGB, a_DstRGB, a_SrcAlpha, a_DstAlpha );
		}

		static FORCEINLINE void BlendEquationSeparatei( GLuint a_Buf, GLenum a_ModeRGB, GLenum a_ModeAlpha )
		{
			glBlendEquationSeparatei( a_Buf, a_ModeRGB, a_ModeAlpha );
		}

		static FORCEINLINE void BlendFunci( GLuint a_Buf, GLenum a_Src, GLenum a_Dst )
		{
			glBlendFunci( a_Buf, a_Src, a_Dst );
		}

		static FORCEINLINE void BlendEquationi( GLuint a_Buf, GLenum a_Mode )
		{
			glBlendEquationi( a_Buf, a_Mode );
		}

		static FORCEINLINE void PatchParameteri( GLenum a_Pname, GLint a_Value )
		{
			glPatchParameteri( a_Pname, a_Value );
		}

		static FORCEINLINE void BindImageTexture( GLuint a_Unit, GLuint a_Texture, GLint a_Level, GLboolean a_Layered, GLint a_Layer, GLenum a_Access, GLenum a_Format )
		{
			glBindImageTexture( a_Unit, a_Texture, a_Level, a_Layered, a_Layer, a_Access, a_Format );
		}

		static FORCEINLINE void DispatchCompute( GLuint a_NumGroupsX, GLuint a_NumGroupsY, GLuint a_NumGroupsZ )
		{
			glDispatchCompute( a_NumGroupsX, a_NumGroupsY, a_NumGroupsZ );
		}

		static FORCEINLINE void DispatchComputeIndirect( GLintptr a_Offset )
		{
			glDispatchComputeIndirect( a_Offset );
		}

		static FORCEINLINE void MemoryBarrier( GLbitfield a_Barriers )
		{
			glMemoryBarrier( a_Barriers );
		}

		static FORCEINLINE void DrawArraysIndirect( GLenum a_Mode, const GLvoid* a_Indirect )
		{
			glDrawArraysIndirect( a_Mode, a_Indirect );
		}

		static FORCEINLINE void BindVertexBuffer( GLuint a_BindingIndex, GLuint a_Buffer, GLintptr a_Offset, GLsizei a_Stride )
		{
			glBindVertexBuffer( a_BindingIndex, a_Buffer, a_Offset, a_Stride );
		}

		static FORCEINLINE void VertexAttribFormat( GLuint a_AttribIndex, GLint a_Size, GLenum a_Type, GLboolean a_Normalized, GLuint a_RelativeOffset )
		{
			glVertexAttribFormat( a_AttribIndex, a_Size, a_Type, a_Normalized, a_RelativeOffset );
		}

		static FORCEINLINE void VertexAttribIFormat( GLuint a_AttribIndex, GLint a_Size, GLenum a_Type, GLuint a_RelativeOffset )
		{
			glVertexAttribIFormat( a_AttribIndex, a_Size, a_Type, a_RelativeOffset );
		}

		static FORCEINLINE void VertexAttribBinding( GLuint a_AttribIndex, GLuint a_BindingIndex )
		{
			glVertexAttribBinding( a_AttribIndex, a_BindingIndex );
		}

		static FORCEINLINE void VertexBindingDivisor( GLuint a_BindingIndex, GLuint a_Divisor )
		{
			glVertexBindingDivisor( a_BindingIndex, a_Divisor );
		}

		static FORCEINLINE void TexBufferRange( GLenum a_Target, GLenum a_InternalFormat, GLuint a_Buffer, GLintptr a_Offset, GLsizeiptr a_Size )
		{
			glTexBufferRange( a_Target, a_InternalFormat, a_Buffer, a_Offset, a_Size );
		}

		static FORCEINLINE void TextureView( GLuint a_View, GLenum a_Target, GLuint a_Texture, GLenum a_InternalFormat, GLuint a_MinLevel, GLuint a_NumLevels, GLuint a_MinLayer, GLuint a_NumLayers )
		{
			glTextureView( a_View, a_Target, a_Texture, a_InternalFormat, a_MinLevel, a_NumLevels, a_MinLayer, a_NumLayers );
		}

		static FORCEINLINE void ClearBufferData( GLenum a_Target, GLenum a_InternalFormat, GLenum a_Format, GLenum a_Type, const void* a_Data )
		{
			glClearBufferData( a_Target, a_InternalFormat, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void GenTextures( GLsizei a_NumTextures, GLuint* a_Textures )
		{
			glGenTextures( a_NumTextures, a_Textures );
		}

		static FORCEINLINE void BindTexture( GLenum a_Target, GLuint a_Texture )
		{
			glBindTexture( a_Target, a_Texture );
		}

		static FORCEINLINE void TexImage2D( GLenum a_Target, GLint a_Level, GLint a_InternalFormat, GLsizei a_Width, GLsizei a_Height, GLint a_Border, GLenum a_Format, GLenum a_Type, const void* a_Data )
		{
			glTexImage2D( a_Target, a_Level, a_InternalFormat, a_Width, a_Height, a_Border, a_Format, a_Type, a_Data );
		}

		static FORCEINLINE void TexParameteri( GLenum a_Target, GLenum a_Pname, GLint a_Param )
		{
			glTexParameteri( a_Target, a_Pname, a_Param );
		}

		static FORCEINLINE void DeleteTextures( GLsizei a_NumTextures, const GLuint* a_Textures )
		{
			glDeleteTextures( a_NumTextures, a_Textures );
		}

		static FORCEINLINE void GenerateMipmap( GLenum a_Target )
		{
			glGenerateMipmap( a_Target );
		}

		static FORCEINLINE void FramebufferTextureLayer( GLenum a_Target, GLenum a_Attachment, GLuint a_Texture, GLint a_Level, GLint a_Layer )
		{
			glFramebufferTextureLayer( a_Target, a_Attachment, a_Texture, a_Level, a_Layer );
		}
	};

} // namespace Tridium