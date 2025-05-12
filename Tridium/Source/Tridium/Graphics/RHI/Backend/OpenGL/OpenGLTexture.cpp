#include "tripch.h"
#include "OpenGLTexture.h"

namespace Tridium {

	RHITexture_OpenGLImpl::RHITexture_OpenGLImpl( const DescriptorType& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: RHITexture( a_Desc )
	{
		if ( a_Desc.Dimension != ERHITextureDimension::Texture2D || a_Desc.Depth != 1 || a_Desc.Mips != 1 )
		{
			TODO( "Only 2D textures are supported!" );
			return;
		}

		GLFormat = RHIToGLTextureFormat( a_Desc.Format );
		if ( !ASSERT( GLFormat.IsValid(),
			"Invalid texture format!" ) )
		{
			return;
		}

		// Generate a texture handle
		TextureObj.Create();
		OpenGL1::BindTexture( GL_TEXTURE_2D, TextureObj );
		TextureObj.SetName( a_Desc.Name );
		if ( !ASSERT( TextureObj.Valid(),
			"Failed to create texture!" ) )
		{
			return;
		}

		TODO( "This only works for 2D textures and not multiple subresources!" );

		// Set the texture data
		OpenGL4::TextureStorage2D( TextureObj, 1, GLFormat.InternalFormat, a_Desc.Width, a_Desc.Height );
		if ( a_SubResourcesData.size() > 0 )
		{
			OpenGL4::TextureSubImage2D( TextureObj, 0, 0, 0, a_Desc.Width, a_Desc.Height,
				GLFormat.Format, GLFormat.Type, a_SubResourcesData[0].Data.data() );
		}

	}

	bool RHITexture_OpenGLImpl::Release()
	{
		TextureObj.Release();
		GLFormat = {};

		return true;
	}

	size_t RHITexture_OpenGLImpl::GetSizeInBytes() const
	{
		if ( !IsValid() )
		{
			return 0;
		}

		GLint prevBinding = 0;
		OpenGL1::GetIntegerv( GL_TEXTURE_BINDING_2D, &prevBinding );
		OpenGL1::BindTexture( GL_TEXTURE_2D, TextureObj );
		GLint size = 0;
		OpenGL1::GetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size );
		OpenGL1::BindTexture( GL_TEXTURE_2D, prevBinding );
		return size;
	}

} // namespace Tridium::GL