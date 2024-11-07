#pragma once
#include "Texture.h"

namespace Tridium {

	enum class EFramebufferTextureFormat
	{
		None = 0,

		// Color
		RGB8,
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
		virtual void BindRead() = 0;
		virtual void BindWrite() = 0;
		virtual void Unbind() = 0;
		virtual void Invalidate() = 0;
		virtual void ClearAttachment( uint32_t a_AttachmentIndex, int value ) = 0;

		virtual void BlitTo( SharedPtr<Framebuffer> a_Target, Vector2 a_SrcMin, Vector2 a_SrcMax, Vector2 a_DstMin, Vector2 a_DstMax, EFramebufferTextureFormat a_BufferMask, ETextureFilter a_Filter ) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel( uint32_t a_AttachmentIndex, int x, int y ) = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual uint32_t GetColorAttachmentID( uint32_t index = 0 ) const = 0;
		virtual uint32_t GetDepthAttachmentID() const = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		TODO( "Probably should remove this and replace it with textures or something." );
		virtual void BindAttatchment( uint32_t a_AttachmentIndex, uint32_t a_Slot = 0 ) = 0;
		virtual void UnbindAttatchment( uint32_t a_Slot = 0 ) = 0;
		virtual void BindDepthAttatchment( uint32_t a_Slot = 0 ) = 0;
		virtual void UnbindDepthAttatchment( uint32_t a_Slot = 0 ) = 0;
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