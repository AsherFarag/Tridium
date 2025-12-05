#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHITexture_OpenGLImpl::RHITexture_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: IRHITexture( a_Device, a_Desc )
	{
		RHI_DEV_CHECK( a_Desc.HeapType != ERHIHeapType::Immutable || !a_SubResourcesData.empty(),
			"Static textures must be initialized with data!" );

		m_GLFormat = GLTextureFormat::From( a_Desc.Format );

		if ( !ASSERT( m_GLFormat.Valid(),
			"Invalid texture format!" ) )
		{
			return;
		}

		// Staging textures are a special case as they are not actually textures but buffers.
		if ( m_Desc.HeapType == ERHIHeapType::Staging )
		{
			RHI_DEV_WARN( a_SubResourcesData.empty(),
						  "Staging texture '{}' cannot be created with initial data, ignoring provided data",
						  a_Desc.Name );

			if ( !CommitAsStagingTexture() )
			{
				ASSERT( false, "Failed to create staging texture" );
			}

			return;
		}

		// Generate a texture handle
		OpenGL1::GenTextures( 1, &m_GLHandle );

		switch ( m_Desc.Dimension )
		{
			case ERHITextureDimension::Texture1D:
			{
				m_GLTarget = GL_TEXTURE_1D;
				OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
				NOT_IMPLEMENTED;
				break;
			}
			case ERHITextureDimension::Texture2D:
			{
				if ( m_Desc.Samples > 1 )
				{
					m_GLTarget = GL_TEXTURE_2D_MULTISAMPLE;
					OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
					TODO( "Check if multisampling is supported!" );
					// Create a multisampled texture
					OpenGL3::TexImage2DMultisample( m_GLTarget,
						m_Desc.Samples, m_GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height, GL_TRUE
					);

					NOT_IMPLEMENTED;
				}
				else
				{
					m_GLTarget = GL_TEXTURE_2D;
					OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
					OpenGL4::TexStorage2D( m_GLTarget,
						m_Desc.Mips, m_GLFormat.InternalFormat,
						a_Desc.Width, a_Desc.Height
					);

					// Keep OpenGL happy by specifying the mip range
					OpenGL1::TexParameteri( m_GLTarget, GL_TEXTURE_BASE_LEVEL, 0 );
					OpenGL1::TexParameteri( m_GLTarget, GL_TEXTURE_MAX_LEVEL, m_Desc.Mips - 1 );

					if ( a_SubResourcesData.empty() )
					{
						break;
					}


					// Upload the texture data
					if ( a_SubResourcesData.size() == 1 && m_Desc.Mips > 1 )
					{
						// Upload top mip only
						OpenGL1::TexSubImage2D( m_GLTarget, 0, 0, 0,
											   a_Desc.Width, a_Desc.Height,
											   m_GLFormat.Format, m_GLFormat.Type,
											   a_SubResourcesData[0].Data );

						// Generate the remaining mip levels automatically
						OpenGL3::GenerateMipmap( m_GLTarget );
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

							OpenGL1::TexSubImage2D( m_GLTarget, mip,
												   dstBox.MinX, dstBox.MinY,
												   dstBox.Width(), dstBox.Height(),
												   m_GLFormat.Format, m_GLFormat.Type,
												   a_SubResourcesData[mip].Data );
						}
					}

				}
				break;
			}
			case ERHITextureDimension::Texture3D:
			{
				m_GLTarget = GL_TEXTURE_3D;
				OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
				NOT_IMPLEMENTED;
				break;
			}
			case ERHITextureDimension::TextureCube:
			{
				m_GLTarget = GL_TEXTURE_CUBE_MAP;
				OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
				OpenGL4::TexStorage2D( m_GLTarget,
					m_Desc.Mips, m_GLFormat.InternalFormat,
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
											   m_GLFormat.Format, m_GLFormat.Type,
											   a_SubResourcesData[0].Data );
					}

					// Generate the remaining mip levels automatically
					OpenGL3::GenerateMipmap( m_GLTarget );
				}
				else
				{
					// Upload all provided mip levels and faces
					for ( uint32_t face = 0; face < 6; ++face )
					{
						for ( uint32_t mip = 0; mip < m_Desc.Mips; ++mip )
						{
							Box dstBox{
								0, Math::Max( m_Desc.Width >> mip, 1u ),
								0, Math::Max( m_Desc.Height >> mip, 1u )
							};

							OpenGL1::TexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip,
												   dstBox.MinX, dstBox.MinY,
												   dstBox.Width(), dstBox.Height(),
												   m_GLFormat.Format, m_GLFormat.Type,
												   a_SubResourcesData[mip + face * m_Desc.Mips].Data );
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

		// Set the object label for debugging
	#if RHI_USE_DEBUG_NAMES
		if ( glObjectLabel && Valid() && !m_Desc.Name.empty() )
		{
			OpenGL4::ObjectLabel( GL_TEXTURE, m_GLHandle, (GLsizei)m_Desc.Name.size(), m_Desc.Name.c_str() );
		}
	#endif

		const auto samplerProps = Device()->GetGPUInfo().DeviceFeatures.Sampler;

		// Bind default sampler parameters using the texture's default sampler
		OpenGL1::BindTexture( m_GLTarget, m_GLHandle );
		GLSamplerDesc samplerDesc = GLSamplerDesc::From( m_Desc.DefaultSampler, Device() );
		samplerDesc.ApplyToTexture( m_GLTarget, 0 );
		OpenGL1::BindTexture( m_GLTarget, 0 );
	}

	bool RHITexture_OpenGLImpl::Release()
	{
		if ( m_GLHandle != 0 )
		{
			if ( m_Desc.HeapType == ERHIHeapType::Staging )
			{
				// Delete the PBO
				OpenGL1::DeleteBuffers( 1, &m_GLHandle );
			}
			else
			{
				// Delete the texture
				OpenGL1::DeleteTextures( 1, &m_GLHandle );
			}

			m_GLHandle = 0;
		}

		m_GLFormat = {};
		m_GLTarget = GL_NONE;

		return true;
	}

	RHITextureSubresourceData RHITexture_OpenGLImpl::MapSubresource( const RHITextureSlice& a_Slice )
	{
		RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Only staging textures can be mapped" );

		const RHIBufferRange subresourceRange = GetSubresourceRange( a_Slice.Resolve( m_Desc ) );

		// Bind the PBO and map it for reading
		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, m_GLHandle );
		void* mappedData = OpenGL3::MapBufferRange( GL_PIXEL_PACK_BUFFER,
			(GLintptr)subresourceRange.Offset,
			(GLsizeiptr)subresourceRange.Size,
			GL_MAP_READ_BIT
		);
		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

		if ( !mappedData )
		{
			return {};
		}

		const size_t rowStride = GetRHIFormatInfo( m_Desc.Format ).Bytes() * Math::Max( m_Desc.Width >> a_Slice.MipLevel, 1u );
		return RHITextureSubresourceData{}
			.SetData( mappedData )
			.SetRowStride( rowStride )
			.SetDepthStride( rowStride * Math::Max( m_Desc.Height >> a_Slice.MipLevel, 1u ) );
	}

	void RHITexture_OpenGLImpl::UnmapSubresource( const RHITextureSlice& a_Slice )
	{
		RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Only staging textures can be unmapped" );

		// Bind the PBO
		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, m_GLHandle );

		// Unmap the buffer
		if ( !OpenGL1::UnmapBuffer( GL_PIXEL_PACK_BUFFER ) )
		{
			ASSERT( false, "Failed to unmap staging texture" );
		}

		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
	}

	size_t RHITexture_OpenGLImpl::GetTotalSizeInBytes() const
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( m_Desc.Format );
		size_t totalSize = 0;
		for ( uint32_t mip = 0; mip < m_Desc.Mips; ++mip )
		{
			uint32_t mipWidth = Math::Max( m_Desc.Width >> mip, 1u );
			uint32_t mipHeight = Math::Max( m_Desc.Height >> mip, 1u );
			uint32_t mipDepth = m_Desc.Is3D() ? Math::Max( m_Desc.DepthOrArraySize >> mip, 1u ) : m_Desc.DepthOrArraySize;
			totalSize += formatInfo.Bytes() * mipWidth * mipHeight * mipDepth;
		}
		return totalSize;
	}

	RHIBufferRange RHITexture_OpenGLImpl::GetSubresourceRange( const RHITextureSlice& a_Slice ) const
	{
		RHIBufferRange range{};

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( m_Desc.Format );
		if ( a_Slice.MipLevel >= m_Desc.Mips )
		{
			ASSERT( false, "Invalid mip level" );
			return {};
		}

		// Calculate the size of all previous mip levels
		for ( uint32_t mip = 0; mip < a_Slice.MipLevel; ++mip )
		{
			uint32_t mipWidth = Math::Max( m_Desc.Width >> mip, 1u );
			uint32_t mipHeight = Math::Max( m_Desc.Height >> mip, 1u );
			uint32_t mipDepth = m_Desc.Is3D() ? Math::Max( m_Desc.DepthOrArraySize >> mip, 1u ) : m_Desc.DepthOrArraySize;
			range.Offset += formatInfo.Bytes() * mipWidth * mipHeight * mipDepth;
		}

		if ( a_Slice.ArraySlice >= m_Desc.DepthOrArraySize )
		{
			ASSERT( false, "Invalid array slice" );
			return {};
		}

		if ( m_Desc.IsArray() || m_Desc.Is3D() )
		{
			uint32_t mipWidth = Math::Max( m_Desc.Width >> a_Slice.MipLevel, 1u );
			uint32_t mipHeight = Math::Max( m_Desc.Height >> a_Slice.MipLevel, 1u );
			uint32_t mipDepth = m_Desc.Is3D() ? Math::Max( m_Desc.DepthOrArraySize >> a_Slice.MipLevel, 1u ) : m_Desc.DepthOrArraySize;
			range.Offset += formatInfo.Bytes() * mipWidth * mipHeight * a_Slice.ArraySlice;
		}

		range.Size = formatInfo.Bytes() *
						Math::Max( m_Desc.Width >> a_Slice.MipLevel, 1u ) *
						Math::Max( m_Desc.Height >> a_Slice.MipLevel, 1u ) *
						( m_Desc.Is3D() ? Math::Max( m_Desc.DepthOrArraySize >> a_Slice.MipLevel, 1u ) : 1u );

		return range;
	}

	bool RHITexture_OpenGLImpl::CommitAsStagingTexture()
	{
		m_GLTarget = GL_BUFFER;

		// Create a Pixel Buffer Object (PBO) for staging
		OpenGL1::GenBuffers( 1, &m_GLHandle );
		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, m_GLHandle );

		// Allocate the buffer storage
		OpenGL1::BufferData( GL_PIXEL_PACK_BUFFER, (GLsizeiptr)GetTotalSizeInBytes(), nullptr, GL_DYNAMIC_READ );

	#if RHI_USE_DEBUG_NAMES
		if ( glObjectLabel && Valid() && !m_Desc.Name.empty() )
		{
			OpenGL4::ObjectLabel( GL_BUFFER, m_GLHandle, (GLsizei)m_Desc.Name.size(), m_Desc.Name.c_str() );
		}
	#endif

		OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

		return true;
	}


} // namespace Tridium::GL