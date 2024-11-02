#include "tripch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Tridium {

	static GLenum ShaderDataTypeToOpenGLBaseType( ShaderDataType type )
	{
		switch ( type )
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:	   return GL_FLOAT;
		case ShaderDataType::Mat4:	   return GL_FLOAT;
		case ShaderDataType::Int:	   return GL_INT;
		case ShaderDataType::Int2:	   return GL_INT;
		case ShaderDataType::Int3:	   return GL_INT;
		case ShaderDataType::Int4:	   return GL_INT;
		case ShaderDataType::Bool:	   return GL_BOOL;
		}

		TE_CORE_ASSERT( false, "Unkown ShaderDataType!" );
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays( 1, &m_RendererID );
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays( 1, &m_RendererID );
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray( m_RendererID );
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray( 0 );
	}

	void OpenGLVertexArray::AddVertexBuffer( const SharedPtr<VertexBuffer>& a_VertexBuffer )
	{
		TE_CORE_ASSERT( a_VertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no Layout!" );

		glBindVertexArray( m_RendererID );
		a_VertexBuffer->Bind();

		const auto& layout = a_VertexBuffer->GetLayout();
		const auto& elements = layout.GetElements();
		for ( uint32_t i = 0; i < elements.size(); ++i )
		{
			const auto& element = elements[ i ];
			glEnableVertexAttribArray( i );
			glVertexAttribPointer( 
				i,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType( element.Type ),
				element.Normalised ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset );
		}

		m_VertexBuffers.push_back( a_VertexBuffer );
	}

	void OpenGLVertexArray::SetIndexBuffer( const SharedPtr<IndexBuffer>& a_IndexBuffer )
	{
		glBindVertexArray( m_RendererID );
		a_IndexBuffer->Bind();

		m_IndexBuffer = a_IndexBuffer;
	}

}