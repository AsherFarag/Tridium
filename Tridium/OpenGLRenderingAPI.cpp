#include "tripch.h"
#include "OpenGLRenderingAPI.h"

#include "glad/glad.h"

void Tridium::OpenGLRenderingAPI::Init()
{
}

void Tridium::OpenGLRenderingAPI::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
	glViewport( x, y, width, height );
}

void Tridium::OpenGLRenderingAPI::SetClearColor( const Vector4& color )
{
	glClearColor( color.r, color.g, color.b, color.a );
}

void Tridium::OpenGLRenderingAPI::Clear()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
