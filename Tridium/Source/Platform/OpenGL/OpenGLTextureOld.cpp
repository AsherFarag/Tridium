#include "tripch.h"
#include "Platform/OpenGL/OpenGLTextureOld.h"
#include <math.h>
#include <algorithm>

#define M_PI       3.14159265358979323846   // pi

namespace Tridium {

	namespace Util {

		ETextureFormat GLDataFormatToTridiumDataFormat( GLenum format )
		{
			switch ( format )
			{
			case GL_R: return ETextureFormat::R8;
			case GL_RG: return ETextureFormat::RG8;
			case GL_RGB: return ETextureFormat::RGB8;
			case GL_RGBA: return ETextureFormat::RGBA8;
			case GL_DEPTH_COMPONENT: return ETextureFormat::Depth;
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
			case ETextureFormat::RG16F: return GL_RG16F;
			case ETextureFormat::RGB16F: return GL_RGB16F;
			case ETextureFormat::RGBA16F: return GL_RGBA16F;
			case ETextureFormat::RGB32F: return GL_RGB32F;
			case ETextureFormat::RGBA32F: return GL_RGBA32F;
			case ETextureFormat::SRGB: return GL_SRGB;
			case ETextureFormat::SRGBA: return GL_SRGB_ALPHA;
			case ETextureFormat::Depth: return GL_DEPTH_COMPONENT;
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
			case ETextureFormat::RG16F: return GL_RG;
			case ETextureFormat::RGB16F: return GL_RGB;
			case ETextureFormat::RGBA16F: return GL_RGBA;
			case ETextureFormat::RGB32F: return GL_RGB;
			case ETextureFormat::RGBA32F: return GL_RGBA;
			case ETextureFormat::SRGB: return GL_RGB;
			case ETextureFormat::SRGBA: return GL_RGBA;
			case ETextureFormat::Depth: return GL_DEPTH_COMPONENT;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

		GLenum GetGLType( ETextureFormat format )
		{
			switch ( format )
			{
			case ETextureFormat::R8: return GL_UNSIGNED_BYTE;
			case ETextureFormat::RG8: return GL_UNSIGNED_BYTE;
			case ETextureFormat::RGB8: return GL_UNSIGNED_BYTE;
			case ETextureFormat::RGBA8: return GL_UNSIGNED_BYTE;
			case ETextureFormat::RG16F: return GL_FLOAT;
			case ETextureFormat::RGB16F: return GL_FLOAT;
			case ETextureFormat::RGBA16F: return GL_FLOAT;
			case ETextureFormat::RGB32F: return GL_FLOAT;
			case ETextureFormat::RGBA32F: return GL_FLOAT;
			case ETextureFormat::SRGB: return GL_UNSIGNED_BYTE;
			case ETextureFormat::SRGBA: return GL_UNSIGNED_BYTE;
			case ETextureFormat::Depth: return GL_FLOAT;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

		GLint TridiumTextureFilterToGLTextureFilter( ETextureFilter filter )
		{
			switch ( filter )
			{
			case ETextureFilter::Nearest: return GL_NEAREST;
			case ETextureFilter::Linear: return GL_LINEAR;
			case ETextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
			case ETextureFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
			case ETextureFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
			case ETextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}

		GLenum TridiumTextureWrapToGLTextureWrap( ETextureWrap wrap )
		{
			switch ( wrap )
			{
			case ETextureWrap::Repeat: return GL_REPEAT;
			case ETextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case ETextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case ETextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
			}

			TE_CORE_ASSERT( false );
			return 0;
		}



		template<typename T>
		std::array<T*, 6> CalculateCubeMap( SharedPtr<Texture> a_Texture, uint32_t a_Resolution, bool a_FilterLinear )
		{
			struct Vec3 { float x, y, z; };
			std::array<std::array<Vec3, 3>, 6> startRightUp = { { // for each face, contains the 3d starting point (corresponding to left bottom pixel), right direction, and up direction in 3d space, correponding to pixel x,y coordinates of each face		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

				{{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}},   // right
				{{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}},  // left
				{{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}},   // down
				{{{-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}},   // up
				{{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}},  // front
				{{{1.0f, -1.0f, 1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}},  // back 
			} };

			std::array<T*, 6> faces;
			for ( int i = 0; i < 6; i++ )
			{
				faces[i] = new T[a_Resolution * a_Resolution * 3];
			}
			int width = a_Texture->GetWidth();
			int height = a_Texture->GetHeight();
			int channels;
			float* imageData = static_cast<float*>( a_Texture->GetData() );

			switch ( a_Texture->GetSpecification().TextureFormat )
			{
			case ETextureFormat::RGB8:
				channels = 3;
				break;
			case ETextureFormat::RGBA8:
				channels = 4;
				break;
			case ETextureFormat::RGB16F:
				channels = 3;
				break;
			case ETextureFormat::RGBA16F:
				channels = 4;
				break;
			case ETextureFormat::RGB32F:
				channels = 3;
				break;
			case ETextureFormat::RGBA32F:
				channels = 4;
				break;
			default:
				TE_CORE_ASSERT( false, "Unknown texture format!" );
				break;
			}

			for ( int i = 0; i < 6; i++ )
			{
				Vec3& start = startRightUp[i][0];
				Vec3& right = startRightUp[i][1];
				Vec3& up = startRightUp[i][2];

				T* face = faces[i];
				Vec3 pixelDirection3d; // 3d direction corresponding to a pixel in the cubemap face
				//#pragma omp parallel for (private pixelDirection?)
				for ( int row = 0; row < a_Resolution; row++ )
				{
					for ( int col = 0; col < a_Resolution; col++ )
					{
						pixelDirection3d.x = start.x + ( (float)col * 2.0f + 0.5f ) / (float)a_Resolution * right.x + ( (float)row * 2.0f + 0.5f ) / (float)a_Resolution * up.x;
						pixelDirection3d.y = start.y + ( (float)col * 2.0f + 0.5f ) / (float)a_Resolution * right.y + ( (float)row * 2.0f + 0.5f ) / (float)a_Resolution * up.y;
						pixelDirection3d.z = start.z + ( (float)col * 2.0f + 0.5f ) / (float)a_Resolution * right.z + ( (float)row * 2.0f + 0.5f ) / (float)a_Resolution * up.z;

						float azimuth = atan2f( pixelDirection3d.x, -pixelDirection3d.z ) + (float)M_PI; // add pi to move range to 0-360 deg
						float elevation = atanf( pixelDirection3d.y / sqrtf( pixelDirection3d.x * pixelDirection3d.x + pixelDirection3d.z * pixelDirection3d.z ) ) + (float)M_PI / 2.0f;

						float colHdri = ( azimuth / (float)M_PI / 2.0f ) * width; // add pi to azimuth to move range to 0-360 deg
						float rowHdri = ( elevation / (float)M_PI ) * height;

						if ( !a_FilterLinear )
						{
							int colNearest = std::clamp( (int)colHdri, 0, width - 1 );
							int rowNearest = std::clamp( (int)rowHdri, 0, height - 1 );

							face[col * channels + a_Resolution * row * channels] = imageData[colNearest * channels + width * rowNearest * channels]; // red
							face[col * channels + a_Resolution * row * channels + 1] = imageData[colNearest * channels + width * rowNearest * channels + 1]; //green
							face[col * channels + a_Resolution * row * channels + 2] = imageData[colNearest * channels + width * rowNearest * channels + 2]; //blue
							if ( channels > 3 )
								face[col * channels + a_Resolution * row * channels + 3] = imageData[colNearest * channels + width * rowNearest * channels + 3]; //alpha
						}
						else // perform bilinear interpolation
						{
							float intCol, intRow;
							float factorCol = modf( colHdri - 0.5f, &intCol );        // factor gives the contribution of the next column, while the contribution of intCol is 1 - factor
							float factorRow = modf( rowHdri - 0.5f, &intRow );

							int low_idx_row = static_cast<int>( intRow );
							int low_idx_column = static_cast<int>( intCol );
							int high_idx_column;
							if ( factorCol < 0.0f )                           //modf can only give a negative value if the azimuth falls in the first pixel, left of the center, so we have to mix with the pixel on the opposite side of the panoramic image
								high_idx_column = width - 1;
							else if ( low_idx_column == width - 1 )          //if we are in the right-most pixel, and fall right of the center, mix with the left-most pixel
								high_idx_column = 0;
							else
								high_idx_column = low_idx_column + 1;

							int high_idx_row;
							if ( factorRow < 0.0f )
								high_idx_row = height - 1;
							else if ( low_idx_row == height - 1 )
								high_idx_row = 0;
							else
								high_idx_row = low_idx_row + 1;

							factorCol = abs( factorCol );
							factorRow = abs( factorRow );
							float f1 = ( 1 - factorRow ) * ( 1 - factorCol );
							float f2 = factorRow * ( 1 - factorCol );
							float f3 = ( 1 - factorRow ) * factorCol;
							float f4 = factorRow * factorCol;

							for ( int j = 0; j < channels; j++ )
							{
								unsigned char interpolatedValue = static_cast<unsigned char>( imageData[low_idx_column * channels + width * low_idx_row * channels + j] * f1 +
									imageData[low_idx_column * channels + width * high_idx_row * channels + j] * f2 +
									imageData[high_idx_column * channels + width * low_idx_row * channels + j] * f3 +
									imageData[high_idx_column * channels + width * high_idx_row * channels + j] * f4 );
								face[col * channels + a_Resolution * row * channels + j] = std::clamp( interpolatedValue, (uint8_t)0, (uint8_t)255 );
							}
						}
					}
				}
			}

			return faces;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Texture
	//////////////////////////////////////////////////////////////////////////

	OpenGLTextureOld::OpenGLTextureOld( const TextureSpecification& a_Specification, void* a_TextureData )
		: m_Specification( a_Specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Util::TridiumDataFormatToGLDataFormat( m_Specification.TextureFormat );
		m_InternalFormat = Util::TridiumDataFormatToGLInternalFormat( m_Specification.TextureFormat );

		//glGenTextures( GL_TEXTURE_2D, 1, &m_RendererID );
		const int mipLevels = 4;
		//glTextureStorage2D( m_RendererID, mipLevels, m_InternalFormat, m_Width, m_Height );

		glGenTextures( 1, &m_RendererID );
		glBindTexture( GL_TEXTURE_2D, m_RendererID );

		//Bind();
		SetMinFilter( a_Specification.MinFilter );
		SetMagFilter( a_Specification.MagFilter );
		SetWrapS( a_Specification.WrapS );
		SetWrapT( a_Specification.WrapT );
		SetWrapR( a_Specification.WrapR );
		//Unbind();

		if ( a_TextureData )
			SetData( a_TextureData, m_Width * m_Height * (uint32_t)Util::GLDataFormatToTridiumDataFormat( m_DataFormat ) );
	}

	OpenGLTextureOld::~OpenGLTextureOld()
	{
		glDeleteTextures( 1, &m_RendererID );
		TODO( "delete m_LocalData;" );
	}

	void OpenGLTextureOld::SetMinFilter( ETextureFilter a_Filter )
	{
		glTextureParameteri( m_RendererID, GL_TEXTURE_MIN_FILTER, Util::TridiumTextureFilterToGLTextureFilter( a_Filter ) );
	}

	void OpenGLTextureOld::SetMagFilter( ETextureFilter a_Filter )
	{
		glTextureParameteri( m_RendererID, GL_TEXTURE_MAG_FILTER, Util::TridiumTextureFilterToGLTextureFilter( a_Filter ) );
	}

	void OpenGLTextureOld::SetWrapS( ETextureWrap a_Wrap )
	{
		glTextureParameteri( m_RendererID, GL_TEXTURE_WRAP_S, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLTextureOld::SetWrapT( ETextureWrap a_Wrap )
	{
		glTextureParameteri( m_RendererID, GL_TEXTURE_WRAP_T, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLTextureOld::SetWrapR( ETextureWrap a_Wrap )
	{
		glTextureParameteri( m_RendererID, GL_TEXTURE_WRAP_R, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLTextureOld::SetData( void* a_Data, uint32_t a_Size )
	{
		uint32_t bpp = (uint32_t)Util::GLDataFormatToTridiumDataFormat( m_DataFormat );
		TE_CORE_ASSERT( a_Size == m_Width * m_Height * bpp, "Data must be entire texture!" );
		m_LocalData = a_Data;

		GLenum type = Util::GetGLType( m_Specification.TextureFormat );
		//glTextureSubImage2D( m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, type, m_LocalData );
		//glGenerateMipmap( GL_TEXTURE_2D );
		glTexImage2D( GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, type, m_LocalData );
		glGenerateMipmap( GL_TEXTURE_2D );
	}

	void OpenGLTextureOld::Bind( uint32_t slot ) const
	{
		glActiveTexture( GL_TEXTURE0 + slot );
		glBindTexture( GL_TEXTURE_2D, m_RendererID );
		//glBindTextureUnit( slot, m_RendererID );
	}
	void OpenGLTextureOld::Unbind( uint32_t slot ) const
	{
		glActiveTexture( GL_TEXTURE0 + slot );
		glBindTexture( GL_TEXTURE_2D, 0 );
		//glBindTextureUnit( slot, 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	// CubeMap
	//////////////////////////////////////////////////////////////////////////

	OpenGLCubeMap::OpenGLCubeMap( const TextureSpecification& a_Specification, SharedPtr<Texture> a_Texture )
		: m_Specification( a_Specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Util::TridiumDataFormatToGLDataFormat( m_Specification.TextureFormat );
		m_InternalFormat = Util::TridiumDataFormatToGLInternalFormat( m_Specification.TextureFormat );
		GLenum type = Util::GetGLType( m_Specification.TextureFormat );

		glGenTextures( 1, &m_RendererID );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_RendererID );

		for ( uint32_t i = 0; i < 6; ++i )
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Width, 0, m_DataFormat, type, nullptr );
		}

		SetMinFilter( a_Specification.MinFilter );
		SetMagFilter( a_Specification.MagFilter );
		SetWrapS( a_Specification.WrapS );
		SetWrapT( a_Specification.WrapT );
		SetWrapR( a_Specification.WrapR );

		if ( a_Texture )
		{
			SetData( a_Texture, m_Width );
		}

		GenerateMipMaps();
	}

	OpenGLCubeMap::OpenGLCubeMap( const TextureSpecification& a_Specification, const std::array<float*, 6>& a_CubeMapData )
		: m_Specification( a_Specification ), m_Width( m_Specification.Width ), m_Height( m_Specification.Height )
	{
		m_DataFormat = Util::TridiumDataFormatToGLDataFormat( m_Specification.TextureFormat );
		m_InternalFormat = Util::TridiumDataFormatToGLInternalFormat( m_Specification.TextureFormat );

		glGenTextures( 1, &m_RendererID );
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_RendererID );

		SetMinFilter( a_Specification.MinFilter );
		SetMagFilter( a_Specification.MagFilter );
		SetWrapS( a_Specification.WrapS );
		SetWrapT( a_Specification.WrapT );
		SetWrapR( a_Specification.WrapR );

		SetData( a_CubeMapData, m_Width );

		GenerateMipMaps();
	}

	OpenGLCubeMap::~OpenGLCubeMap()
	{
		glDeleteTextures( 1, &m_RendererID );
	}

	void OpenGLCubeMap::SetMinFilter( ETextureFilter a_Filter )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Util::TridiumTextureFilterToGLTextureFilter( a_Filter ) );
	}

	void OpenGLCubeMap::SetMagFilter( ETextureFilter a_Filter )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Util::TridiumTextureFilterToGLTextureFilter( a_Filter ) );
	}

	void OpenGLCubeMap::SetWrapS( ETextureWrap a_Wrap )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLCubeMap::SetWrapT( ETextureWrap a_Wrap )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLCubeMap::SetWrapR( ETextureWrap a_Wrap )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Util::TridiumTextureWrapToGLTextureWrap( a_Wrap ) );
	}

	void OpenGLCubeMap::SetData( SharedPtr<Texture> a_Texture, uint32_t a_Size )
	{
		SetData( Util::CalculateCubeMap<float>( a_Texture, a_Size, false) , a_Size );
	}

	void OpenGLCubeMap::SetData( const std::array<float*, 6>& a_CubeMapData, uint32_t a_Size )
	{
		m_Width = a_Size;
		m_Height = a_Size;
		GLenum type = Util::GetGLType( m_Specification.TextureFormat );

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
				(void*)a_CubeMapData[i]
			);
		}
	}

	void OpenGLCubeMap::GenerateMipMaps()
	{
		glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
	}

	void OpenGLCubeMap::SetMipLevel( uint32_t a_Level )
	{
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, a_Level );
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
