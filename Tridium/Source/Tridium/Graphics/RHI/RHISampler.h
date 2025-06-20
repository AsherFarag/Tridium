#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	//=======================================================

	enum ERHISamplerFlags : uint32_t
	{
		None = 0,
		// If set, the range of the texture coordinates are not normalized to [0, 1]
		// and are instead in the range of [0, Width] for U, [0, Height] for V, and [0, Depth] for W.
		// WARNING: Not yet supported
		NonNormalizedCoordinates = 1 << 1,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHISamplerFlags );

	//=======================================================
	// RHI Sampler
	//  Describes a sampler state used for texture sampling in shaders.
	//  For anisotropic filtering, all 3 filters (Min, Mag, Mip) must be set to either Anisotropic or ComparisonAnisotropic.
	//  'MinFilter' and 'MagFilter' must be either a regular filter or a comparison filter. One cannot be a regular filter while the other is a comparison filter.
	struct RHISampler
	{
		// Texture minification filter, the filter used when a texel is smaller than the pixel size.
		ERHISamplerFilter MinFilter = ERHISamplerFilter::Linear;
		// Texture magnification filter, the filter used when a texel is larger than the pixel size.
		ERHISamplerFilter MagFilter = ERHISamplerFilter::Linear;
		// Texture mipmap filter, the filter used when sampling a mipmap level.
		ERHISamplerFilter MipFilter = ERHISamplerFilter::Linear;
		// Address mode for the U coordinate (horizontal).
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		// Address mode for the V coordinate (vertical).
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		// Address mode for the W coordinate (depth). (Only used for 3D textures or texture arrays)
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		// Clamps the maximum anisotropy for anisotropic filtering to this value. Only used if the filter is anisotropic.
		uint8_t MaxAnisotropy = 1u;
		// Function used to compare sampled data against existing sampled data, only used if a comparison filter is used.
		ERHIComparison ComparisonFunc = ERHIComparison::Never;
		// Offset from the calculated mipmap level. Sample Level = Calculated Mip Level + MipLODBias.
		float MipLODBias = 0.0f;
		// Extra flags to specify specific sampler behavior.
		ERHISamplerFlags Flags = ERHISamplerFlags::None;
		// Border color used when the address mode is set to Border.
		Color BorderColor = Color::White();
		// Clamps the minimum mipmap level that can be sampled. Must be less than or equal to MaxLOD.
		float MinLOD = 0.0f;
		// Clamps the maximum mipmap level that can be sampled. Must be greater than or equal to MinLOD.
		float MaxLOD = +FLT_MAX;

		constexpr auto& SetMinFilter( ERHISamplerFilter a_Filter ) { MinFilter = a_Filter; return *this; }
		constexpr auto& SetMagFilter( ERHISamplerFilter a_Filter ) { MagFilter = a_Filter; return *this; }
		constexpr auto& SetMipFilter( ERHISamplerFilter a_Filter ) { MipFilter = a_Filter; return *this; }
		constexpr auto& SetAddressU( ERHISamplerAddressMode a_Address ) { AddressU = a_Address; return *this; }
		constexpr auto& SetAddressV( ERHISamplerAddressMode a_Address ) { AddressV = a_Address; return *this; }
		constexpr auto& SetAddressW( ERHISamplerAddressMode a_Address ) { AddressW = a_Address; return *this; }
		constexpr auto& SetMipLODBias( float a_Bias ) { MipLODBias = a_Bias; return *this; }
		constexpr auto& SetMaxAnisotropy( uint32_t a_Anisotropy ) { MaxAnisotropy = a_Anisotropy; return *this; }
		constexpr auto& SetComparisonFunc( ERHIComparison a_Comparison ) { ComparisonFunc = a_Comparison; return *this; }
		constexpr auto& SetFlags( ERHISamplerFlags a_Flags ) { Flags = a_Flags; return *this; }
		constexpr auto& SetFlag( ERHISamplerFlags a_Flag, bool a_Enabled = true ) { Flags = EnumFlags( Flags ).SetFlag( a_Flag, a_Enabled ); return *this; }
		constexpr auto& SetBorderColor( const Color& a_Color ) { BorderColor = a_Color; return *this; }
		constexpr auto& SetMinLOD( float a_LOD ) { MinLOD = a_LOD; return *this; }
		constexpr auto& SetMaxLOD( float a_LOD ) { MaxLOD = a_LOD; return *this; }

		constexpr bool operator==( const RHISampler& a_Other ) const
		{
			if ( std::is_constant_evaluated() )
			{
				// If we are in a constant expression, we can compare the members directly.
				return MinFilter == a_Other.MinFilter &&
					MagFilter == a_Other.MagFilter &&
					MipFilter == a_Other.MipFilter &&
					AddressU == a_Other.AddressU &&
					AddressV == a_Other.AddressV &&
					AddressW == a_Other.AddressW &&
					MaxAnisotropy == a_Other.MaxAnisotropy &&
					ComparisonFunc == a_Other.ComparisonFunc &&
					MipLODBias == a_Other.MipLODBias &&
					Flags == a_Other.Flags &&
					BorderColor == a_Other.BorderColor &&
					MinLOD == a_Other.MinLOD &&
					MaxLOD == a_Other.MaxLOD;
			}

			// RHISampler is a tightly packed POD structure, so we can compare the raw memory.
			return std::memcmp( this, &a_Other, sizeof( RHISampler ) ) == 0;
		}
	};

	//=======================================================
	// RHI Sampler Description
	//
	struct RHISamplerDesc
	{
		using ResourceType = class IRHISampler;
		// Texture minification filter, the filter used when a texel is smaller than the pixel size.
		ERHISamplerFilter MinFilter = ERHISamplerFilter::Linear;
		// Texture magnification filter, the filter used when a texel is larger than the pixel size.
		ERHISamplerFilter MagFilter = ERHISamplerFilter::Linear;
		// Texture mipmap filter, the filter used when sampling a mipmap level.
		ERHISamplerFilter MipFilter = ERHISamplerFilter::Linear;
		// Address mode for the U coordinate (horizontal).
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		// Address mode for the V coordinate (vertical).
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		// Address mode for the W coordinate (depth). (Only used for 3D textures or texture arrays)
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		// Clamps the maximum anisotropy for anisotropic filtering to this value. Only used if the filter is anisotropic.
		uint8_t MaxAnisotropy = 1u;
		// Function used to compare sampled data against existing sampled data, only used if a comparison filter is used.
		ERHIComparison ComparisonFunc = ERHIComparison::Never;
		// Offset from the calculated mipmap level. Sample Level = Calculated Mip Level + MipLODBias.
		float MipLODBias = 0.0f;
		// Extra flags to specify specific sampler behavior.
		ERHISamplerFlags Flags = ERHISamplerFlags::None;
		// Border color used when the address mode is set to Border.
		Color BorderColor = Color::White();
		// Clamps the minimum mipmap level that can be sampled. Must be less than or equal to MaxLOD.
		float MinLOD = 0.0f;
		// Clamps the maximum mipmap level that can be sampled. Must be greater than or equal to MinLOD.
		float MaxLOD = +FLT_MAX;
		// Debug Name
		String Name{};

		constexpr auto& SetMinFilter( ERHISamplerFilter a_Filter ) { MinFilter = a_Filter; return *this; }
		constexpr auto& SetMagFilter( ERHISamplerFilter a_Filter ) { MagFilter = a_Filter; return *this; }
		constexpr auto& SetMipFilter( ERHISamplerFilter a_Filter ) { MipFilter = a_Filter; return *this; }
		constexpr auto& SetAddressU( ERHISamplerAddressMode a_Address ) { AddressU = a_Address; return *this; }
		constexpr auto& SetAddressV( ERHISamplerAddressMode a_Address ) { AddressV = a_Address; return *this; }
		constexpr auto& SetAddressW( ERHISamplerAddressMode a_Address ) { AddressW = a_Address; return *this; }
		constexpr auto& SetMipLODBias( float a_Bias ) { MipLODBias = a_Bias; return *this; }
		constexpr auto& SetMaxAnisotropy( uint32_t a_Anisotropy ) { MaxAnisotropy = a_Anisotropy; return *this; }
		constexpr auto& SetComparisonFunc( ERHIComparison a_Comparison ) { ComparisonFunc = a_Comparison; return *this; }
		constexpr auto& SetFlags( ERHISamplerFlags a_Flags ) { Flags = a_Flags; return *this; }
		constexpr auto& SetFlag( ERHISamplerFlags a_Flag, bool a_Enabled = true ) { Flags = EnumFlags( Flags ).SetFlag( a_Flag, a_Enabled ); return *this; }
		constexpr auto& SetBorderColor( const Color& a_Color ) { BorderColor = a_Color; return *this; }
		constexpr auto& SetMinLOD( float a_LOD ) { MinLOD = a_LOD; return *this; }
		constexpr auto& SetMaxLOD( float a_LOD ) { MaxLOD = a_LOD; return *this; }
		          auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }

		constexpr bool operator==( const RHISamplerDesc& a_Other ) const
		{
			// Compare all members except the debug name
			constexpr size_t SizeWithoutName = sizeof( RHISamplerDesc ) - sizeof( String );
			return false;
		}
	};

	class IRHISampler : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( Sampler )
		IRHISampler( IDynamicRHI* a_Device, const DescriptorType& a_Desc ) : IRHIObject( a_Device ), m_Desc( a_Desc ) {}
		virtual ~IRHISampler() = default;
	};

} // namespace Tridium

namespace std {

	template<>
	struct hash<Tridium::RHISampler>
	{
		size_t operator()( const Tridium::RHISampler& a_Sampler ) const noexcept
		{
			const std::string_view raw( reinterpret_cast<const char*>(&a_Sampler), sizeof( Tridium::RHISampler ) );
			return std::hash<std::string_view>{}(raw);
		}
	};

} // namespace std