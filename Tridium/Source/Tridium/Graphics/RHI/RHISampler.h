#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	//=======================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHISamplerDescriptor, RHISampler )
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

	DECLARE_RHI_RESOURCE_INTERFACE( RHISampler )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHISampler, ERHIResourceType::Sampler );
		virtual bool Commit( const RHISamplerDescriptor& a_Descriptor ) = 0;
	};

} // namespace Tridium