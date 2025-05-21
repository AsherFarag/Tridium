#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHITexture_OpenGLImpl::RHITexture_OpenGLImpl( const DescriptorType& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: RHITexture( a_Desc )
	{
		ASSERT( a_Desc.Usage != ERHIUsage::Static || !a_SubResourcesData.empty(),
			"Static textures must be initialized with data!" );


		if ( a_Desc.Dimension != ERHITextureDimension::Texture2D || a_Desc.Depth != 1 || a_Desc.Mips != 1 )
		{
			TODO( "Only 2D textures are supported!" );
			return;
		}

		GLFormat = GLTextureFormat::From( a_Desc.Format );
		if ( !ASSERT( GLFormat.Valid(),
			"Invalid texture format!" ) )
		{
			return;
		}

		// Generate a texture handle
		TextureObj.Create();

		switch ( m_Desc.Dimension )
		{
			case ERHITextureDimension::Texture1D:
			{
				OpenGL1::BindTexture( GL_TEXTURE_1D, TextureObj );
				NOT_IMPLEMENTED;
				break;
			}
			case ERHITextureDimension::Texture2D:
			{
				OpenGL1::BindTexture( GL_TEXTURE_2D, TextureObj );

				if ( m_Desc.Samples > 1 )
				{
					TODO( "Check if multisampling is supported!" );
					// Create a multisampled texture
					OpenGL3::TexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE,
						m_Desc.Samples, GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height, GL_TRUE
					);

					NOT_IMPLEMENTED;
				}
				else
				{
					OpenGL4::TexStorage2D( GL_TEXTURE_2D,
						m_Desc.Mips, GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height
					);

					if ( a_SubResourcesData.empty() )
						break;

					// Upload the texture data
					ASSERT( m_Desc.Mips == a_SubResourcesData.size(),
						"Invalid number of subresources!" );

					for ( uint32_t mip = 0; mip < m_Desc.Mips; ++mip )
					{
						Box dstBox{
							0, Math::Max( m_Desc.Width >> mip, 1u ),
							0, Math::Max( m_Desc.Height >> mip, 1u )
						};

						OpenGL1::TexSubImage2D( GL_TEXTURE_2D, mip,
							dstBox.MinX, dstBox.MinY,
							dstBox.Width(), dstBox.Height(),
							GLFormat.Format, GLFormat.Type,
							a_SubResourcesData[mip].Data.data()
						);
					}
				}
				break;
			}
			case ERHITextureDimension::Texture3D:
			{
				OpenGL1::BindTexture( GL_TEXTURE_3D, TextureObj );
				NOT_IMPLEMENTED;
				break;
			}
			default:
			{
				NOT_IMPLEMENTED;
				break;
			}
		}

		TextureObj.SetName( a_Desc.Name );
	}

	bool RHITexture_OpenGLImpl::Release()
	{
		TextureObj.Release();
		GLFormat = {};

		return true;
	}

	size_t RHITexture_OpenGLImpl::GetSizeInBytes() const
	{
		if ( !Valid() )
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