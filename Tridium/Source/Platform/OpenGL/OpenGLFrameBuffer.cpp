#include "tripch.h"
#include "OpenGLFramebuffer.h"

#include "glad/glad.h"

namespace Tridium {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Util {

		static auto GLTypeFromFramebufferTextureFormat( EFramebufferTextureFormat format )
		{
			switch ( format )
			{
			case EFramebufferTextureFormat::RGB8:        return GL_UNSIGNED_BYTE;
			case EFramebufferTextureFormat::RGBA8:       return GL_UNSIGNED_BYTE;
			case EFramebufferTextureFormat::RG16F:       return GL_FLOAT;
			case EFramebufferTextureFormat::RGB16F:      return GL_FLOAT;
			case EFramebufferTextureFormat::RGBA16F:     return GL_FLOAT;
			case EFramebufferTextureFormat::RGB32F:      return GL_FLOAT;
			case EFramebufferTextureFormat::RGBA32F:     return GL_FLOAT;
			case EFramebufferTextureFormat::RED_INT:     return GL_INT;
			}

			TE_CORE_ASSERT( false, "Unknown FramebufferTextureFormat!" );
			return 0;
		}

		static EFramebufferTextureFormat GLToTridiumFBTextureFormat( GLenum format )
		{
			switch ( format )
			{
			case GL_RGB8:        return EFramebufferTextureFormat::RGB8;
			case GL_RGBA8:       return EFramebufferTextureFormat::RGBA8;
			case GL_RG16F:       return EFramebufferTextureFormat::RG16F;
			case GL_RGB16F:      return EFramebufferTextureFormat::RGB16F;
			case GL_RGBA16F:     return EFramebufferTextureFormat::RGBA16F;
			case GL_RGB32F:      return EFramebufferTextureFormat::RGB32F;
			case GL_RGBA32F:     return EFramebufferTextureFormat::RGBA32F;
			case GL_R32I: return EFramebufferTextureFormat::RED_INT;
			}

			return EFramebufferTextureFormat::None;
		}


		static GLenum TridiumFBTextureFormatToGL( EFramebufferTextureFormat format )
		{
			switch ( format )
			{
			case EFramebufferTextureFormat::RGB8:        return GL_RGB8;
			case EFramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case EFramebufferTextureFormat::RG16F:       return GL_RG16F;
			case EFramebufferTextureFormat::RGB16F:      return GL_RGB16F;
			case EFramebufferTextureFormat::RGBA16F:     return GL_RGBA16F;
			case EFramebufferTextureFormat::RGB32F:      return GL_RGB32F;
			case EFramebufferTextureFormat::RGBA32F:     return GL_RGBA32F;
			case EFramebufferTextureFormat::RED_INT:	 return GL_R32I;
			}

			return 0;
		}

		static GLenum TridiumFBTextureFormatToGLData( EFramebufferTextureFormat format )
		{
			switch ( format )
			{
			case EFramebufferTextureFormat::RGB8:        return GL_RGB;
			case EFramebufferTextureFormat::RGBA8:       return GL_RGBA;
			case EFramebufferTextureFormat::RG16F:       return GL_RG;
			case EFramebufferTextureFormat::RGB16F:      return GL_RGB;
			case EFramebufferTextureFormat::RGBA16F:     return GL_RGBA;
			case EFramebufferTextureFormat::RGB32F:      return GL_RGB;
			case EFramebufferTextureFormat::RGBA32F:     return GL_RGBA;
			case EFramebufferTextureFormat::RED_INT:     return GL_RED_INTEGER;

			}

			return 0;
		}

		static GLenum TridiumFBTextureTypeToGL( EFramebufferTextureType type )
		{
			switch ( type )
			{
			case EFramebufferTextureType::Texture2D:      return GL_TEXTURE_2D;
			case EFramebufferTextureType::TextureCubeMap: return GL_TEXTURE_CUBE_MAP;
			}

			return 0;
		}

		static GLenum TextureTarget( bool multisampled )
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures( bool multisampled, uint32_t* outID, uint32_t count )
		{
			glCreateTextures( TextureTarget( multisampled ), count, outID );
		}

		static void BindTexture( bool multisampled, uint32_t id )
		{
			glBindTexture( TextureTarget( multisampled ), id );
		}

		static void AttachColorTexture( uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index )
		{
			bool multisampled = samples > 1;
			if ( multisampled )
			{
				glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE );
			}
			else
			{
				int type = GLTypeFromFramebufferTextureFormat( GLToTridiumFBTextureFormat( internalFormat ) );
				glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			}

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget( multisampled ), id, 0 );
		}

		static void AttachDepthTexture( uint32_t id, GLenum textureType, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height )
		{
			bool multisampled = samples > 1;
			if ( multisampled )
			{
				glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE );
			}
			else
			{
				if ( textureType == GL_TEXTURE_2D )
				{
					glTexStorage2D( textureType, 1, format, width, height );
				}


				glTexParameteri( textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				// Fix for shadow mapping
				// Should probably be a parameter
				glTexParameteri( textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );
				glTexParameteri( textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
				glTexParameteri( textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
				float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv( textureType, GL_TEXTURE_BORDER_COLOR, color );
			}

			glBindFramebuffer( GL_FRAMEBUFFER, id );

			if ( textureType == GL_TEXTURE_2D )
			{
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureType, id, 0 );
			}
			else
			{
				glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0 );
			}
		}

		static bool IsDepthFormat( EFramebufferTextureFormat format )
		{
			switch ( format )
			{
				case EFramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer( const FramebufferSpecification& spec ) 
		: m_Specification( spec )
	{
		for ( auto spec : m_Specification.Attachments.Attachments )
		{
			if ( !Util::IsDepthFormat( spec.TextureFormat ) )
				m_ColorAttachmentSpecifications.emplace_back( spec );
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers( 1, &m_RendererID );
		glDeleteTextures( m_ColorAttachments.size(), m_ColorAttachments.data() );
		glDeleteTextures( 1, &m_DepthAttachment );
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if ( m_RendererID )
		{
			glDeleteFramebuffers( 1, &m_RendererID );
			glDeleteTextures( m_ColorAttachments.size(), m_ColorAttachments.data() );
			glDeleteTextures( 1, &m_DepthAttachment );

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers( 1, &m_RendererID );
		glBindFramebuffer( GL_FRAMEBUFFER, m_RendererID );

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if ( m_ColorAttachmentSpecifications.size() )
		{
			m_ColorAttachments.resize( m_ColorAttachmentSpecifications.size() );
			Util::CreateTextures( multisample, m_ColorAttachments.data(), m_ColorAttachments.size() );

			for ( size_t i = 0; i < m_ColorAttachments.size(); i++ )
			{
				Util::BindTexture( multisample, m_ColorAttachments[ i ] );
				Util::AttachColorTexture( 
					m_ColorAttachments[i],
					m_Specification.Samples,
					Util::TridiumFBTextureFormatToGL( m_ColorAttachmentSpecifications[i].TextureFormat ),
					Util::TridiumFBTextureFormatToGLData( m_ColorAttachmentSpecifications[i].TextureFormat ),
					m_Specification.Width, m_Specification.Height, i );
			}
		}

		if ( m_DepthAttachmentSpecification.TextureFormat != EFramebufferTextureFormat::None )
		{
			switch ( m_DepthAttachmentSpecification.TextureType )
			{
			case EFramebufferTextureType::Texture2D:
				Util::CreateTextures( multisample, &m_DepthAttachment, 1 );
				Util::BindTexture( multisample, m_DepthAttachment );
				break;
			case EFramebufferTextureType::TextureCubeMap:
				glGenTextures( 1, &m_DepthAttachment );
				glBindTexture( GL_TEXTURE_CUBE_MAP, m_DepthAttachment );
				for ( int i = 0; i < 6; i++ )
				{
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
				}
				break;
			}

			switch ( m_DepthAttachmentSpecification.TextureFormat )
			{
			case EFramebufferTextureFormat::DEPTH24STENCIL8:
				Util::AttachDepthTexture( m_DepthAttachment, Util::TridiumFBTextureTypeToGL(m_DepthAttachmentSpecification.TextureType), m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height );
				break;
			}
		}

		if ( m_ColorAttachments.size() > 1 )
		{
			TE_CORE_ASSERT( m_ColorAttachments.size() <= 8, "More than 8 color attachments when invalidating this Framebuffer");
			GLenum buffers[ 8 ] =
			{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
			glDrawBuffers( m_ColorAttachments.size(), buffers );
		}

		else if ( m_ColorAttachments.empty() )
		{
			// Only depth-pass
			glDrawBuffer( GL_NONE );
			glReadBuffer( GL_NONE );
		}

		TE_CORE_ASSERT( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!" );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_RendererID );
		glViewport( 0, 0, m_Specification.Width, m_Specification.Height );
	}

	void OpenGLFramebuffer::BindRead()
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_RendererID );
	}

	void OpenGLFramebuffer::BindWrite()
	{
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_RendererID );

	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void OpenGLFramebuffer::Resize( uint32_t width, uint32_t height )
	{
		if ( width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize )
		{
			TE_CORE_WARN( "Attempted to rezize Framebuffer to {0}, {1}", width, height );
			return;
		}

		if ( width == m_Specification.Width && height == m_Specification.Height )
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel( uint32_t attachmentIndex, int x, int y )
	{
		TE_CORE_ASSERT( attachmentIndex < m_ColorAttachments.size(), "attachmentIndex out of m_ColorAttachments scope");

		glReadBuffer( GL_COLOR_ATTACHMENT0 + attachmentIndex );
		int pixelData;
		glReadPixels( x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData );
		return pixelData;

	}

	void OpenGLFramebuffer::ClearAttachment( uint32_t attachmentIndex, int value )
	{
		TE_CORE_ASSERT( attachmentIndex < m_ColorAttachments.size(), "attachmentIndex out of m_ColorAttachments scope" );

		auto& spec = m_ColorAttachmentSpecifications[ attachmentIndex ];
		glClearTexImage( m_ColorAttachments[ attachmentIndex ], 0,
			Util::TridiumFBTextureFormatToGL( spec.TextureFormat ), GL_INT, &value );
	}

	void OpenGLFramebuffer::BlitTo( SharedPtr<Framebuffer> target, Vector2 srcMin, Vector2 srcMax, Vector2 dstMin, Vector2 dstMax, EFramebufferTextureFormat bufferMask, ETextureFilter filter )
	{
		GLbitfield mask = Util::IsDepthFormat( bufferMask ) ? GL_DEPTH_BUFFER_BIT : GL_COLOR_BUFFER_BIT;

		TODO( "Not sure if this is correct, fix this later! " );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_RendererID );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, target->GetRendererID() );
		glBlitFramebuffer( srcMin.x, srcMin.y, srcMax.x, srcMax.y, dstMin.x, dstMin.y, dstMax.x, dstMax.y, mask, filter == ETextureFilter::Linear ? GL_LINEAR : GL_NEAREST );
	}

	void OpenGLFramebuffer::BindAttatchment( uint32_t a_AttachmentIndex, uint32_t a_Slot )
	{
		glBindTextureUnit( a_Slot, GetColorAttachmentID( a_AttachmentIndex ) );
	}

	void OpenGLFramebuffer::UnbindAttatchment( uint32_t a_Slot )
	{
		glBindTextureUnit( a_Slot, 0 );
	}

	void OpenGLFramebuffer::BindDepthAttatchment( uint32_t a_Slot )
	{
		glBindTextureUnit( a_Slot, m_DepthAttachment );
	}

	void OpenGLFramebuffer::UnbindDepthAttatchment( uint32_t a_Slot )
	{
		glBindTextureUnit( a_Slot, 0 );
	}

	OpenGLRenderBuffer::OpenGLRenderBuffer( uint32_t a_Width, uint32_t a_Height, EFramebufferTextureFormat a_Format )
		: m_Width( a_Width ), m_Height( a_Height ), m_Format( a_Format )
	{
		Invalidate();
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		glDeleteRenderbuffers( 1, &m_RendererID );
	}

	void OpenGLRenderBuffer::Invalidate()
	{
		TODO( "Set up format!" );
		glGenRenderbuffers( 1, &m_RendererID );
		glBindRenderbuffer( GL_RENDERBUFFER, m_RendererID );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RendererID ); // Attach depth buffer
	}

	void OpenGLRenderBuffer::Bind()
	{
		glBindRenderbuffer( GL_RENDERBUFFER, m_RendererID );
	}

	void OpenGLRenderBuffer::Unbind()
	{
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

	void OpenGLRenderBuffer::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		m_Width = a_Width;
		m_Height = a_Height;

		Invalidate();
	}

}