#pragma once

namespace Tridium {

	enum class EFramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INT,
		RG16F,
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification( EFramebufferTextureFormat format )
			: TextureFormat( format ) {}

		EFramebufferTextureFormat TextureFormat = EFramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification( std::initializer_list<FramebufferTextureSpecification> attachments )
			: Attachments( attachments ) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		static SharedPtr<Framebuffer> Create( const FramebufferSpecification& spec );
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Invalidate() = 0;
		virtual void ClearAttachment( uint32_t attachmentIndex, int value ) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel( uint32_t attachmentIndex, int x, int y ) = 0;

		virtual uint32_t GetColorAttachmentID( uint32_t index = 0 ) const = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
	};

	class RenderBuffer
	{
	public:
		static SharedPtr<RenderBuffer> Create( uint32_t a_Width, uint32_t a_Height, EFramebufferTextureFormat a_Format );
		virtual ~RenderBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Invalidate() = 0;

		virtual void Resize( uint32_t a_Width, uint32_t a_Height ) = 0;

		virtual uint32_t GetID() const = 0;
		virtual EFramebufferTextureFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};

}