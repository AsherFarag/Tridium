#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHITexture.h>

namespace Tridium {

	struct GLTextureFormat
	{
		GLenum InternalFormat = GL_NONE;
		GLenum Format = GL_NONE;
		GLenum Type = GL_NONE;

		bool IsValid() const
		{
			return InternalFormat != GL_NONE
				&& Format != GL_NONE
				&& Type != GL_NONE;
		}
	};

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( OpenGLTexture, RHITexture )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( OpenGLTexture, ERHInterfaceType::OpenGL );

		virtual bool Commit( const RHITextureDescriptor & a_Desc ) override;
		virtual bool Release() override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const { return TextureObj.NativePtr(); }
		virtual bool IsValid() const override { return TextureObj.Valid(); }

		OpenGL::GLTextureWrapper TextureObj{};
		GLTextureFormat GLFormat{};
	};

	constexpr GLTextureFormat RHIToGLTextureFormat( ERHIFormat a_Format )
	{
		switch ( a_Format )
		{
		case ERHIFormat::Unknown:       return { GL_NONE, GL_NONE, GL_NONE };

		// 8-bit Normalized
		case ERHIFormat::R8_UNORM:      return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
		case ERHIFormat::RG8_UNORM:     return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
		case ERHIFormat::RGBA8_UNORM:   return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

		// 8-bit Integer
		case ERHIFormat::R8_SINT:       return { GL_R8I, GL_RED_INTEGER, GL_BYTE };
		case ERHIFormat::RG8_SINT:      return { GL_RG8I, GL_RG_INTEGER, GL_BYTE };
		case ERHIFormat::RGBA8_SINT:    return { GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE };

		case ERHIFormat::R8_UINT:       return { GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE };
		case ERHIFormat::RG8_UINT:      return { GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE };
		case ERHIFormat::RGBA8_UINT:    return { GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE };

		// 16-bit Normalized
		case ERHIFormat::R16_UNORM:     return { GL_R16, GL_RED, GL_UNSIGNED_SHORT };
		case ERHIFormat::RG16_UNORM:    return { GL_RG16, GL_RG, GL_UNSIGNED_SHORT };
		case ERHIFormat::RGBA16_UNORM:  return { GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT };

		// 16-bit Float
		case ERHIFormat::R16_FLOAT:     return { GL_R16F, GL_RED, GL_HALF_FLOAT };
		case ERHIFormat::RG16_FLOAT:    return { GL_RG16F, GL_RG, GL_HALF_FLOAT };
		case ERHIFormat::RGBA16_FLOAT:  return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT };

		// 16-bit Integer
		case ERHIFormat::R16_SINT:      return { GL_R16I, GL_RED_INTEGER, GL_SHORT };
		case ERHIFormat::RG16_SINT:     return { GL_RG16I, GL_RG_INTEGER, GL_SHORT };
		case ERHIFormat::RGBA16_SINT:   return { GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT };

		case ERHIFormat::R16_UINT:      return { GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT };
		case ERHIFormat::RG16_UINT:     return { GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT };
		case ERHIFormat::RGBA16_UINT:   return { GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT };

		// 32-bit Float
		case ERHIFormat::R32_FLOAT:     return { GL_R32F, GL_RED, GL_FLOAT };
		case ERHIFormat::RG32_FLOAT:    return { GL_RG32F, GL_RG, GL_FLOAT };
		case ERHIFormat::RGB32_FLOAT:   return { GL_RGB32F, GL_RGB, GL_FLOAT };
		case ERHIFormat::RGBA32_FLOAT:  return { GL_RGBA32F, GL_RGBA, GL_FLOAT };

		// 32-bit Integer
		case ERHIFormat::R32_SINT:      return { GL_R32I, GL_RED_INTEGER, GL_INT };
		case ERHIFormat::RG32_SINT:     return { GL_RG32I, GL_RG_INTEGER, GL_INT };
		case ERHIFormat::RGB32_SINT:    return { GL_RGB32I, GL_RGB_INTEGER, GL_INT };
		case ERHIFormat::RGBA32_SINT:   return { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT };

		case ERHIFormat::R32_UINT:      return { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT };
		case ERHIFormat::RG32_UINT:     return { GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT };
		case ERHIFormat::RGB32_UINT:    return { GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT };
		case ERHIFormat::RGBA32_UINT:   return { GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT };

		// sRGB
		case ERHIFormat::SRGBA8_UNORM:  return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE };

		// Depth & Stencil
		case ERHIFormat::D16_UNORM:     return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };
		case ERHIFormat::D32_FLOAT:     return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
		case ERHIFormat::D24_UNORM_S8_UINT: return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };

		// Compressed Formats
		TODO( "Implement compressed formats" );
		//case ERHIFormat::BC1_UNORM:     return { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
		//case ERHIFormat::BC3_UNORM:     return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
		case ERHIFormat::BC4_UNORM:     return { GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE };
		case ERHIFormat::BC5_UNORM:     return { GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE };
		//case ERHIFormat::BC7_UNORM:     return { GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_RGBA, GL_UNSIGNED_BYTE };

		default:                        return { GL_NONE, GL_NONE, GL_NONE };
		}
	}


}