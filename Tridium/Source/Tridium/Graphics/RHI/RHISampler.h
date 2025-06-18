#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	//=======================================================

	struct RHISamplerDesc
	{
		using ResourceType = class IRHISampler;
		ERHISamplerFilter Filter = ERHISamplerFilter::Bilinear;
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		float MipLODBias = 0.0f;
		uint32_t MaxAnisotropy = 1;
		ERHIComparison ComparisonFunc = ERHIComparison::Never;
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
		constexpr auto& SetComparisonFunc( ERHIComparison a_Comparison ) { ComparisonFunc = a_Comparison; return *this; }
		constexpr auto& SetBorderColor( const Color& a_Color ) { BorderColor = a_Color; return *this; }
		constexpr auto& SetMinLOD( float a_LOD ) { MinLOD = a_LOD; return *this; }
		constexpr auto& SetMaxLOD( float a_LOD ) { MaxLOD = a_LOD; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	class IRHISampler : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( Sampler )
		IRHISampler( IDynamicRHI* a_Device, const DescriptorType& a_Desc ) : IRHIObject( a_Device ), m_Desc( a_Desc ) {}
		virtual ~IRHISampler() = default;
	};

} // namespace Tridium