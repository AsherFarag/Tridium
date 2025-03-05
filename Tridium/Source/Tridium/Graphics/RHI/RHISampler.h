#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	DEFINE_ALLOCATABLE_RHI_RESOURCE( Sampler )
	{
		ERHISamplerFilter Filter = ERHISamplerFilter::Bilinear;
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		float MipLODBias = 0.0f;
		uint32_t MaxAnisotropy = 1;
		ERHISamplerComparison ComparisonFunc = ERHISamplerComparison::Never;
		Color BorderColor = Color::Black();
		float MinLOD = 0.0f;
		float MaxLOD = FLT_MAX;
	};

} // namespace Tridium