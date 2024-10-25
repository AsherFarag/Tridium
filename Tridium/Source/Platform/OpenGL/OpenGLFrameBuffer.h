#pragma once
#include <Tridium/Rendering/Framebuffer.h>

namespace Tridium {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer( const FramebufferSpecification & spec );
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize( uint32_t width, uint32_t height ) override;
		virtual int ReadPixel( uint32_t attachmentIndex, int x, int y ) override;

		virtual void ClearAttachment( uint32_t attachmentIndex, int value ) override;

		virtual uint32_t GetColorAttachmentID( uint32_t index = 0 ) const override { TE_CORE_ASSERT( index < m_ColorAttachments.size(), "Attempting to access outside of m_ColorAttachments!"); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = EFramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

	class OpenGLRenderBuffer : public RenderBuffer
	{
	public:
		OpenGLRenderBuffer( uint32_t a_Width, uint32_t a_Height, EFramebufferTextureFormat a_Format );
		virtual ~OpenGLRenderBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize( uint32_t a_Width, uint32_t a_Height ) override;

		virtual uint32_t GetID() const override { return m_RendererID; }
		virtual EFramebufferTextureFormat GetFormat() const override { return m_Format; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }


	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Width = 0, m_Height = 0;
		EFramebufferTextureFormat m_Format;
	};

}