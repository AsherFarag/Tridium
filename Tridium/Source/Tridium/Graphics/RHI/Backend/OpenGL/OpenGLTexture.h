#pragma once
#include "OpenGLCommon.h"
#include <Tridium/Graphics/RHI/RHITexture.h>

namespace Tridium {

	struct GLTextureFormat
	{
		GLenum InternalFormat;
		GLenum Format;
		GLenum Type;

		bool IsValid() const
		{
			return InternalFormat != GL_NONE
				&& Format != GL_NONE
				&& Type != GL_NONE;
		}
	};

	class OpenGLTexture final : public RHITexture
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );

		bool Commit( const void* a_Params ) override;

		bool Release() override;

		bool IsValid() const override
		{
			return m_Handle != 0;
		}

		const void* NativePtr() const override
		{
			return &m_Handle;
		}

		GLuint GetGLHandle() const { return m_Handle; }
		bool IsRenderBuffer() const { return false; }

	private:
		GLuint m_Handle = 0;
		GLTextureFormat m_Format = {};
	};

	constexpr GLTextureFormat RHIToGLTextureFormat( ERHITextureFormat a_Format )
	{
		switch ( a_Format )
		{
		case ERHITextureFormat::R8:      return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::RG8:     return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::RGBA8:   return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::SRGB8:   return { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::SRGBA8:  return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::R16:     return { GL_R16, GL_RED, GL_UNSIGNED_SHORT };
		case ERHITextureFormat::RG16:    return { GL_RG16, GL_RG, GL_UNSIGNED_SHORT };
		case ERHITextureFormat::RGBA16:  return { GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT };
		case ERHITextureFormat::R16F:    return { GL_R16F, GL_RED, GL_HALF_FLOAT };
		case ERHITextureFormat::RG16F:   return { GL_RG16F, GL_RG, GL_HALF_FLOAT };
		case ERHITextureFormat::RGBA16F: return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT };
		case ERHITextureFormat::R32F:    return { GL_R32F, GL_RED, GL_FLOAT };
		case ERHITextureFormat::RG32F:   return { GL_RG32F, GL_RG, GL_FLOAT };
		case ERHITextureFormat::RGBA32F: return { GL_RGBA32F, GL_RGBA, GL_FLOAT };
		case ERHITextureFormat::R16I:    return { GL_R16I, GL_RED_INTEGER, GL_SHORT };
		case ERHITextureFormat::RG16I:   return { GL_RG16I, GL_RG_INTEGER, GL_SHORT };
		case ERHITextureFormat::RGBA16I: return { GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT };
		case ERHITextureFormat::R32I:    return { GL_R32I, GL_RED_INTEGER, GL_INT };
		case ERHITextureFormat::RG32I:   return { GL_RG32I, GL_RG_INTEGER, GL_INT };
		case ERHITextureFormat::RGBA32I: return { GL_RGBA32I, GL_RGBA_INTEGER, GL_INT };
		case ERHITextureFormat::D16:     return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };
		case ERHITextureFormat::D32:     return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
		case ERHITextureFormat::D24S8:   return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
		case ERHITextureFormat::BC1:     return { GL_NONE, GL_NONE, GL_NONE }; // TODO: Implement BC1
		case ERHITextureFormat::BC3:     return { GL_NONE, GL_NONE, GL_NONE }; // TODO: Implement BC3
		case ERHITextureFormat::BC4:     return { GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::BC5:     return { GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE };
		case ERHITextureFormat::BC7:     return { GL_NONE, GL_NONE, GL_NONE }; // TODO: Implement BC7
		default:                         return { GL_NONE, GL_NONE, GL_NONE };
		}
	}

}