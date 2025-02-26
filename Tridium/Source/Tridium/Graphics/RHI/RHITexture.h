#pragma once
#include "RHIResource.h"

namespace Tridium {

	DEFINE_RHI_RESOURCE( Texture )
	{
		Span<const uint8_t> InitialData = {}; // Initial data for the texture
		uint32_t Width  = 1;                  // Width of the texture
		uint32_t Height = 1;                  // Height of the texture
		uint32_t Depth  = 1;                  // Depth of the texture
		uint32_t Layers = 1;                  // Number of layers in the texture (e.g. 6 for a cube map)
		uint32_t Mips   = 1;                  // Number of mip levels in the texture
		bool IsRenderTarget = false;          // Whether the texture is a render target
		ERHITextureFormat Format       = ERHITextureFormat::RGBA8;        // Data format of the texture
		ERHIUsageHint UsageHint        = ERHIUsageHint::Default;          // Usage hint for the texture
		ERHITextureDimension Dimension = ERHITextureDimension::Texture2D; // Dimension of the texture
	};

}