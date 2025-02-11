#pragma once
#include "Tridium/Graphics/Rendering/Buffer.h"

namespace Tridium {

	class OpenGLVertexBufferOld : public VertexBuffer
	{
	public:
		OpenGLVertexBufferOld( float* a_Verticies, uint32_t size );
		virtual ~OpenGLVertexBufferOld();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout( const BufferLayout& a_Layout ) override { m_Layout = a_Layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBufferOld : public IndexBuffer
	{
	public:
		OpenGLIndexBufferOld( uint32_t* a_Indicies, uint32_t a_Count );
		virtual ~OpenGLIndexBufferOld();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

}