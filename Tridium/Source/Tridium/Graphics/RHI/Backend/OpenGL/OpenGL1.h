#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define FORCEINLINE __forceinline

namespace Tridium {

	// Static API wrapper for OpenGL functions
	// Newer versions can inherit from this class and override the static functions
	class OpenGLAPI
	{
	};

	class OpenGL1 : public OpenGLAPI
	{
	public:
	#pragma region OpenGL 1.0

		static FORCEINLINE void CullFace( GLenum a_Mode )
		{
			glCullFace( a_Mode );
		}

		static FORCEINLINE void FrontFace( GLenum a_Mode )
		{
			glFrontFace( a_Mode );
		}

		static FORCEINLINE void Hint( GLenum a_Target, GLenum a_Mode )
		{
			glHint( a_Target, a_Mode );
		}

		static FORCEINLINE void LineWidth( GLfloat a_Width )
		{
			glLineWidth( a_Width );
		}

		static FORCEINLINE void PolygonMode( GLenum a_Face, GLenum a_Mode )
		{
			glPolygonMode( a_Face, a_Mode );
		}

		static FORCEINLINE void Scissor( GLint x, GLint y, GLsizei a_Width, GLsizei a_Height )
		{
			glScissor( x, y, a_Width, a_Height );
		}

		static FORCEINLINE void TexParameterf( GLenum a_Target, GLenum a_PName, GLfloat a_Param )
		{
			glTexParameterf( a_Target, a_PName, a_Param );
		}

		static FORCEINLINE void TexParameterfv( GLenum a_Target, GLenum a_PName, const GLfloat* a_Params )
		{
			glTexParameterfv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void TexParameteri( GLenum a_Target, GLenum a_PName, GLint a_Param )
		{
			glTexParameteri( a_Target, a_PName, a_Param );
		}

		static FORCEINLINE void TexParameteriv( GLenum a_Target, GLenum a_PName, const GLint* a_Params )
		{
			glTexParameteriv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void TexImage1D( GLenum a_Target, GLint a_Level, GLint a_InternalFormat, GLsizei a_Width, GLint a_Border, GLenum a_Format, GLenum a_Type, const void* a_Pixels )
		{
			glTexImage1D( a_Target, a_Level, a_InternalFormat, a_Width, a_Border, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void TexImage2D( GLenum a_Target, GLint a_Level, GLint a_InternalFormat, GLsizei a_Width, GLsizei a_Height, GLint a_Border, GLenum a_Format, GLenum a_Type, const void* a_Pixels )
		{
			glTexImage2D( a_Target, a_Level, a_InternalFormat, a_Width, a_Height, a_Border, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void DrawBuffer( GLenum a_Mode )
		{
			glDrawBuffer( a_Mode );
		}

		static FORCEINLINE void Clear( GLbitfield a_Mask )
		{
			glClear( a_Mask );
		}

		static FORCEINLINE void ClearColor( GLclampf a_Red, GLclampf a_Green, GLclampf a_Blue, GLclampf a_Alpha )
		{
			glClearColor( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void ClearStencil( GLint a_S )
		{
			glClearStencil( a_S );
		}

		static FORCEINLINE void ClearDepth( GLclampd a_Depth )
		{
			glClearDepth( a_Depth );
		}

		static FORCEINLINE void StencilMask( GLuint a_Mask )
		{
			glStencilMask( a_Mask );
		}

		static FORCEINLINE void ColorMask( GLboolean a_Red, GLboolean a_Green, GLboolean a_Blue, GLboolean a_Alpha )
		{
			glColorMask( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void DepthMask( GLboolean a_Flag )
		{
			glDepthMask( a_Flag );
		}

		static FORCEINLINE void Disable( GLenum a_Cap )
		{
			glDisable( a_Cap );
		}

		static FORCEINLINE void Enable( GLenum a_Cap )
		{
			glEnable( a_Cap );
		}

		static FORCEINLINE void Finish()
		{
			glFinish();
		}

		static FORCEINLINE void Flush()
		{
			glFlush();
		}

		static FORCEINLINE void BlendFunc( GLenum a_Src, GLenum a_Dst )
		{
			glBlendFunc( a_Src, a_Dst );
		}

		static FORCEINLINE void LogicOp( GLenum a_Op )
		{
			glLogicOp( a_Op );
		}

		static FORCEINLINE void StencilFunc( GLenum a_Func, GLint a_Ref, GLuint a_Mask )
		{
			glStencilFunc( a_Func, a_Ref, a_Mask );
		}

		static FORCEINLINE void StencilOp( GLenum a_Fail, GLenum a_ZFail, GLenum a_ZPass )
		{
			glStencilOp( a_Fail, a_ZFail, a_ZPass );
		}

		static FORCEINLINE void DepthFunc( GLenum a_Func )
		{
			glDepthFunc( a_Func );
		}

		static FORCEINLINE void PixelStoref( GLenum a_PName, GLfloat a_Param )
		{
			glPixelStoref( a_PName, a_Param );
		}

		static FORCEINLINE void PixelStorei( GLenum a_PName, GLint a_Param )
		{
			glPixelStorei( a_PName, a_Param );
		}

		static FORCEINLINE void ReadBuffer( GLenum a_Mode )
		{
			glReadBuffer( a_Mode );
		}

		static FORCEINLINE void ReadPixels( GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLenum a_Type, void* a_Pixels )
		{
			glReadPixels( a_X, a_Y, a_Width, a_Height, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void GetBooleanv( GLenum a_PName, GLboolean* a_Params )
		{
			glGetBooleanv( a_PName, a_Params );
		}

		static FORCEINLINE void GetDoublev( GLenum a_PName, GLdouble* a_Params )
		{
			glGetDoublev( a_PName, a_Params );
		}

		static FORCEINLINE GLenum GetError()
		{
			return glGetError();
		}

		static FORCEINLINE void GetFloatv( GLenum a_PName, GLfloat* a_Params )
		{
			glGetFloatv( a_PName, a_Params );
		}

		static FORCEINLINE void GetIntegerv( GLenum a_PName, GLint* a_Params )
		{
			glGetIntegerv( a_PName, a_Params );
		}

		static FORCEINLINE const GLubyte* GetString( GLenum a_Name )
		{
			return glGetString( a_Name );
		}

		static FORCEINLINE void GetTexImage( GLenum a_Target, GLint a_Level, GLenum a_Format, GLenum a_Type, void* a_Pixels )
		{
			glGetTexImage( a_Target, a_Level, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void GetTexParameterfv( GLenum a_Target, GLenum a_PName, GLfloat* a_Params )
		{
			glGetTexParameterfv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexParameteriv( GLenum a_Target, GLenum a_PName, GLint* a_Params )
		{
			glGetTexParameteriv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexLevelParameterfv( GLenum a_Target, GLint a_Level, GLenum a_PName, GLfloat* a_Params )
		{
			glGetTexLevelParameterfv( a_Target, a_Level, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexLevelParameteriv( GLenum a_Target, GLint a_Level, GLenum a_PName, GLint* a_Params )
		{
			glGetTexLevelParameteriv( a_Target, a_Level, a_PName, a_Params );
		}

		static FORCEINLINE GLboolean IsEnabled( GLenum a_Cap )
		{
			return glIsEnabled( a_Cap );
		}

		static FORCEINLINE void DepthRange( GLclampd a_Near, GLclampd a_Far )
		{
			glDepthRange( a_Near, a_Far );
		}

		static FORCEINLINE void Viewport( GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glViewport( a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void NewList( GLuint a_List, GLenum a_Mode )
		{
			glNewList( a_List, a_Mode );
		}

		static FORCEINLINE void EndList()
		{
			glEndList();
		}

		static FORCEINLINE void CallList( GLuint a_List )
		{
			glCallList( a_List );
		}

		static FORCEINLINE void CallLists( GLsizei a_N, GLenum a_Type, const GLvoid* a_Lists )
		{
			glCallLists( a_N, a_Type, a_Lists );
		}

		static FORCEINLINE void DeleteLists( GLuint a_List, GLsizei a_Range )
		{
			glDeleteLists( a_List, a_Range );
		}

		static FORCEINLINE GLuint GenLists( GLsizei a_Range )
		{
			return glGenLists( a_Range );
		}

		static FORCEINLINE void ListBase( GLuint a_Base )
		{
			glListBase( a_Base );
		}

		static FORCEINLINE void Begin( GLenum a_Mode )
		{
			glBegin( a_Mode );
		}

		static FORCEINLINE void Bitmap( GLsizei a_Width, GLsizei a_Height, GLfloat a_Xorig, GLfloat a_Yorig, GLfloat a_Xmove, GLfloat a_Ymove, const GLubyte* a_Bitmap )
		{
			glBitmap( a_Width, a_Height, a_Xorig, a_Yorig, a_Xmove, a_Ymove, a_Bitmap );
		}

		static FORCEINLINE void Color3b( GLbyte a_Red, GLbyte a_Green, GLbyte a_Blue )
		{
			glColor3b( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3bv( const GLbyte* a_V )
		{
			glColor3bv( a_V );
		}

		static FORCEINLINE void Color3d( GLdouble a_Red, GLdouble a_Green, GLdouble a_Blue )
		{
			glColor3d( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3dv( const GLdouble* a_V )
		{
			glColor3dv( a_V );
		}

		static FORCEINLINE void Color3f( GLfloat a_Red, GLfloat a_Green, GLfloat a_Blue )
		{
			glColor3f( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3fv( const GLfloat* a_V )
		{
			glColor3fv( a_V );
		}

		static FORCEINLINE void Color3i( GLint a_Red, GLint a_Green, GLint a_Blue )
		{
			glColor3i( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3iv( const GLint* a_V )
		{
			glColor3iv( a_V );
		}

		static FORCEINLINE void Color3s( GLshort a_Red, GLshort a_Green, GLshort a_Blue )
		{
			glColor3s( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3sv( const GLshort* a_V )
		{
			glColor3sv( a_V );
		}

		static FORCEINLINE void Color3ub( GLubyte a_Red, GLubyte a_Green, GLubyte a_Blue )
		{
			glColor3ub( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3ubv( const GLubyte* a_V )
		{
			glColor3ubv( a_V );
		}

		static FORCEINLINE void Color3ui( GLuint a_Red, GLuint a_Green, GLuint a_Blue )
		{
			glColor3ui( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3uiv( const GLuint* a_V )
		{
			glColor3uiv( a_V );
		}

		static FORCEINLINE void Color3us( GLushort a_Red, GLushort a_Green, GLushort a_Blue )
		{
			glColor3us( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void Color3usv( const GLushort* a_V )
		{
			glColor3usv( a_V );
		}

		static FORCEINLINE void Color4b( GLbyte a_Red, GLbyte a_Green, GLbyte a_Blue, GLbyte a_Alpha )
		{
			glColor4b( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4bv( const GLbyte* a_V )
		{
			glColor4bv( a_V );
		}

		static FORCEINLINE void Color4d( GLdouble a_Red, GLdouble a_Green, GLdouble a_Blue, GLdouble a_Alpha )
		{
			glColor4d( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4dv( const GLdouble* a_V )
		{
			glColor4dv( a_V );
		}

		static FORCEINLINE void Color4f( GLfloat a_Red, GLfloat a_Green, GLfloat a_Blue, GLfloat a_Alpha )
		{
			glColor4f( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4fv( const GLfloat* a_V )
		{
			glColor4fv( a_V );
		}

		static FORCEINLINE void Color4i( GLint a_Red, GLint a_Green, GLint a_Blue, GLint a_Alpha )
		{
			glColor4i( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4iv( const GLint* a_V )
		{
			glColor4iv( a_V );
		}

		static FORCEINLINE void Color4s( GLshort a_Red, GLshort a_Green, GLshort a_Blue, GLshort a_Alpha )
		{
			glColor4s( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4sv( const GLshort* a_V )
		{
			glColor4sv( a_V );
		}

		static FORCEINLINE void Color4ub( GLubyte a_Red, GLubyte a_Green, GLubyte a_Blue, GLubyte a_Alpha )
		{
			glColor4ub( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4ubv( const GLubyte* a_V )
		{
			glColor4ubv( a_V );
		}

		static FORCEINLINE void Color4ui( GLuint a_Red, GLuint a_Green, GLuint a_Blue, GLuint a_Alpha )
		{
			glColor4ui( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4uiv( const GLuint* a_V )
		{
			glColor4uiv( a_V );
		}

		static FORCEINLINE void Color4us( GLushort a_Red, GLushort a_Green, GLushort a_Blue, GLushort a_Alpha )
		{
			glColor4us( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void Color4usv( const GLushort* a_V )
		{
			glColor4usv( a_V );
		}

		static FORCEINLINE void EdgeFlag( GLboolean a_Flag )
		{
			glEdgeFlag( a_Flag );
		}

		static FORCEINLINE void EdgeFlagv( const GLboolean* a_Flag )
		{
			glEdgeFlagv( a_Flag );
		}

		static FORCEINLINE void End()
		{
			glEnd();
		}

		static FORCEINLINE void Indexd( GLdouble a_C )
		{
			glIndexd( a_C );
		}

		static FORCEINLINE void Indexdv( const GLdouble* a_C )
		{
			glIndexdv( a_C );
		}

		static FORCEINLINE void Indexf( GLfloat a_C )
		{
			glIndexf( a_C );
		}

		static FORCEINLINE void Indexfv( const GLfloat* a_C )
		{
			glIndexfv( a_C );
		}

		static FORCEINLINE void Indexi( GLint a_C )
		{
			glIndexi( a_C );
		}

		static FORCEINLINE void Indexiv( const GLint* a_C )
		{
			glIndexiv( a_C );
		}

		static FORCEINLINE void Indexs( GLshort a_C )
		{
			glIndexs( a_C );
		}

		static FORCEINLINE void Indexsv( const GLshort* a_C )
		{
			glIndexsv( a_C );
		}

		static FORCEINLINE void Normal3b( GLbyte a_Nx, GLbyte a_Ny, GLbyte a_Nz )
		{
			glNormal3b( a_Nx, a_Ny, a_Nz );
		}

		static FORCEINLINE void Normal3bv( const GLbyte* a_V )
		{
			glNormal3bv( a_V );
		}

		static FORCEINLINE void Normal3d( GLdouble a_Nx, GLdouble a_Ny, GLdouble a_Nz )
		{
			glNormal3d( a_Nx, a_Ny, a_Nz );
		}

		static FORCEINLINE void Normal3dv( const GLdouble* a_V )
		{
			glNormal3dv( a_V );
		}

		static FORCEINLINE void Normal3f( GLfloat a_Nx, GLfloat a_Ny, GLfloat a_Nz )
		{
			glNormal3f( a_Nx, a_Ny, a_Nz );
		}

		static FORCEINLINE void Normal3fv( const GLfloat* a_V )
		{
			glNormal3fv( a_V );
		}

		static FORCEINLINE void Normal3i( GLint a_Nx, GLint a_Ny, GLint a_Nz )
		{
			glNormal3i( a_Nx, a_Ny, a_Nz );
		}

		static FORCEINLINE void Normal3iv( const GLint* a_V )
		{
			glNormal3iv( a_V );
		}

		static FORCEINLINE void Normal3s( GLshort a_Nx, GLshort a_Ny, GLshort a_Nz )
		{
			glNormal3s( a_Nx, a_Ny, a_Nz );
		}

		static FORCEINLINE void Normal3sv( const GLshort* a_V )
		{
			glNormal3sv( a_V );
		}

		static FORCEINLINE void RasterPos2d( GLdouble a_X, GLdouble a_Y )
		{
			glRasterPos2d( a_X, a_Y );
		}

		static FORCEINLINE void RasterPos2dv( const GLdouble* a_V )
		{
			glRasterPos2dv( a_V );
		}

		static FORCEINLINE void RasterPos2f( GLfloat a_X, GLfloat a_Y )
		{
			glRasterPos2f( a_X, a_Y );
		}

		static FORCEINLINE void RasterPos2fv( const GLfloat* a_V )
		{
			glRasterPos2fv( a_V );
		}

		static FORCEINLINE void RasterPos2i( GLint a_X, GLint a_Y )
		{
			glRasterPos2i( a_X, a_Y );
		}

		static FORCEINLINE void RasterPos2iv( const GLint* a_V )
		{
			glRasterPos2iv( a_V );
		}

		static FORCEINLINE void RasterPos2s( GLshort a_X, GLshort a_Y )
		{
			glRasterPos2s( a_X, a_Y );
		}

		static FORCEINLINE void RasterPos2sv( const GLshort* a_V )
		{
			glRasterPos2sv( a_V );
		}

		static FORCEINLINE void RasterPos3d( GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glRasterPos3d( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void RasterPos3dv( const GLdouble* a_V )
		{
			glRasterPos3dv( a_V );
		}

		static FORCEINLINE void RasterPos3f( GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glRasterPos3f( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void RasterPos3fv( const GLfloat* a_V )
		{
			glRasterPos3fv( a_V );
		}

		static FORCEINLINE void RasterPos3i( GLint a_X, GLint a_Y, GLint a_Z )
		{
			glRasterPos3i( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void RasterPos3iv( const GLint* a_V )
		{
			glRasterPos3iv( a_V );
		}

		static FORCEINLINE void RasterPos3s( GLshort a_X, GLshort a_Y, GLshort a_Z )
		{
			glRasterPos3s( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void RasterPos3sv( const GLshort* a_V )
		{
			glRasterPos3sv( a_V );
		}

		static FORCEINLINE void RasterPos4d( GLdouble a_X, GLdouble a_Y, GLdouble a_Z, GLdouble a_W )
		{
			glRasterPos4d( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void RasterPos4dv( const GLdouble* a_V )
		{
			glRasterPos4dv( a_V );
		}

		static FORCEINLINE void RasterPos4f( GLfloat a_X, GLfloat a_Y, GLfloat a_Z, GLfloat a_W )
		{
			glRasterPos4f( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void RasterPos4fv( const GLfloat* a_V )
		{
			glRasterPos4fv( a_V );
		}

		static FORCEINLINE void RasterPos4i( GLint a_X, GLint a_Y, GLint a_Z, GLint a_W )
		{
			glRasterPos4i( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void RasterPos4iv( const GLint* a_V )
		{
			glRasterPos4iv( a_V );
		}

		static FORCEINLINE void RasterPos4s( GLshort a_X, GLshort a_Y, GLshort a_Z, GLshort a_W )
		{
			glRasterPos4s( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void RasterPos4sv( const GLshort* a_V )
		{
			glRasterPos4sv( a_V );
		}

		static FORCEINLINE void Rectd( GLdouble a_X1, GLdouble a_Y1, GLdouble a_X2, GLdouble a_Y2 )
		{
			glRectd( a_X1, a_Y1, a_X2, a_Y2 );
		}

		static FORCEINLINE void Rectdv( const GLdouble* a_V1, const GLdouble* a_V2 )
		{
			glRectdv( a_V1, a_V2 );
		}

		static FORCEINLINE void Rectf( GLfloat a_X1, GLfloat a_Y1, GLfloat a_X2, GLfloat a_Y2 )
		{
			glRectf( a_X1, a_Y1, a_X2, a_Y2 );
		}

		static FORCEINLINE void Rectfv( const GLfloat* a_V1, const GLfloat* a_V2 )
		{
			glRectfv( a_V1, a_V2 );
		}

		static FORCEINLINE void Recti( GLint a_X1, GLint a_Y1, GLint a_X2, GLint a_Y2 )
		{
			glRecti( a_X1, a_Y1, a_X2, a_Y2 );
		}

		static FORCEINLINE void Rectiv( const GLint* a_V1, const GLint* a_V2 )
		{
			glRectiv( a_V1, a_V2 );
		}

		static FORCEINLINE void Rects( GLshort a_X1, GLshort a_Y1, GLshort a_X2, GLshort a_Y2 )
		{
			glRects( a_X1, a_Y1, a_X2, a_Y2 );
		}

		static FORCEINLINE void Rectsv( const GLshort* a_V1, const GLshort* a_V2 )
		{
			glRectsv( a_V1, a_V2 );
		}

		static FORCEINLINE void TexCoord1d( GLdouble a_S )
		{
			glTexCoord1d( a_S );
		}

		static FORCEINLINE void TexCoord1dv( const GLdouble* a_V )
		{
			glTexCoord1dv( a_V );
		}

		static FORCEINLINE void TexCoord1f( GLfloat a_S )
		{
			glTexCoord1f( a_S );
		}

		static FORCEINLINE void TexCoord1fv( const GLfloat* a_V )
		{
			glTexCoord1fv( a_V );
		}

		static FORCEINLINE void TexCoord1i( GLint a_S )
		{
			glTexCoord1i( a_S );
		}

		static FORCEINLINE void TexCoord1iv( const GLint* a_V )
		{
			glTexCoord1iv( a_V );
		}

		static FORCEINLINE void TexCoord1s( GLshort a_S )
		{
			glTexCoord1s( a_S );
		}

		static FORCEINLINE void TexCoord1sv( const GLshort* a_V )
		{
			glTexCoord1sv( a_V );
		}

		static FORCEINLINE void TexCoord2d( GLdouble a_S, GLdouble a_T )
		{
			glTexCoord2d( a_S, a_T );
		}

		static FORCEINLINE void TexCoord2dv( const GLdouble* a_V )
		{
			glTexCoord2dv( a_V );
		}

		static FORCEINLINE void TexCoord2f( GLfloat a_S, GLfloat a_T )
		{
			glTexCoord2f( a_S, a_T );
		}

		static FORCEINLINE void TexCoord2fv( const GLfloat* a_V )
		{
			glTexCoord2fv( a_V );
		}

		static FORCEINLINE void TexCoord2i( GLint a_S, GLint a_T )
		{
			glTexCoord2i( a_S, a_T );
		}

		static FORCEINLINE void TexCoord2iv( const GLint* a_V )
		{
			glTexCoord2iv( a_V );
		}

		static FORCEINLINE void TexCoord2s( GLshort a_S, GLshort a_T )
		{
			glTexCoord2s( a_S, a_T );
		}

		static FORCEINLINE void TexCoord2sv( const GLshort* a_V )
		{
			glTexCoord2sv( a_V );
		}

		static FORCEINLINE void TexCoord3d( GLdouble a_S, GLdouble a_T, GLdouble a_R )
		{
			glTexCoord3d( a_S, a_T, a_R );
		}

		static FORCEINLINE void TexCoord3dv( const GLdouble* a_V )
		{
			glTexCoord3dv( a_V );
		}

		static FORCEINLINE void TexCoord3f( GLfloat a_S, GLfloat a_T, GLfloat a_R )
		{
			glTexCoord3f( a_S, a_T, a_R );
		}

		static FORCEINLINE void TexCoord3fv( const GLfloat* a_V )
		{
			glTexCoord3fv( a_V );
		}

		static FORCEINLINE void TexCoord3i( GLint a_S, GLint a_T, GLint a_R )
		{
			glTexCoord3i( a_S, a_T, a_R );
		}

		static FORCEINLINE void TexCoord3iv( const GLint* a_V )
		{
			glTexCoord3iv( a_V );
		}

		static FORCEINLINE void TexCoord3s( GLshort a_S, GLshort a_T, GLshort a_R )
		{
			glTexCoord3s( a_S, a_T, a_R );
		}

		static FORCEINLINE void TexCoord3sv( const GLshort* a_V )
		{
			glTexCoord3sv( a_V );
		}

		static FORCEINLINE void TexCoord4d( GLdouble a_S, GLdouble a_T, GLdouble a_R, GLdouble a_Q )
		{
			glTexCoord4d( a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void TexCoord4dv( const GLdouble* a_V )
		{
			glTexCoord4dv( a_V );
		}

		static FORCEINLINE void TexCoord4f( GLfloat a_S, GLfloat a_T, GLfloat a_R, GLfloat a_Q )
		{
			glTexCoord4f( a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void TexCoord4fv( const GLfloat* a_V )
		{
			glTexCoord4fv( a_V );
		}

		static FORCEINLINE void TexCoord4i( GLint a_S, GLint a_T, GLint a_R, GLint a_Q )
		{
			glTexCoord4i( a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void TexCoord4iv( const GLint* a_V )
		{
			glTexCoord4iv( a_V );
		}

		static FORCEINLINE void TexCoord4s( GLshort a_S, GLshort a_T, GLshort a_R, GLshort a_Q )
		{
			glTexCoord4s( a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void TexCoord4sv( const GLshort* a_V )
		{
			glTexCoord4sv( a_V );
		}

		static FORCEINLINE void Vertex2d( GLdouble a_X, GLdouble a_Y )
		{
			glVertex2d( a_X, a_Y );
		}

		static FORCEINLINE void Vertex2dv( const GLdouble* a_V )
		{
			glVertex2dv( a_V );
		}

		static FORCEINLINE void Vertex2f( GLfloat a_X, GLfloat a_Y )
		{
			glVertex2f( a_X, a_Y );
		}

		static FORCEINLINE void Vertex2fv( const GLfloat* a_V )
		{
			glVertex2fv( a_V );
		}

		static FORCEINLINE void Vertex2i( GLint a_X, GLint a_Y )
		{
			glVertex2i( a_X, a_Y );
		}

		static FORCEINLINE void Vertex2iv( const GLint* a_V )
		{
			glVertex2iv( a_V );
		}

		static FORCEINLINE void Vertex2s( GLshort a_X, GLshort a_Y )
		{
			glVertex2s( a_X, a_Y );
		}

		static FORCEINLINE void Vertex2sv( const GLshort* a_V )
		{
			glVertex2sv( a_V );
		}

		static FORCEINLINE void Vertex3d( GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glVertex3d( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Vertex3dv( const GLdouble* a_V )
		{
			glVertex3dv( a_V );
		}

		static FORCEINLINE void Vertex3f( GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glVertex3f( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Vertex3fv( const GLfloat* a_V )
		{
			glVertex3fv( a_V );
		}

		static FORCEINLINE void Vertex3i( GLint a_X, GLint a_Y, GLint a_Z )
		{
			glVertex3i( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Vertex3iv( const GLint* a_V )
		{
			glVertex3iv( a_V );
		}

		static FORCEINLINE void Vertex3s( GLshort a_X, GLshort a_Y, GLshort a_Z )
		{
			glVertex3s( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Vertex3sv( const GLshort* a_V )
		{
			glVertex3sv( a_V );
		}

		static FORCEINLINE void Vertex4d( GLdouble a_X, GLdouble a_Y, GLdouble a_Z, GLdouble a_W )
		{
			glVertex4d( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void Vertex4dv( const GLdouble* a_V )
		{
			glVertex4dv( a_V );
		}

		static FORCEINLINE void Vertex4f( GLfloat a_X, GLfloat a_Y, GLfloat a_Z, GLfloat a_W )
		{
			glVertex4f( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void Vertex4fv( const GLfloat* a_V )
		{
			glVertex4fv( a_V );
		}

		static FORCEINLINE void Vertex4i( GLint a_X, GLint a_Y, GLint a_Z, GLint a_W )
		{
			glVertex4i( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void Vertex4iv( const GLint* a_V )
		{
			glVertex4iv( a_V );
		}

		static FORCEINLINE void Vertex4s( GLshort a_X, GLshort a_Y, GLshort a_Z, GLshort a_W )
		{
			glVertex4s( a_X, a_Y, a_Z, a_W );
		}

		static FORCEINLINE void Vertex4sv( const GLshort* a_V )
		{
			glVertex4sv( a_V );
		}

		static FORCEINLINE void ClipPlane( GLenum a_Plane, const GLdouble* a_Equation )
		{
			glClipPlane( a_Plane, a_Equation );
		}

		static FORCEINLINE void ColorMaterial( GLenum a_Face, GLenum a_Mode )
		{
			glColorMaterial( a_Face, a_Mode );
		}

		static FORCEINLINE void Fogf( GLenum a_PName, GLfloat a_Param )
		{
			glFogf( a_PName, a_Param );
		}

		static FORCEINLINE void Fogfv( GLenum a_PName, const GLfloat* a_Params )
		{
			glFogfv( a_PName, a_Params );
		}

		static FORCEINLINE void Fogi( GLenum a_PName, GLint a_Param )
		{
			glFogi( a_PName, a_Param );
		}

		static FORCEINLINE void Fogiv( GLenum a_PName, const GLint* a_Params )
		{
			glFogiv( a_PName, a_Params );
		}

		static FORCEINLINE void Lightf( GLenum a_Light, GLenum a_PName, GLfloat a_Param )
		{
			glLightf( a_Light, a_PName, a_Param );
		}

		static FORCEINLINE void Lightfv( GLenum a_Light, GLenum a_PName, const GLfloat* a_Params )
		{
			glLightfv( a_Light, a_PName, a_Params );
		}

		static FORCEINLINE void Lighti( GLenum a_Light, GLenum a_PName, GLint a_Param )
		{
			glLighti( a_Light, a_PName, a_Param );
		}

		static FORCEINLINE void Lightiv( GLenum a_Light, GLenum a_PName, const GLint* a_Params )
		{
			glLightiv( a_Light, a_PName, a_Params );
		}

		static FORCEINLINE void LightModelf( GLenum a_PName, GLfloat a_Param )
		{
			glLightModelf( a_PName, a_Param );
		}

		static FORCEINLINE void LightModelfv( GLenum a_PName, const GLfloat* a_Params )
		{
			glLightModelfv( a_PName, a_Params );
		}

		static FORCEINLINE void LightModeli( GLenum a_PName, GLint a_Param )
		{
			glLightModeli( a_PName, a_Param );
		}

		static FORCEINLINE void LightModeliv( GLenum a_PName, const GLint* a_Params )
		{
			glLightModeliv( a_PName, a_Params );
		}

		static FORCEINLINE void LineStipple( GLint a_Factor, GLushort a_Pattern )
		{
			glLineStipple( a_Factor, a_Pattern );
		}

		static FORCEINLINE void Materialf( GLenum a_Face, GLenum a_PName, GLfloat a_Param )
		{
			glMaterialf( a_Face, a_PName, a_Param );
		}

		static FORCEINLINE void Materialfv( GLenum a_Face, GLenum a_PName, const GLfloat* a_Params )
		{
			glMaterialfv( a_Face, a_PName, a_Params );
		}

		static FORCEINLINE void Materiali( GLenum a_Face, GLenum a_PName, GLint a_Param )
		{
			glMateriali( a_Face, a_PName, a_Param );
		}

		static FORCEINLINE void Materialiv( GLenum a_Face, GLenum a_PName, const GLint* a_Params )
		{
			glMaterialiv( a_Face, a_PName, a_Params );
		}

		static FORCEINLINE void PolygonStipple( const GLubyte* a_Mask )
		{
			glPolygonStipple( a_Mask );
		}

		static FORCEINLINE void ShadeModel( GLenum a_Mode )
		{
			glShadeModel( a_Mode );
		}

		static FORCEINLINE void TexEnvf( GLenum a_Target, GLenum a_PName, GLfloat a_Param )
		{
			glTexEnvf( a_Target, a_PName, a_Param );
		}

		static FORCEINLINE void TexEnvfv( GLenum a_Target, GLenum a_PName, const GLfloat* a_Params )
		{
			glTexEnvfv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void TexEnvi( GLenum a_Target, GLenum a_PName, GLint a_Param )
		{
			glTexEnvi( a_Target, a_PName, a_Param );
		}

		static FORCEINLINE void TexEnviv( GLenum a_Target, GLenum a_PName, const GLint* a_Params )
		{
			glTexEnviv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void TexGend( GLenum a_Coord, GLenum a_PName, GLdouble a_Param )
		{
			glTexGend( a_Coord, a_PName, a_Param );
		}

		static FORCEINLINE void TexGendv( GLenum a_Coord, GLenum a_PName, const GLdouble* a_Params )
		{
			glTexGendv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void TexGenf( GLenum a_Coord, GLenum a_PName, GLfloat a_Param )
		{
			glTexGenf( a_Coord, a_PName, a_Param );
		}

		static FORCEINLINE void TexGenfv( GLenum a_Coord, GLenum a_PName, const GLfloat* a_Params )
		{
			glTexGenfv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void TexGeni( GLenum a_Coord, GLenum a_PName, GLint a_Param )
		{
			glTexGeni( a_Coord, a_PName, a_Param );
		}

		static FORCEINLINE void TexGeniv( GLenum a_Coord, GLenum a_PName, const GLint* a_Params )
		{
			glTexGeniv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void FeedbackBuffer( GLsizei a_Size, GLenum a_Type, GLfloat* a_Buffer )
		{
			glFeedbackBuffer( a_Size, a_Type, a_Buffer );
		}

		static FORCEINLINE void SelectBuffer( GLsizei a_Size, GLuint* a_Buffer )
		{
			glSelectBuffer( a_Size, a_Buffer );
		}

		static FORCEINLINE GLint RenderMode( GLenum a_Mode )
		{
			return glRenderMode( a_Mode );
		}

		static FORCEINLINE void InitNames()
		{
			glInitNames();
		}

		static FORCEINLINE void LoadName( GLuint a_Name )
		{
			glLoadName( a_Name );
		}

		static FORCEINLINE void PassThrough( GLfloat a_PassThroughValue )
		{
			glPassThrough( a_PassThroughValue );
		}

		static FORCEINLINE void PopName()
		{
			glPopName();
		}

		static FORCEINLINE void PushName( GLuint a_Name )
		{
			glPushName( a_Name );
		}

		static FORCEINLINE void ClearAccum( GLfloat a_Red, GLfloat a_Green, GLfloat a_Blue, GLfloat a_Alpha )
		{
			glClearAccum( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void ClearIndex( GLfloat a_C )
		{
			glClearIndex( a_C );
		}

		static FORCEINLINE void IndexMask( GLuint a_Mask )
		{
			glIndexMask( a_Mask );
		}

		static FORCEINLINE void Accum( GLenum a_Operation, GLfloat a_Value )
		{
			glAccum( a_Operation, a_Value );
		}

		static FORCEINLINE void PopAttrib()
		{
			glPopAttrib();
		}

		static FORCEINLINE void PushAttrib( GLbitfield a_Mask )
		{
			glPushAttrib( a_Mask );
		}

		static FORCEINLINE void Map1d( GLenum a_Target, GLdouble a_U1, GLdouble a_U2, GLint a_Stride, GLint a_Order, const GLdouble* a_Points )
		{
			glMap1d( a_Target, a_U1, a_U2, a_Stride, a_Order, a_Points );
		}


		static FORCEINLINE void Map1f( GLenum a_Target, GLfloat a_U1, GLfloat a_U2, GLint a_Stride, GLint a_Order, const GLfloat* a_Points )
		{
			glMap1f( a_Target, a_U1, a_U2, a_Stride, a_Order, a_Points );
		}

		static FORCEINLINE void Map2d( GLenum a_Target, GLdouble a_U1, GLdouble a_U2, GLint a_Ustride, GLint a_Uorder, GLdouble a_V1, GLdouble a_V2, GLint a_Vstride, GLint a_Vorder, const GLdouble* a_Points )
		{
			glMap2d( a_Target, a_U1, a_U2, a_Ustride, a_Uorder, a_V1, a_V2, a_Vstride, a_Vorder, a_Points );
		}

		static FORCEINLINE void Map2f( GLenum a_Target, GLfloat a_U1, GLfloat a_U2, GLint a_Ustride, GLint a_Uorder, GLfloat a_V1, GLfloat a_V2, GLint a_Vstride, GLint a_Vorder, const GLfloat* a_Points )
		{
			glMap2f( a_Target, a_U1, a_U2, a_Ustride, a_Uorder, a_V1, a_V2, a_Vstride, a_Vorder, a_Points );
		}

		static FORCEINLINE void MapGrid1d( GLint a_Un, GLdouble a_U1, GLdouble a_U2 )
		{
			glMapGrid1d( a_Un, a_U1, a_U2 );
		}

		static FORCEINLINE void MapGrid1f( GLint a_Un, GLfloat a_U1, GLfloat a_U2 )
		{
			glMapGrid1f( a_Un, a_U1, a_U2 );
		}

		static FORCEINLINE void MapGrid2d( GLint a_Un, GLdouble a_U1, GLdouble a_U2, GLint a_Vn, GLdouble a_V1, GLdouble a_V2 )
		{
			glMapGrid2d( a_Un, a_U1, a_U2, a_Vn, a_V1, a_V2 );
		}

		static FORCEINLINE void MapGrid2f( GLint a_Un, GLfloat a_U1, GLfloat a_U2, GLint a_Vn, GLfloat a_V1, GLfloat a_V2 )
		{
			glMapGrid2f( a_Un, a_U1, a_U2, a_Vn, a_V1, a_V2 );
		}

		static FORCEINLINE void EvalCoord1d( GLdouble a_U )
		{
			glEvalCoord1d( a_U );
		}

		static FORCEINLINE void EvalCoord1dv( const GLdouble* a_U )
		{
			glEvalCoord1dv( a_U );
		}

		static FORCEINLINE void EvalCoord1f( GLfloat a_U )
		{
			glEvalCoord1f( a_U );
		}

		static FORCEINLINE void EvalCoord1fv( const GLfloat* a_U )
		{
			glEvalCoord1fv( a_U );
		}

		static FORCEINLINE void EvalCoord2d( GLdouble a_U, GLdouble a_V )
		{
			glEvalCoord2d( a_U, a_V );
		}

		static FORCEINLINE void EvalCoord2dv( const GLdouble* a_U )
		{
			glEvalCoord2dv( a_U );
		}

		static FORCEINLINE void EvalCoord2f( GLfloat a_U, GLfloat a_V )
		{
			glEvalCoord2f( a_U, a_V );
		}

		static FORCEINLINE void EvalCoord2fv( const GLfloat* a_U )
		{
			glEvalCoord2fv( a_U );
		}

		static FORCEINLINE void EvalMesh1( GLenum a_Mode, GLint a_I1, GLint a_I2 )
		{
			glEvalMesh1( a_Mode, a_I1, a_I2 );
		}

		static FORCEINLINE void EvalPoint1( GLint a_I )
		{
			glEvalPoint1( a_I );
		}

		static FORCEINLINE void EvalMesh2( GLenum a_Mode, GLint a_I1, GLint a_I2, GLint a_J1, GLint a_J2 )
		{
			glEvalMesh2( a_Mode, a_I1, a_I2, a_J1, a_J2 );
		}

		static FORCEINLINE void EvalPoint2( GLint a_I, GLint a_J )
		{
			glEvalPoint2( a_I, a_J );
		}

		static FORCEINLINE void AlphaFunc( GLenum a_Func, GLclampf a_Ref )
		{
			glAlphaFunc( a_Func, a_Ref );
		}

		static FORCEINLINE void PixelZoom( GLfloat a_Xfactor, GLfloat a_Yfactor )
		{
			glPixelZoom( a_Xfactor, a_Yfactor );
		}

		static FORCEINLINE void PixelTransferf( GLenum a_PName, GLfloat a_Param )
		{
			glPixelTransferf( a_PName, a_Param );
		}

		static FORCEINLINE void PixelTransferi( GLenum a_PName, GLint a_Param )
		{
			glPixelTransferi( a_PName, a_Param );
		}

		static FORCEINLINE void PixelMapfv( GLenum a_Map, GLsizei a_Mapsize, const GLfloat* a_Values )
		{
			glPixelMapfv( a_Map, a_Mapsize, a_Values );
		}

		static FORCEINLINE void PixelMapuiv( GLenum a_Map, GLsizei a_Mapsize, const GLuint* a_Values )
		{
			glPixelMapuiv( a_Map, a_Mapsize, a_Values );
		}

		static FORCEINLINE void PixelMapusv( GLenum a_Map, GLsizei a_Mapsize, const GLushort* a_Values )
		{
			glPixelMapusv( a_Map, a_Mapsize, a_Values );
		}

		static FORCEINLINE void CopyPixels( GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height, GLenum a_Type )
		{
			glCopyPixels( a_X, a_Y, a_Width, a_Height, a_Type );
		}

		static FORCEINLINE void DrawPixels( GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glDrawPixels( a_Width, a_Height, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void GetClipPlane( GLenum a_Plane, GLdouble* a_Equation )
		{
			glGetClipPlane( a_Plane, a_Equation );
		}

		static FORCEINLINE void GetLightfv( GLenum a_Light, GLenum a_PName, GLfloat* a_Params )
		{
			glGetLightfv( a_Light, a_PName, a_Params );
		}

		static FORCEINLINE void GetLightiv( GLenum a_Light, GLenum a_PName, GLint* a_Params )
		{
			glGetLightiv( a_Light, a_PName, a_Params );
		}

		static FORCEINLINE void GetMapdv( GLenum a_Target, GLenum a_Query, GLdouble* a_V )
		{
			glGetMapdv( a_Target, a_Query, a_V );
		}

		static FORCEINLINE void GetMapfv( GLenum a_Target, GLenum a_Query, GLfloat* a_V )
		{
			glGetMapfv( a_Target, a_Query, a_V );
		}

		static FORCEINLINE void GetMapiv( GLenum a_Target, GLenum a_Query, GLint* a_V )
		{
			glGetMapiv( a_Target, a_Query, a_V );
		}

		static FORCEINLINE void GetMaterialfv( GLenum a_Face, GLenum a_PName, GLfloat* a_Params )
		{
			glGetMaterialfv( a_Face, a_PName, a_Params );
		}

		static FORCEINLINE void GetMaterialiv( GLenum a_Face, GLenum a_PName, GLint* a_Params )
		{
			glGetMaterialiv( a_Face, a_PName, a_Params );
		}

		static FORCEINLINE void GetPixelMapfv( GLenum a_Map, GLfloat* a_Values )
		{
			glGetPixelMapfv( a_Map, a_Values );
		}

		static FORCEINLINE void GetPixelMapuiv( GLenum a_Map, GLuint* a_Values )
		{
			glGetPixelMapuiv( a_Map, a_Values );
		}

		static FORCEINLINE void GetPixelMapusv( GLenum a_Map, GLushort* a_Values )
		{
			glGetPixelMapusv( a_Map, a_Values );
		}

		static FORCEINLINE void GetPolygonStipple( GLubyte* a_Mask )
		{
			glGetPolygonStipple( a_Mask );
		}

		static FORCEINLINE void GetTexEnvfv( GLenum a_Target, GLenum a_PName, GLfloat* a_Params )
		{
			glGetTexEnvfv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexEnviv( GLenum a_Target, GLenum a_PName, GLint* a_Params )
		{
			glGetTexEnviv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexGendv( GLenum a_Coord, GLenum a_PName, GLdouble* a_Params )
		{
			glGetTexGendv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexGenfv( GLenum a_Coord, GLenum a_PName, GLfloat* a_Params )
		{
			glGetTexGenfv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void GetTexGeniv( GLenum a_Coord, GLenum a_PName, GLint* a_Params )
		{
			glGetTexGeniv( a_Coord, a_PName, a_Params );
		}

		static FORCEINLINE void IsList( GLuint a_List )
		{
			glIsList( a_List );
		}

		static FORCEINLINE void Frustum( GLdouble a_Left, GLdouble a_Right, GLdouble a_Bottom, GLdouble a_Top, GLdouble a_Near, GLdouble a_Far )
		{
			glFrustum( a_Left, a_Right, a_Bottom, a_Top, a_Near, a_Far );
		}

		static FORCEINLINE void LoadIdentity()
		{
			glLoadIdentity();
		}

		static FORCEINLINE void LoadMatrixf( const GLfloat* a_M )
		{
			glLoadMatrixf( a_M );
		}

		static FORCEINLINE void LoadMatrixd( const GLdouble* a_M )
		{
			glLoadMatrixd( a_M );
		}

		static FORCEINLINE void MatrixMode( GLenum a_Mode )
		{
			glMatrixMode( a_Mode );
		}

		static FORCEINLINE void MultMatrixf( const GLfloat* a_M )
		{
			glMultMatrixf( a_M );
		}

		static FORCEINLINE void MultMatrixd( const GLdouble* a_M )
		{
			glMultMatrixd( a_M );
		}

		static FORCEINLINE void Ortho( GLdouble a_Left, GLdouble a_Right, GLdouble a_Bottom, GLdouble a_Top, GLdouble a_Near, GLdouble a_Far )
		{
			glOrtho( a_Left, a_Right, a_Bottom, a_Top, a_Near, a_Far );
		}

		static FORCEINLINE void PopMatrix()
		{
			glPopMatrix();
		}

		static FORCEINLINE void PushMatrix()
		{
			glPushMatrix();
		}

		static FORCEINLINE void Rotated( GLdouble a_Angle, GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glRotated( a_Angle, a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Rotatef( GLfloat a_Angle, GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glRotatef( a_Angle, a_X, a_Y, a_Z );
		}
			
		static FORCEINLINE void Scaled( GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glScaled( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Scalef( GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glScalef( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Translated( GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glTranslated( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void Translatef( GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glTranslatef( a_X, a_Y, a_Z );
		}

	#pragma endregion

	#pragma region OpenGL 1.1

		static FORCEINLINE void DrawArrays( GLenum a_Mode, GLint a_First, GLsizei a_Count )
		{
			glDrawArrays( a_Mode, a_First, a_Count );
		}

		static FORCEINLINE void DrawElements( GLenum a_Mode, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices )
		{
			glDrawElements( a_Mode, a_Count, a_Type, a_Indices );
		}

		static FORCEINLINE void GetPointerv( GLenum a_PName, GLvoid** a_Params )
		{
			glGetPointerv( a_PName, a_Params );
		}

		static FORCEINLINE void PolygonOffset( GLfloat a_Factor, GLfloat a_Units )
		{
			glPolygonOffset( a_Factor, a_Units );
		}

		static FORCEINLINE void CopyTexImage1D( GLenum a_Target, GLint a_Level, GLenum a_InternalFormat, GLint a_X, GLint a_Y, GLsizei a_Width, GLint a_Border )
		{
			glCopyTexImage1D( a_Target, a_Level, a_InternalFormat, a_X, a_Y, a_Width, a_Border );
		}

		static FORCEINLINE void CopyTexImage2D( GLenum a_Target, GLint a_Level, GLenum a_InternalFormat, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height, GLint a_Border )
		{
			glCopyTexImage2D( a_Target, a_Level, a_InternalFormat, a_X, a_Y, a_Width, a_Height, a_Border );
		}

		static FORCEINLINE void CopyTexSubImage1D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_X, GLint a_Y, GLsizei a_Width )
		{
			glCopyTexSubImage1D( a_Target, a_Level, a_Xoffset, a_X, a_Y, a_Width );
		}

		static FORCEINLINE void CopyTexSubImage2D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glCopyTexSubImage2D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_X, a_Y, a_Width, a_Height );
		}

		static FORCEINLINE void TexSubImage1D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLsizei a_Width, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTexSubImage1D( a_Target, a_Level, a_Xoffset, a_Width, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void TexSubImage2D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTexSubImage2D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_Width, a_Height, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void BindTexture( GLenum a_Target, GLuint a_Texture )
		{
			glBindTexture( a_Target, a_Texture );
		}

		static FORCEINLINE void DeleteTextures( GLsizei a_N, const GLuint* a_Textures )
		{
			glDeleteTextures( a_N, a_Textures );
		}

		static FORCEINLINE void GenTextures( GLsizei a_N, GLuint* a_Textures )
		{
			glGenTextures( a_N, a_Textures );
		}

		static FORCEINLINE GLboolean IsTexture( GLuint a_Texture )
		{
			return glIsTexture( a_Texture );
		}

		static FORCEINLINE void ArrayElement( GLint a_I )
		{
			glArrayElement( a_I );
		}

		static FORCEINLINE void ColorPointer( GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glColorPointer( a_Size, a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void DisableClientState( GLenum a_Array )
		{
			glDisableClientState( a_Array );
		}

		static FORCEINLINE void EdgeFlagPointer( GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glEdgeFlagPointer( a_Stride, a_Ptr );
		}

		static FORCEINLINE void EnableClientState( GLenum a_Array )
		{
			glEnableClientState( a_Array );
		}

		static FORCEINLINE void IndexPointer( GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glIndexPointer( a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void InterleavedArrays( GLenum a_Format, GLsizei a_Stride, const GLvoid* a_Pointer )
		{
			glInterleavedArrays( a_Format, a_Stride, a_Pointer );
		}

		static FORCEINLINE void NormalPointer( GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glNormalPointer( a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void TexCoordPointer( GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glTexCoordPointer( a_Size, a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void VertexPointer( GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glVertexPointer( a_Size, a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void AreTexturesResident( GLsizei a_N, const GLuint* a_Textures, GLboolean* a_Residences )
		{
			glAreTexturesResident( a_N, a_Textures, a_Residences );
		}

		static FORCEINLINE void PrioritizeTextures( GLsizei a_N, const GLuint* a_Textures, const GLclampf* a_Priorities )
		{
			glPrioritizeTextures( a_N, a_Textures, a_Priorities );
		}

		static FORCEINLINE void Indexub( GLubyte a_C )
		{
			glIndexub( a_C );
		}

		static FORCEINLINE void Indexubv( const GLubyte* a_C )
		{
			glIndexubv( a_C );
		}

		static FORCEINLINE void PopClientAttrib()
		{
			glPopClientAttrib();
		}

		static FORCEINLINE void PushClientAttrib( GLbitfield a_Mask )
		{
			glPushClientAttrib( a_Mask );
		}

	#pragma endregion

	#pragma region OpenGL 1.2

		static FORCEINLINE void DrawRangeElements( GLenum a_Mode, GLuint a_Start, GLuint a_End, GLsizei a_Count, GLenum a_Type, const GLvoid* a_Indices )
		{
			glDrawRangeElements( a_Mode, a_Start, a_End, a_Count, a_Type, a_Indices );
		}

		static FORCEINLINE void TexImage3D( GLenum a_Target, GLint a_Level, GLint a_InternalFormat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLint a_Border, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTexImage3D( a_Target, a_Level, a_InternalFormat, a_Width, a_Height, a_Depth, a_Border, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void TexSubImage3D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLenum a_Type, const GLvoid* a_Pixels )
		{
			glTexSubImage3D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_Type, a_Pixels );
		}

		static FORCEINLINE void CopyTexSubImage3D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLint a_X, GLint a_Y, GLsizei a_Width, GLsizei a_Height )
		{
			glCopyTexSubImage3D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_X, a_Y, a_Width, a_Height );
		}

	#pragma endregion

	#pragma region OpenGL 1.3

		static FORCEINLINE void ActiveTexture( GLenum a_Texture )
		{
			glActiveTexture( a_Texture );
		}

		static FORCEINLINE void SampleCoverage( GLclampf a_Value, GLboolean a_Invert )
		{
			glSampleCoverage( a_Value, a_Invert );
		}

		static FORCEINLINE void CompressedTexImage3D( GLenum a_Target, GLint a_Level, GLenum a_InternalFormat, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLint a_Border, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexImage3D( a_Target, a_Level, a_InternalFormat, a_Width, a_Height, a_Depth, a_Border, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTexImage2D( GLenum a_Target, GLint a_Level, GLenum a_InternalFormat, GLsizei a_Width, GLsizei a_Height, GLint a_Border, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexImage2D( a_Target, a_Level, a_InternalFormat, a_Width, a_Height, a_Border, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTexImage1D( GLenum a_Target, GLint a_Level, GLenum a_InternalFormat, GLsizei a_Width, GLint a_Border, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexImage1D( a_Target, a_Level, a_InternalFormat, a_Width, a_Border, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTexSubImage3D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLint a_Zoffset, GLsizei a_Width, GLsizei a_Height, GLsizei a_Depth, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexSubImage3D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_Zoffset, a_Width, a_Height, a_Depth, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTexSubImage2D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLint a_Yoffset, GLsizei a_Width, GLsizei a_Height, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexSubImage2D( a_Target, a_Level, a_Xoffset, a_Yoffset, a_Width, a_Height, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void CompressedTexSubImage1D( GLenum a_Target, GLint a_Level, GLint a_Xoffset, GLsizei a_Width, GLenum a_Format, GLsizei a_ImageSize, const GLvoid* a_Data )
		{
			glCompressedTexSubImage1D( a_Target, a_Level, a_Xoffset, a_Width, a_Format, a_ImageSize, a_Data );
		}

		static FORCEINLINE void GetCompressedTexImage( GLenum a_Target, GLint a_Level, GLvoid* a_Img )
		{
			glGetCompressedTexImage( a_Target, a_Level, a_Img );
		}

		static FORCEINLINE void ClientActiveTexture( GLenum a_Texture )
		{
			glClientActiveTexture( a_Texture );
		}

		static FORCEINLINE void MultiTexCoord1d( GLenum a_Target, GLdouble a_S )
		{
			glMultiTexCoord1d( a_Target, a_S );
		}

		static FORCEINLINE void MultiTexCoord1dv( GLenum a_Target, const GLdouble* a_V )
		{
			glMultiTexCoord1dv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord1f( GLenum a_Target, GLfloat a_S )
		{
			glMultiTexCoord1f( a_Target, a_S );
		}

		static FORCEINLINE void MultiTexCoord1fv( GLenum a_Target, const GLfloat* a_V )
		{
			glMultiTexCoord1fv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord1i( GLenum a_Target, GLint a_S )
		{
			glMultiTexCoord1i( a_Target, a_S );
		}

		static FORCEINLINE void MultiTexCoord1iv( GLenum a_Target, const GLint* a_V )
		{
			glMultiTexCoord1iv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord1s( GLenum a_Target, GLshort a_S )
		{
			glMultiTexCoord1s( a_Target, a_S );
		}

		static FORCEINLINE void MultiTexCoord1sv( GLenum a_Target, const GLshort* a_V )
		{
			glMultiTexCoord1sv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord2d( GLenum a_Target, GLdouble a_S, GLdouble a_T )
		{
			glMultiTexCoord2d( a_Target, a_S, a_T );
		}

		static FORCEINLINE void MultiTexCoord2dv( GLenum a_Target, const GLdouble* a_V )
		{
			glMultiTexCoord2dv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord2f( GLenum a_Target, GLfloat a_S, GLfloat a_T )
		{
			glMultiTexCoord2f( a_Target, a_S, a_T );
		}

		static FORCEINLINE void MultiTexCoord2fv( GLenum a_Target, const GLfloat* a_V )
		{
			glMultiTexCoord2fv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord2i( GLenum a_Target, GLint a_S, GLint a_T )
		{
			glMultiTexCoord2i( a_Target, a_S, a_T );
		}

		static FORCEINLINE void MultiTexCoord2iv( GLenum a_Target, const GLint* a_V )
		{
			glMultiTexCoord2iv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord2s( GLenum a_Target, GLshort a_S, GLshort a_T )
		{
			glMultiTexCoord2s( a_Target, a_S, a_T );
		}

		static FORCEINLINE void MultiTexCoord2sv( GLenum a_Target, const GLshort* a_V )
		{
			glMultiTexCoord2sv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord3d( GLenum a_Target, GLdouble a_S, GLdouble a_T, GLdouble a_R )
		{
			glMultiTexCoord3d( a_Target, a_S, a_T, a_R );
		}

		static FORCEINLINE void MultiTexCoord3dv( GLenum a_Target, const GLdouble* a_V )
		{
			glMultiTexCoord3dv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord3f( GLenum a_Target, GLfloat a_S, GLfloat a_T, GLfloat a_R )
		{
			glMultiTexCoord3f( a_Target, a_S, a_T, a_R );
		}

		static FORCEINLINE void MultiTexCoord3fv( GLenum a_Target, const GLfloat* a_V )
		{
			glMultiTexCoord3fv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord3i( GLenum a_Target, GLint a_S, GLint a_T, GLint a_R )
		{
			glMultiTexCoord3i( a_Target, a_S, a_T, a_R );
		}

		static FORCEINLINE void MultiTexCoord3iv( GLenum a_Target, const GLint* a_V )
		{
			glMultiTexCoord3iv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord3s( GLenum a_Target, GLshort a_S, GLshort a_T, GLshort a_R )
		{
			glMultiTexCoord3s( a_Target, a_S, a_T, a_R );
		}

		static FORCEINLINE void MultiTexCoord3sv( GLenum a_Target, const GLshort* a_V )
		{
			glMultiTexCoord3sv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord4d( GLenum a_Target, GLdouble a_S, GLdouble a_T, GLdouble a_R, GLdouble a_Q )
		{
			glMultiTexCoord4d( a_Target, a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void MultiTexCoord4dv( GLenum a_Target, const GLdouble* a_V )
		{
			glMultiTexCoord4dv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord4f( GLenum a_Target, GLfloat a_S, GLfloat a_T, GLfloat a_R, GLfloat a_Q )
		{
			glMultiTexCoord4f( a_Target, a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void MultiTexCoord4fv( GLenum a_Target, const GLfloat* a_V )
		{
			glMultiTexCoord4fv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord4i( GLenum a_Target, GLint a_S, GLint a_T, GLint a_R, GLint a_Q )
		{
			glMultiTexCoord4i( a_Target, a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void MultiTexCoord4iv( GLenum a_Target, const GLint* a_V )
		{
			glMultiTexCoord4iv( a_Target, a_V );
		}

		static FORCEINLINE void MultiTexCoord4s( GLenum a_Target, GLshort a_S, GLshort a_T, GLshort a_R, GLshort a_Q )
		{
			glMultiTexCoord4s( a_Target, a_S, a_T, a_R, a_Q );
		}

		static FORCEINLINE void MultiTexCoord4sv( GLenum a_Target, const GLshort* a_V )
		{
			glMultiTexCoord4sv( a_Target, a_V );
		}

		static FORCEINLINE void LoadTransposeMatrixf( const GLfloat* a_M )
		{
			glLoadTransposeMatrixf( a_M );
		}

		static FORCEINLINE void LoadTransposeMatrixd( const GLdouble* a_M )
		{
			glLoadTransposeMatrixd( a_M );
		}

		static FORCEINLINE void MultTransposeMatrixf( const GLfloat* a_M )
		{
			glMultTransposeMatrixf( a_M );
		}

		static FORCEINLINE void MultTransposeMatrixd( const GLdouble* a_M )
		{
			glMultTransposeMatrixd( a_M );
		}

	#pragma endregion

	#pragma region OpenGL 1.4

		static FORCEINLINE void BlendFuncSeparate( GLenum a_SrcRGB, GLenum a_DstRGB, GLenum a_SrcAlpha, GLenum a_DstAlpha )
		{
			glBlendFuncSeparate( a_SrcRGB, a_DstRGB, a_SrcAlpha, a_DstAlpha );
		}

		static FORCEINLINE void MultiDrawArrays( GLenum a_Mode, const GLint* a_First, const GLsizei* a_Count, GLsizei a_Drawcount )
		{
			glMultiDrawArrays( a_Mode, a_First, a_Count, a_Drawcount );
		}

		static FORCEINLINE void MultiDrawElements( GLenum a_Mode, const GLsizei* a_Count, GLenum a_Type, const GLvoid* const* a_Indices, GLsizei a_Drawcount )
		{
			glMultiDrawElements( a_Mode, a_Count, a_Type, a_Indices, a_Drawcount );
		}

		static FORCEINLINE void PointParameterf( GLenum a_PName, GLfloat a_Param )
		{
			glPointParameterf( a_PName, a_Param );
		}

		static FORCEINLINE void PointParameterfv( GLenum a_PName, const GLfloat* a_Params )
		{
			glPointParameterfv( a_PName, a_Params );
		}

		static FORCEINLINE void PointParameteri( GLenum a_PName, GLint a_Param )
		{
			glPointParameteri( a_PName, a_Param );
		}

		static FORCEINLINE void PointParameteriv( GLenum a_PName, const GLint* a_Params )
		{
			glPointParameteriv( a_PName, a_Params );
		}

		static FORCEINLINE void FogCoordf( GLfloat a_Coord )
		{
			glFogCoordf( a_Coord );
		}

		static FORCEINLINE void FogCoordfv( const GLfloat* a_Coord )
		{
			glFogCoordfv( a_Coord );
		}

		static FORCEINLINE void FogCoordd( GLdouble a_Coord )
		{
			glFogCoordd( a_Coord );
		}

		static FORCEINLINE void FogCoorddv( const GLdouble* a_Coord )
		{
			glFogCoorddv( a_Coord );
		}

		static FORCEINLINE void FogCoordPointer( GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glFogCoordPointer( a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void SecondaryColor3b( GLbyte a_Red, GLbyte a_Green, GLbyte a_Blue )
		{
			glSecondaryColor3b( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3bv( const GLbyte* a_V )
		{
			glSecondaryColor3bv( a_V );
		}

		static FORCEINLINE void SecondaryColor3d( GLdouble a_Red, GLdouble a_Green, GLdouble a_Blue )
		{
			glSecondaryColor3d( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3dv( const GLdouble* a_V )
		{
			glSecondaryColor3dv( a_V );
		}

		static FORCEINLINE void SecondaryColor3f( GLfloat a_Red, GLfloat a_Green, GLfloat a_Blue )
		{
			glSecondaryColor3f( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3fv( const GLfloat* a_V )
		{
			glSecondaryColor3fv( a_V );
		}

		static FORCEINLINE void SecondaryColor3i( GLint a_Red, GLint a_Green, GLint a_Blue )
		{
			glSecondaryColor3i( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3iv( const GLint* a_V )
		{
			glSecondaryColor3iv( a_V );
		}

		static FORCEINLINE void SecondaryColor3s( GLshort a_Red, GLshort a_Green, GLshort a_Blue )
		{
			glSecondaryColor3s( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3sv( const GLshort* a_V )
		{
			glSecondaryColor3sv( a_V );
		}

		static FORCEINLINE void SecondaryColor3ub( GLubyte a_Red, GLubyte a_Green, GLubyte a_Blue )
		{
			glSecondaryColor3ub( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3ubv( const GLubyte* a_V )
		{
			glSecondaryColor3ubv( a_V );
		}

		static FORCEINLINE void SecondaryColor3ui( GLuint a_Red, GLuint a_Green, GLuint a_Blue )
		{
			glSecondaryColor3ui( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3uiv( const GLuint* a_V )
		{
			glSecondaryColor3uiv( a_V );
		}

		static FORCEINLINE void SecondaryColor3us( GLushort a_Red, GLushort a_Green, GLushort a_Blue )
		{
			glSecondaryColor3us( a_Red, a_Green, a_Blue );
		}

		static FORCEINLINE void SecondaryColor3usv( const GLushort* a_V )
		{
			glSecondaryColor3usv( a_V );
		}

		static FORCEINLINE void SecondaryColorPointer( GLint a_Size, GLenum a_Type, GLsizei a_Stride, const GLvoid* a_Ptr )
		{
			glSecondaryColorPointer( a_Size, a_Type, a_Stride, a_Ptr );
		}

		static FORCEINLINE void WindowPos2d( GLdouble a_X, GLdouble a_Y )
		{
			glWindowPos2d( a_X, a_Y );
		}

		static FORCEINLINE void WindowPos2dv( const GLdouble* a_V )
		{
			glWindowPos2dv( a_V );
		}

		static FORCEINLINE void WindowPos2f( GLfloat a_X, GLfloat a_Y )
		{
			glWindowPos2f( a_X, a_Y );
		}

		static FORCEINLINE void WindowPos2fv( const GLfloat* a_V )
		{
			glWindowPos2fv( a_V );
		}

		static FORCEINLINE void WindowPos2i( GLint a_X, GLint a_Y )
		{
			glWindowPos2i( a_X, a_Y );
		}

		static FORCEINLINE void WindowPos2iv( const GLint* a_V )
		{
			glWindowPos2iv( a_V );
		}

		static FORCEINLINE void WindowPos2s( GLshort a_X, GLshort a_Y )
		{
			glWindowPos2s( a_X, a_Y );
		}

		static FORCEINLINE void WindowPos2sv( const GLshort* a_V )
		{
			glWindowPos2sv( a_V );
		}

		static FORCEINLINE void WindowPos3d( GLdouble a_X, GLdouble a_Y, GLdouble a_Z )
		{
			glWindowPos3d( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void WindowPos3dv( const GLdouble* a_V )
		{
			glWindowPos3dv( a_V );
		}

		static FORCEINLINE void WindowPos3f( GLfloat a_X, GLfloat a_Y, GLfloat a_Z )
		{
			glWindowPos3f( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void WindowPos3fv( const GLfloat* a_V )
		{
			glWindowPos3fv( a_V );
		}

		static FORCEINLINE void WindowPos3i( GLint a_X, GLint a_Y, GLint a_Z )
		{
			glWindowPos3i( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void WindowPos3iv( const GLint* a_V )
		{
			glWindowPos3iv( a_V );
		}

		static FORCEINLINE void WindowPos3s( GLshort a_X, GLshort a_Y, GLshort a_Z )
		{
			glWindowPos3s( a_X, a_Y, a_Z );
		}

		static FORCEINLINE void WindowPos3sv( const GLshort* a_V )
		{
			glWindowPos3sv( a_V );
		}

		static FORCEINLINE void BlendColor( GLclampf a_Red, GLclampf a_Green, GLclampf a_Blue, GLclampf a_Alpha )
		{
			glBlendColor( a_Red, a_Green, a_Blue, a_Alpha );
		}

		static FORCEINLINE void BlendEquation( GLenum a_Mode )
		{
			glBlendEquation( a_Mode );
		}

	#pragma endregion

	#pragma region OpenGL 1.5

		static FORCEINLINE void GenQueries( GLsizei a_N, GLuint* a_IDs )
		{
			glGenQueries( a_N, a_IDs );
		}

		static FORCEINLINE void DeleteQueries( GLsizei a_N, const GLuint* a_IDs )
		{
			glDeleteQueries( a_N, a_IDs );
		}

		static FORCEINLINE GLboolean IsQuery( GLuint a_ID )
		{
			return glIsQuery( a_ID );
		}

		static FORCEINLINE void BeginQuery( GLenum a_Target, GLuint a_ID )
		{
			glBeginQuery( a_Target, a_ID );
		}

		static FORCEINLINE void EndQuery( GLenum a_Target )
		{
			glEndQuery( a_Target );
		}

		static FORCEINLINE void GetQueryiv( GLenum a_Target, GLenum a_PName, GLint* a_Params )
		{
			glGetQueryiv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetQueryObjectiv( GLuint a_ID, GLenum a_PName, GLint* a_Params )
		{
			glGetQueryObjectiv( a_ID, a_PName, a_Params );
		}

		static FORCEINLINE void GetQueryObjectuiv( GLuint a_ID, GLenum a_PName, GLuint* a_Params )
		{
			glGetQueryObjectuiv( a_ID, a_PName, a_Params );
		}

		static FORCEINLINE void BindBuffer( GLenum a_Target, GLuint a_Buffer )
		{
			glBindBuffer( a_Target, a_Buffer );
		}

		static FORCEINLINE void DeleteBuffers( GLsizei a_N, const GLuint* a_Buffers )
		{
			glDeleteBuffers( a_N, a_Buffers );
		}

		static FORCEINLINE void GenBuffers( GLsizei a_N, GLuint* a_Buffers )
		{
			glGenBuffers( a_N, a_Buffers );
		}

		static FORCEINLINE GLboolean IsBuffer( GLuint a_Buffer )
		{
			return glIsBuffer( a_Buffer );
		}

		static FORCEINLINE void BufferData( GLenum a_Target, GLsizeiptr a_Size, const GLvoid* a_Data, GLenum a_Usage )
		{
			glBufferData( a_Target, a_Size, a_Data, a_Usage );
		}

		static FORCEINLINE void BufferSubData( GLenum a_Target, GLintptr a_Offset, GLsizeiptr a_Size, const GLvoid* a_Data )
		{
			glBufferSubData( a_Target, a_Offset, a_Size, a_Data );
		}

		static FORCEINLINE void GetBufferSubData( GLenum a_Target, GLintptr a_Offset, GLsizeiptr a_Size, GLvoid* a_Data )
		{
			glGetBufferSubData( a_Target, a_Offset, a_Size, a_Data );
		}

		static FORCEINLINE GLvoid* MapBuffer( GLenum a_Target, GLenum a_Access )
		{
			return glMapBuffer( a_Target, a_Access );
		}

		static FORCEINLINE GLboolean UnmapBuffer( GLenum a_Target )
		{
			return glUnmapBuffer( a_Target );
		}

		static FORCEINLINE void GetBufferParameteriv( GLenum a_Target, GLenum a_PName, GLint* a_Params )
		{
			glGetBufferParameteriv( a_Target, a_PName, a_Params );
		}

		static FORCEINLINE void GetBufferPointerv( GLenum a_Target, GLenum a_PName, GLvoid** a_Params )
		{
			glGetBufferPointerv( a_Target, a_PName, a_Params );
		}

	#pragma endregion

	};

} // namespace Tridium