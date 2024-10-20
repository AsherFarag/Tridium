#include "tripch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Tridium {

	namespace Utils {

		ETextureFormat GLDataFormatToTridiumDataFormat( GLenum format )
		{
			switch ( format )
			{
			case GL_R: return ETextureFormat::R8;
			case GL_RG: return ETextureFormat::RG8;
			case GL_RGB: return ETextureFormat::RGB8;
			case GL_RGBA: return ETextureFormat::RGBA8;
			}

			TE_CORE_ASSERT( false );
			return ETextureFormat::None;
		}

		GLenum TridiumDataFormatToGLInternalFormat( ETextureFormat format )
		{
			switch ( format )
			{
			case ETextureFormat::R8: return GL_R8;
			case ETextureFormat::RG8: return GL_RG8;
			case ETextureFormat::RGB8: return GL_RGB8;
			case ETextureFormat::RGBA8: return GL_RGBA8;
			case ETextureFormat::RGB32F: return GL_RGB32F;
			case ETextureFormat::RGBA32F: return GL_RGBA32F;
			case ETextureFormat::SRGB: return GL_SRGB;
			case ETextureFormat::SRGBA: return GL_SRGB_ALPHA;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

		GLenum TridiumDataFormatToGLDataFormat( ETextureFormat format )
		{
			switch ( format )
			{
			case ETextureFormat::R8: return GL_R;
			case ETextureFormat::RG8: return GL_RG;
			case ETextureFormat::RGB8: return GL_RGB;
			case ETextureFormat::RGBA8: return GL_RGBA;
			case ETextureFormat::RGB32F: return GL_RGB;
			case ETextureFormat::RGBA32F: return GL_RGBA;
			case ETextureFormat::SRGB: return GL_RGB;
			case ETextureFormat::SRGBA: return GL_RGBA;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

	}


	//////////////////////////////////////////////////////////////////////////
	// Texture
	//////////////////////////////////////////////////////////////////////////

	OpenGLTexture::OpenGLTexture( const TextureSpecification& a_Specification, void* a_TextureData )
		: m_Specification( a_Specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Utils::TridiumDataFormatToGLDataFormat( m_Specification.TextureFormat );
		m_InternalFormat = Utils::TridiumDataFormatToGLInternalFormat( m_Specification.TextureFormat );

		SetData( a_TextureData, m_Width * m_Height * (uint32_t)Utils::GLDataFormatToTridiumDataFormat( m_DataFormat ) );

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

	//////////////////////////////////////////////////////////////////////////
	// CubeMap
	//////////////////////////////////////////////////////////////////////////

	OpenGLCubeMap::OpenGLCubeMap( const TextureSpecification& a_Specification, void** a_TextureData )
		: m_Specification( a_Specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Utils::TridiumDataFormatToGLDataFormat( m_Specification.TextureFormat );
		m_InternalFormat = Utils::TridiumDataFormatToGLInternalFormat( m_Specification.TextureFormat );

		glGenTextures( 1, &m_RendererID );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_RendererID );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		if ( a_TextureData )
			SetData( a_TextureData, m_Width * m_Height * (uint32_t)Utils::GLDataFormatToTridiumDataFormat( m_DataFormat ) );
	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures( 1, &m_RendererID );
	}

	void OpenGLCubeMap::SetData( void** a_Data, uint32_t a_Size )
	{
		auto type = m_InternalFormat == GL_RGBA32F ? GL_FLOAT : GL_UNSIGNED_BYTE;	
		for ( uint32_t i = 0; i < 6; i++ )
		{
			glTexImage2D( 
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				m_InternalFormat,
				m_Width, m_Height,
				0,
				m_DataFormat,
				type,
				a_Data[i]
			);
		}
	}

	void OpenGLCubeMap::Bind( uint32_t slot ) const
	{
		glBindTextureUnit( slot, m_RendererID );
	}

	void OpenGLCubeMap::Unbind( uint32_t slot ) const
	{
		glBindTextureUnit( slot, 0 );
	}
}