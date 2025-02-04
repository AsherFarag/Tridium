#pragma once
#include "RHIResource.h"

namespace Tridium {

    enum class ERHITextureFormat : uint8_t
    {
        Unknown,

        // 8-bit Unorm
        R8,        // DXGI_FORMAT_R8_UNORM / GL_R8 / VK_FORMAT_R8_UNORM
        RG8,       // DXGI_FORMAT_R8G8_UNORM / GL_RG8 / VK_FORMAT_R8G8_UNORM
        RGBA8,     // DXGI_FORMAT_R8G8B8A8_UNORM / GL_RGBA8 / VK_FORMAT_R8G8B8A8_UNORM

        // 8-bit sRGB
        SRGB8,     // DXGI_FORMAT_UNKNOWN (Use SRGBA8) / GL_SRGB8 / VK_FORMAT_R8G8B8_SRGB
        SRGBA8,    // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB / GL_SRGB8_ALPHA8 / VK_FORMAT_R8G8B8A8_SRGB

        // 16-bit Unorm & Float
        R16,       // DXGI_FORMAT_R16_UNORM / GL_R16 / VK_FORMAT_R16_UNORM
        RG16,      // DXGI_FORMAT_R16G16_UNORM / GL_RG16 / VK_FORMAT_R16G16_UNORM
        RGBA16,    // DXGI_FORMAT_R16G16B16A16_UNORM / GL_RGBA16 / VK_FORMAT_R16G16B16A16_UNORM
        R16F,      // DXGI_FORMAT_R16_FLOAT / GL_R16F / VK_FORMAT_R16_SFLOAT
        RG16F,     // DXGI_FORMAT_R16G16_FLOAT / GL_RG16F / VK_FORMAT_R16G16_SFLOAT
        RGBA16F,   // DXGI_FORMAT_R16G16B16A16_FLOAT / GL_RGBA16F / VK_FORMAT_R16G16B16A16_SFLOAT

        // 32-bit Float
        R32F,      // DXGI_FORMAT_R32_FLOAT / GL_R32F / VK_FORMAT_R32_SFLOAT
        RG32F,     // DXGI_FORMAT_R32G32_FLOAT / GL_RG32F / VK_FORMAT_R32G32_SFLOAT
        RGBA32F,   // DXGI_FORMAT_R32G32B32A32_FLOAT / GL_RGBA32F / VK_FORMAT_R32G32B32A32_SFLOAT

        // Integer Formats
        R16I,      // DXGI_FORMAT_R16_SINT / GL_R16I / VK_FORMAT_R16_SINT
        RG16I,     // DXGI_FORMAT_R16G16_SINT / GL_RG16I / VK_FORMAT_R16G16_SINT
        RGBA16I,   // DXGI_FORMAT_R16G16B16A16_SINT / GL_RGBA16I / VK_FORMAT_R16G16B16A16_SINT
        R32I,      // DXGI_FORMAT_R32_SINT / GL_R32I / VK_FORMAT_R32_SINT
        RG32I,     // DXGI_FORMAT_R32G32_SINT / GL_RG32I / VK_FORMAT_R32G32_SINT
        RGBA32I,   // DXGI_FORMAT_R32G32B32A32_SINT / GL_RGBA32I / VK_FORMAT_R32G32B32A32_SINT

        // Depth-Stencil Formats
        D16,       // DXGI_FORMAT_D16_UNORM / GL_DEPTH_COMPONENT16 / VK_FORMAT_D16_UNORM
        D32,       // DXGI_FORMAT_D32_FLOAT / GL_DEPTH_COMPONENT32F / VK_FORMAT_D32_SFLOAT
        D24S8,     // DXGI_FORMAT_D24_UNORM_S8_UINT / GL_DEPTH24_STENCIL8 / VK_FORMAT_D24_UNORM_S8_UINT

        // Compressed Formats (BC / DXT)
        BC1,       // DXGI_FORMAT_BC1_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT1_EXT / VK_FORMAT_BC1_RGB_UNORM_BLOCK
        BC3,       // DXGI_FORMAT_BC3_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT5_EXT / VK_FORMAT_BC3_UNORM_BLOCK
        BC4,       // DXGI_FORMAT_BC4_UNORM / GL_COMPRESSED_RED_RGTC1 / VK_FORMAT_BC4_UNORM_BLOCK
        BC5,       // DXGI_FORMAT_BC5_UNORM / GL_COMPRESSED_RG_RGTC2 / VK_FORMAT_BC5_UNORM_BLOCK
        BC7,       // DXGI_FORMAT_BC7_UNORM / GL_COMPRESSED_RGBA_BPTC_UNORM_ARB / VK_FORMAT_BC7_UNORM_BLOCK

        COUNT,
    };

	// Returns the size in bytes of the given texture format.
	constexpr uint32_t GetTextureFormatSize( ERHITextureFormat a_Format )
	{
		switch ( a_Format )
		{
			using enum ERHITextureFormat;
			case R8:      return 1;
			case RG8:     return 2;
			case RGBA8:   return 4;
			case SRGB8:   return 3;
			case SRGBA8:  return 4;
			case R16:     return 2;
			case RG16:    return 4;
			case RGBA16:  return 8;
			case R16F:    return 2;
			case RG16F:   return 4;
			case RGBA16F: return 8;
			case R32F:    return 4;
			case RG32F:   return 8;
			case RGBA32F: return 16;
			case R16I:    return 2;
			case RG16I:   return 4;
			case RGBA16I: return 8;
			case R32I:    return 4;
			case RG32I:   return 8;
			case RGBA32I: return 16;
			case D16:     return 2;
			case D32:     return 4;
			case D24S8:   return 4;
			case BC1:     return 8;
			case BC3:     return 16;
			case BC4:     return 8;
			case BC5:     return 16;
			case BC7:     return 16;
			default: return 0;
		}
	}

	RHI_RESOURCE_BASE_TYPE( Texture )
	{
		Span<const uint8_t> InitialData = {};
		uint8_t DimensionCount = 2;
		uint32_t Dimensions[3] = { 0, 0, 0 };
		uint32_t Layers = 1;
		uint32_t Mips = 1;
		bool IsRenderTarget = false;
		bool IsCubeMap = false;
		ERHITextureFormat Format = ERHITextureFormat::RGBA8;
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
	};

}