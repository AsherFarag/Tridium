#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHITexture_OpenGLImpl::RHITexture_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: IRHITexture( a_Device, a_Desc )
	{
		RHI_DEV_CHECK( a_Desc.HeapType != ERHIHeapType::Immutable || !a_SubResourcesData.empty(),
			"Static textures must be initialized with data!" );

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
			case ERHITextureDimension::TextureCube:
			{
				GLTarget = GL_TEXTURE_CUBE_MAP;
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
					// Upload top mip only for all faces
					for ( uint32_t face = 0; face < 6; ++face )
					{
						OpenGL1::TexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0,
											   0, 0,
											   a_Desc.Width, a_Desc.Height,
											   GLFormat.Format, GLFormat.Type,
											   a_SubResourcesData[0].Data );
					}
					// Generate the remaining mip levels automatically
					OpenGL3::GenerateMipmap( GLTarget );
				}
				else
				{
					// Upload all provided mip levels and faces
					for ( uint32_t mip = 0; mip < m_Desc.Mips; ++mip )
					{
						for ( uint32_t face = 0; face < 6; ++face )
						{
							size_t subresourceIndex = mip * 6 + face;
							if ( subresourceIndex >= a_SubResourcesData.size() )
							{
								break;
							}
							Box dstBox{
								0, Math::Max( m_Desc.Width >> mip, 1u ),
								0, Math::Max( m_Desc.Height >> mip, 1u )
							};
							OpenGL1::TexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip,
												   dstBox.MinX, dstBox.MinY,
												   dstBox.Width(), dstBox.Height(),
												   GLFormat.Format, GLFormat.Type,
												   a_SubResourcesData[subresourceIndex].Data );
						}
					}
				}
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

	RHITextureSubresourceData RHITexture_OpenGLImpl::MapSubresource( const RHITextureSlice& a_Slice )
	{
		//RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Only staging textures can be mapped" );

		//const RHITextureSlice slice = a_Slice.Resolve( Desc() );

		//// Each mip or array slice can be represented as a different region in a staging PBO.
		//// Assume you’ve created one PBO per subresource in Create().
		//GLuint pbo = m_SubresourcePBOs[slice.ArraySlice * Desc().Mips + slice.MipLevel];
		//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbo );

		//// Map the buffer to CPU memory
		//GLsizeiptr pboSize = m_SubresourceSizes[slice.ArraySlice * Desc().Mips + slice.MipLevel];
		//void* data = glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, pboSize,
		//	GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );

		//if ( !data )
		//{
		//	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		//	return {};
		//}

		//// Compute row/depth stride based on mip size and format
		//const RHIFormatInfo formatInfo = GetRHIFormatInfo( Desc().Format );
		//const uint32_t mipWidth = std::max( 1u, Desc().Width >> slice.MipLevel );
		//const uint32_t mipHeight = std::max( 1u, Desc().Height >> slice.MipLevel );
		//const uint32_t bytesPerPixel = formatInfo.BytesPerBlock;

		//RHITextureSubresourceData result{};
		//result.Data = data;
		//result.RowStride = mipWidth * bytesPerPixel;
		//result.DepthStride = result.RowStride * mipHeight;

		//return result;

		return {}; TODO( "Implement texture mapping!" );
	}

	void RHITexture_OpenGLImpl::UnmapSubresource( const RHITextureSlice& a_Slice )
	{
		//const RHITextureSlice slice = a_Slice.Resolve( Desc() );
		//GLuint pbo = m_SubresourcePBOs[slice.ArraySlice * Desc().Mips + slice.MipLevel];

		//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbo );
		//glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );

		//// Upload to texture
		//const uint32_t mipWidth = std::max( 1u, Desc().Width >> slice.MipLevel );
		//const uint32_t mipHeight = std::max( 1u, Desc().Height >> slice.MipLevel );

		//GLenum glFormat = GetGLFormat( Desc().Format );
		//GLenum glType = GetGLType( Desc().Format );

		//glBindTexture( GL_TEXTURE_2D, m_TextureID );
		//glTexSubImage2D( GL_TEXTURE_2D, slice.MipLevel, 0, 0, mipWidth, mipHeight, glFormat, glType, 0 );
		//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		TODO( "Implement texture unmapping!" );
	}


} // namespace Tridium::GL