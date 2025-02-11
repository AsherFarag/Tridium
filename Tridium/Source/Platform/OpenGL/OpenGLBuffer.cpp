#include "tripch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Tridium {

	// ============================
	// 
	// ========== Vertex ==========
	//
	// ============================

#pragma region Vertex

	OpenGLVertexBufferOld::OpenGLVertexBufferOld( float* a_Verticies, uint32_t size )
	{
		glCreateBuffers( 1, &m_RendererID );
		glBindBuffer( GL_ARRAY_BUFFER, m_RendererID );
		glBufferData( GL_ARRAY_BUFFER, size, a_Verticies, GL_STATIC_DRAW );
	}

	OpenGLVertexBufferOld::~OpenGLVertexBufferOld()
	{
		glDeleteBuffers( 1, &m_RendererID );
	}

	void OpenGLVertexBufferOld::Bind() const
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_RendererID );
	}

	void OpenGLVertexBufferOld::Unbind() const
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

#pragma endregion

	// ===========================
	// 
	// ========== INDEX ==========
	//
	// ===========================
#pragma region Index

	OpenGLIndexBufferOld::OpenGLIndexBufferOld( uint32_t* a_Indicies, uint32_t a_Count )
		: m_Count( a_Count )
	{
		glCreateBuffers( 1, &m_RendererID );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_RendererID );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, a_Count * sizeof( uint32_t ), a_Indicies, GL_STATIC_DRAW );
	}

	OpenGLIndexBufferOld::~OpenGLIndexBufferOld()
	{
		glDeleteBuffers( 1, &m_RendererID );
	}

	void OpenGLIndexBufferOld::Bind() const
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_RendererID );
	}

	void OpenGLIndexBufferOld::Unbind() const
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

#pragma endregion

}
