#include "tripch.h"
#include "OpenGLRenderingAPI.h"

#include "glad/glad.h"

namespace Tridium {
	void OpenGLRenderingAPI::Init()
	{
		glEnable( GL_DEPTH_TEST );
	}

	void OpenGLRenderingAPI::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
	{
		glViewport( x, y, width, height );
	}

	void OpenGLRenderingAPI::SetClearColor( const Vector4& color )
	{
		glClearColor( color.r, color.g, color.b, color.a );
	}

	void OpenGLRenderingAPI::Clear()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRenderingAPI::DrawIndexed( const Ref<VertexArray>& a_VertexArray )
	{
		glDrawElements( GL_TRIANGLES, a_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr );
	}

}