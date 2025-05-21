#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	//=======================================================

	struct RHISamplerDescriptor
	{
		using ResourceType = class RHISampler;
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
		String Name{};

		constexpr auto& SetFilter( ERHISamplerFilter a_Filter ) { Filter = a_Filter; return *this; }
		constexpr auto& SetAddressU( ERHISamplerAddressMode a_Address ) { AddressU = a_Address; return *this; }
		constexpr auto& SetAddressV( ERHISamplerAddressMode a_Address ) { AddressV = a_Address; return *this; }
		constexpr auto& SetAddressW( ERHISamplerAddressMode a_Address ) { AddressW = a_Address; return *this; }
		constexpr auto& SetMipLODBias( float a_Bias ) { MipLODBias = a_Bias; return *this; }
		constexpr auto& SetMaxAnisotropy( uint32_t a_Anisotropy ) { MaxAnisotropy = a_Anisotropy; return *this; }
		constexpr auto& SetComparisonFunc( ERHISamplerComparison a_Comparison ) { ComparisonFunc = a_Comparison; return *this; }
		constexpr auto& SetBorderColor( const Color& a_Color ) { BorderColor = a_Color; return *this; }
		constexpr auto& SetMinLOD( float a_LOD ) { MinLOD = a_LOD; return *this; }
		constexpr auto& SetMaxLOD( float a_LOD ) { MaxLOD = a_LOD; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHISampler )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHISampler, ERHIResourceType::Sampler );
		RHISampler( const DescriptorType& a_Desc )
			: m_Desc( a_Desc )
		{}
	};

} // namespace Tridium