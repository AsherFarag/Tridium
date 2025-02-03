#include "tripch.h"
#include "OpenGLTexture.h"

namespace Tridium {

	bool OpenGLTexture::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );
		if ( desc->DimensionCount != 2 ||
			desc->Layers != 1 ||
			desc->Mips != 1 )
		{
			return false;
		}

		m_Format = RHIToGLTextureFormat( desc->Format );
		if ( !m_Format.IsValid() )
		{
			return false;
		}

		const GLsizei width = (GLsizei)desc->Dimensions[0];
		const GLsizei height = (GLsizei)desc->Dimensions[1];
		const Byte* data = desc->InitialData.data();

		// Generate a texture handle
		glGenTextures( 1, &m_Handle );

		// Create a Cubemap texture
		if ( desc->IsCubeMap )
		{
			glBindTexture( GL_TEXTURE_CUBE_MAP, m_Handle );
			for ( int i = 0; i < 6; ++i )
			{
				glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_Format.InternalFormat, width, height, 0, m_Format.Format, m_Format.Type, data );
			}
			glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		}
		// Create a 2D texture
		else
		{
			glBindTexture( GL_TEXTURE_2D, m_Handle );
			glTexImage2D( GL_TEXTURE_2D, 0, m_Format.InternalFormat, width, height, 0, m_Format.Format, m_Format.Type, data );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		// Check if the handle is valid
		return m_Handle != 0;
	}

	bool OpenGLTexture::Release()
	{
		if ( m_Handle != 0 )
		{
			glDeleteTextures( 1, &m_Handle );
			m_Handle = 0;
		}

		return true;
	}

} // namespace Tridium::GL