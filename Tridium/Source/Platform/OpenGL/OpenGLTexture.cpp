#include "tripch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Tridium {

	namespace Utils {

		EDataFormat GLDataFormatToTridiumDataFormat( GLenum format )
		{
			switch ( format )
			{
			case GL_R: return EDataFormat::R8;
			case GL_RG: return EDataFormat::RG8;
			case GL_RGB: return EDataFormat::RGB8;
			case GL_RGBA: return EDataFormat::RGBA8;
			}

			TE_CORE_ASSERT( false );
			return EDataFormat::None;
		}

		GLenum TridiumDataFormatToGLInternalFormat( EDataFormat format )
		{
			switch ( format )
			{
			case EDataFormat::R8: return GL_R8;
			case EDataFormat::RG8: return GL_RG8;
			case EDataFormat::RGB8: return GL_RGB8;
			case EDataFormat::RGBA8: return GL_RGBA8;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

		GLenum TridiumDataFormatToGLDataFormat( EDataFormat format )
		{
			switch ( format )
			{
			case EDataFormat::R8: return GL_R;
			case EDataFormat::RG8: return GL_RG;
			case EDataFormat::RGB8: return GL_RGB;
			case EDataFormat::RGBA8: return GL_RGBA;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

	}

	OpenGLTexture::OpenGLTexture( const TextureSpecification& specification )
		: m_Specification( specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Utils::TridiumDataFormatToGLDataFormat( m_Specification.DataFormat );
		m_InternalFormat = Utils::TridiumDataFormatToGLInternalFormat( m_Specification.DataFormat );

		glCreateTextures( GL_TEXTURE_2D, 1, &m_RendererID );
		glTextureStorage2D( m_RendererID, 1, m_InternalFormat, m_Width, m_Height );

		glTextureParameteri( m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTextureParameteri( m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTextureParameteri( m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures( 1, &m_RendererID );
	}

	void OpenGLTexture::SetData( void* data, uint32_t size )
	{
		uint32_t bpp = (uint32_t)Utils::GLDataFormatToTridiumDataFormat( m_DataFormat );
		TE_CORE_ASSERT( size == m_Width * m_Height * bpp, "Data must be entire texture!" );
		glTextureSubImage2D( m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data );
	}

	void OpenGLTexture::Bind( uint32_t slot ) const
	{
		glBindTextureUnit( slot, m_RendererID );
	}
	void OpenGLTexture::Unbind( uint32_t slot ) const
	{
		glBindTextureUnit( slot, 0 );
	}
}