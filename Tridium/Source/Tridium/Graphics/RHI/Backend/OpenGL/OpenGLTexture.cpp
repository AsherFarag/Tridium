#include "tripch.h"
#include "OpenGLTexture.h"

namespace Tridium {

	bool OpenGLTexture::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );
		if ( desc->Dimension != ERHITextureDimension::Texture2D || desc->Layers != 1 || desc->Mips != 1 )
		{
			return false;
		}

		m_Format = RHIToGLTextureFormat( desc->Format );
		if ( !m_Format.IsValid() )
		{
			return false;
		}

		const GLsizei width = (GLsizei)desc->Width;
		const GLsizei height = (GLsizei)desc->Height;
		const Byte* data = desc->InitialData.data();

		if ( IsRenderBuffer() )
		{
			// Create a render buffer object
			OpenGL3::GenRenderbuffers( 1, &m_Handle );
			OpenGL3::BindRenderbuffer( GL_RENDERBUFFER, m_Handle );
			OpenGL3::RenderbufferStorage( GL_RENDERBUFFER, m_Format.InternalFormat, width, height );
			OpenGL3::BindRenderbuffer( GL_RENDERBUFFER, 0 );
		}
		else
		{
			// Generate a texture handle
			OpenGL1::GenTextures( 1, &m_Handle );

			switch ( desc->Dimension )
			{
			case ERHITextureDimension::TextureCube:
			{
				OpenGL1::BindTexture( GL_TEXTURE_CUBE_MAP, m_Handle );
				for ( int i = 0; i < 6; ++i )
				{
					OpenGL1::TexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_Format.InternalFormat, width, height, 0, m_Format.Format, m_Format.Type, data );
				}
				OpenGL1::BindTexture( GL_TEXTURE_CUBE_MAP, 0 );
				break;
			}
			case ERHITextureDimension::Texture2D:
			{
				OpenGL1::BindTexture( GL_TEXTURE_2D, m_Handle );
				OpenGL1::TexImage2D( GL_TEXTURE_2D, 0, m_Format.InternalFormat, width, height, 0, m_Format.Format, m_Format.Type, data );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, desc->Mips - 1 );
				OpenGL1::BindTexture( GL_TEXTURE_2D, 0 );
				break;
			}
			default:
			{
				ASSERT_LOG( false, "Unsupported texture dimension!" );
				return false;
			}
			}
		}

	#if RHI_DEBUG_ENABLED
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			GLenum target = IsRenderBuffer() ? GL_RENDERBUFFER : GL_TEXTURE;
			OpenGL4::ObjectLabel( target, m_Handle, desc->Name.size(), static_cast<const GLchar*>( desc->Name.data() ) );
		}
	#endif

		// Check if the handle is valid
		return m_Handle != 0;
	}

	bool OpenGLTexture::Release()
	{
		if ( m_Handle != 0 )
		{
			IsRenderBuffer() ? OpenGL3::DeleteRenderbuffers( 1, &m_Handle ) : OpenGL1::DeleteTextures( 1, &m_Handle );
			m_Handle = 0;
		}

		return true;
	}

} // namespace Tridium::GL