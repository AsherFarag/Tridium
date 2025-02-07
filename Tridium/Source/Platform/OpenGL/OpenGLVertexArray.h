#pragma once
#include <Tridium/Graphics/Rendering/VertexArray.h>

namespace Tridium {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer( const SharedPtr<VertexBuffer>& a_VertexBuffer ) override;
		virtual void SetIndexBuffer( const SharedPtr<IndexBuffer>& a_IndexBuffer ) override;

		virtual const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		virtual const SharedPtr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;

		std::vector<SharedPtr<VertexBuffer>> m_VertexBuffers;
		SharedPtr<IndexBuffer> m_IndexBuffer;
	};

}