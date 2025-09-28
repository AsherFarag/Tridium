#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHITexture_OpenGLImpl::RHITexture_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: IRHITexture( a_Device, a_Desc )
	{
		ASSERT( a_Desc.Usage != ERHIUsage::Static || !a_SubResourcesData.empty(),
			"Static textures must be initialized with data!" );


		if ( a_Desc.Dimension != ERHITextureDimension::Texture2D || a_Desc.DepthOrArraySize != 1 )
		{
			TODO( "Only 2D textures are supported!" );
			NOT_IMPLEMENTED;
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
				GLTarget = GL_TEXTURE_1D;
				OpenGL1::BindTexture( GLTarget, TextureObj );
				NOT_IMPLEMENTED;
				break;
			}
			case ERHITextureDimension::Texture2D:
			{
				if ( m_Desc.Samples > 1 )
				{
					GLTarget = GL_TEXTURE_2D_MULTISAMPLE;
					OpenGL1::BindTexture( GLTarget, TextureObj );
					TODO( "Check if multisampling is supported!" );
					// Create a multisampled texture
					OpenGL3::TexImage2DMultisample( GLTarget,
						m_Desc.Samples, GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height, GL_TRUE
					);

					NOT_IMPLEMENTED;
				}
				else
				{
					GLTarget = GL_TEXTURE_2D;
					OpenGL1::BindTexture( GLTarget, TextureObj );
					OpenGL4::TexStorage2D( GLTarget,
						m_Desc.Mips, GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height
					);

					if ( a_SubResourcesData.empty() )
					{
						break;
					}


					// Upload the texture data
					if ( a_SubResourcesData.size() == 1 && m_Desc.Mips > 1 )
					{
						// Upload top mip only
						OpenGL1::TexSubImage2D( GLTarget, 0, 0, 0,
											   a_Desc.Width, a_Desc.Height,
											   GLFormat.Format, GLFormat.Type,
											   a_SubResourcesData[0].Data );

						// Generate the remaining mip levels automatically
						OpenGL3::GenerateMipmap( GLTarget );
					}
					else
					{
						// Upload all provided mip levels as in your current code
						for ( uint32_t mip = 0; mip < m_Desc.Mips; ++mip )
						{
							Box dstBox{
								0, Math::Max( m_Desc.Width >> mip, 1u ),
								0, Math::Max( m_Desc.Height >> mip, 1u )
							};

							OpenGL1::TexSubImage2D( GLTarget, mip,
												   dstBox.MinX, dstBox.MinY,
												   dstBox.Width(), dstBox.Height(),
												   GLFormat.Format, GLFormat.Type,
												   a_SubResourcesData[mip].Data );
						}
					}

				}
				break;
			}
			case ERHITextureDimension::Texture3D:
			{
				GLTarget = GL_TEXTURE_3D;
				OpenGL1::BindTexture( GLTarget, TextureObj );
				NOT_IMPLEMENTED;
				break;
			}
			default:
			{
				NOT_IMPLEMENTED;
				break;
			}
		}

		// Bind default sampler parameters
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_Desc.Mips - 1 );

		TextureObj.SetName( a_Desc.Name );
	}

	bool RHITexture_OpenGLImpl::Release()
	{
		TextureObj.Release();
		GLFormat = {};

		return true;
	}

	//size_t RHITexture_OpenGLImpl::GetSizeInBytes() const
	//{
	//	if ( !Valid() )
	//	{
	//		return 0;
	//	}

	//	GLint prevBinding = 0;
	//	OpenGL1::GetIntegerv( GL_TEXTURE_BINDING_2D, &prevBinding );
	//	OpenGL1::BindTexture( GL_TEXTURE_2D, TextureObj );
	//	GLint size = 0;
	//	OpenGL1::GetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size );
	//	OpenGL1::BindTexture( GL_TEXTURE_2D, prevBinding );
	//	return size;
	//}

} // namespace Tridium::GL