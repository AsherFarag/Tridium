#pragma once
#include "RHICommon.h"

namespace Tridium {

	//=======================================================
	// RHI Sampler
	//  A sampler represents a texture sampling state and is used to sample textures in shaders.
	//=======================================================

	enum ERHISamplerFlags : uint16_t
	{
		None = 0,
		// If set, the range of the texture coordinates are not normalized to [0, 1]
		// and are instead in the range of [0, Width] for U, [0, Height] for V, and [0, Depth] for W.
		// WARNING: Not yet supported
		NonNormalizedCoordinates = 1 << 1,

		NUM_BITS = 1,
	};
	DEFINE_ENUM_BITMASK_OPERATORS( ERHISamplerFlags );

	//=======================================================
	// RHI Sampler
	//  Describes a sampler state used for texture sampling in shaders.
	struct RHISampler
	{
		// Texture filter, used for minification, magnification, and mipmapping.
		ERHISamplerFilter Filter = ERHISamplerFilter::MinMagMipLinear;
		// Address mode for the U coordinate (horizontal).
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		// Address mode for the V coordinate (vertical).
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		// Address mode for the W coordinate (depth). (Only used for 3D textures or texture arrays)
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		// Clamps the maximum anisotropy for anisotropic filtering to this value. Only used if the filter is anisotropic. Range: [1, 16].
		uint8_t MaxAnisotropy = 1u;
		// Function used to compare sampled data against existing sampled data, only used if a comparison filter is used.
		ERHIComparison ComparisonFunc = ERHIComparison::Never;
		// Extra flags to specify specific sampler behavior.
		ERHISamplerFlags Flags = ERHISamplerFlags::None;
		// Offset from the calculated mipmap level. Sample Level = Calculated Mip Level + MipLODBias.
		float MipLODBias = 0.0f;
		// Border color used when the address mode is set to Border. NOTE: This color is packed into 4 16-bit unsigned integers (0-65535).
		Color4 BorderColor = Color4::White();
		// Clamps the minimum mipmap level that can be sampled. Must be less than or equal to MaxLOD.
		float MinLOD = 0.0f;
		// Clamps the maximum mipmap level that can be sampled. Must be greater than or equal to MinLOD.
		float MaxLOD = +FLT_MAX;

		constexpr auto& SetFilter( ERHISamplerFilter a_Filter ) { Filter = a_Filter; return *this; }
		constexpr auto& SetAddressU( ERHISamplerAddressMode a_Address ) { AddressU = a_Address; return *this; }
		constexpr auto& SetAddressV( ERHISamplerAddressMode a_Address ) { AddressV = a_Address; return *this; }
		constexpr auto& SetAddressW( ERHISamplerAddressMode a_Address ) { AddressW = a_Address; return *this; }
		constexpr auto& SetMipLODBias( float a_Bias ) { MipLODBias = a_Bias; return *this; }
		constexpr auto& SetMaxAnisotropy( uint8_t a_Anisotropy ) { MaxAnisotropy = Math::Clamp<uint8_t>( a_Anisotropy, 1u, 16u ); return *this; }
		constexpr auto& SetComparisonFunc( ERHIComparison a_Comparison ) { ComparisonFunc = a_Comparison; return *this; }
		constexpr auto& SetFlags( ERHISamplerFlags a_Flags ) { Flags = a_Flags; return *this; }
		constexpr auto& SetFlag( ERHISamplerFlags a_Flag, bool a_Enabled = true ) { Flags = EnumFlags( Flags ).SetFlag( a_Flag, a_Enabled ); return *this; }
		constexpr auto& SetBorderColor( const Color4& a_Color ) { BorderColor = a_Color; return *this; }
		constexpr auto& SetMinLOD( float a_LOD ) { MinLOD = a_LOD; return *this; }
		constexpr auto& SetMaxLOD( float a_LOD ) { MaxLOD = a_LOD; return *this; }

		constexpr bool operator==( const RHISampler& a_Other ) const
		{
			if ( std::is_constant_evaluated() )
			{
				// If we are in a constant expression, we can compare the members directly.
				return Filter == a_Other.Filter &&
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

	PACKED_STRUCT( struct RHIPackedSampler )
	{
		constexpr RHIPackedSampler() { m_Flags.IsValid = 0; }
		constexpr bool Valid() const { return m_Flags.IsValid != 0; }

		constexpr RHISampler Unpack() const
		{
			if ( !Valid() )
				return RHISampler{}; // Return an empty sampler if not valid

			RHISampler unpacked{};
			unpacked.MipLODBias = Cast<float>( m_MipLODBias16 ) / 16.0f;
			unpacked.MinLOD = Cast<float>( m_MinLOD16 ) / 16.0f;
			unpacked.MaxLOD = Cast<float>( m_MaxLOD16 ) / 16.0f;

			// Convert 16-bit to float [0, 1]
			unpacked.BorderColor[0] = Cast<float>( m_BorderColor[0] ) / 65535.0f;
			unpacked.BorderColor[1] = Cast<float>( m_BorderColor[1] ) / 65535.0f;
			unpacked.BorderColor[2] = Cast<float>( m_BorderColor[2] ) / 65535.0f;
			unpacked.BorderColor[3] = Cast<float>( m_BorderColor[3] ) / 65535.0f;

			// Filter
			unpacked.Filter = ERHISamplerFilter( m_Flags.Filter );

			// Sampler flags
			unpacked.Flags = ERHISamplerFlags( m_Flags.SamplerFlags );

			// Address modes - +1 because we ignore the Unknown address mode (0) and start from Repeat (1)
			unpacked.AddressU = ERHISamplerAddressMode( m_Flags.AddressU + 1 );
			unpacked.AddressV = ERHISamplerAddressMode( m_Flags.AddressV + 1 );
			unpacked.AddressW = ERHISamplerAddressMode( m_Flags.AddressW + 1 );

			// Max anisotropy
			unpacked.MaxAnisotropy = m_Flags.MaxAnisotropy;

			// Comparison function
			unpacked.ComparisonFunc = ERHIComparison( m_Flags.ComparisonFunc );

			return unpacked;
		}

		static constexpr RHIPackedSampler Pack( const RHISampler& a_Sampler )
		{
			RHIPackedSampler packed{};

			packed.m_Flags.Filter = uint8_t( a_Sampler.Filter );
			packed.m_Flags.SamplerFlags = uint8_t( a_Sampler.Flags );
			packed.m_Flags.AddressU = Math::Max<uint8_t>( uint8_t( a_Sampler.AddressU ), 1u ) - 1; // -1 because we ignore Unknown address mode
			packed.m_Flags.AddressV = Math::Max<uint8_t>( uint8_t( a_Sampler.AddressV ), 1u ) - 1;
			packed.m_Flags.AddressW = Math::Max<uint8_t>( uint8_t( a_Sampler.AddressW ), 1u ) - 1;
			packed.m_Flags.MaxAnisotropy = Math::Min<uint8_t>( a_Sampler.MaxAnisotropy, 16 ); // Clamp to [1, 16]
			packed.m_Flags.ComparisonFunc = uint8_t( a_Sampler.ComparisonFunc );
			packed.m_Flags.IsValid = 1;

			packed.m_MipLODBias16 = Cast<int16_t>( a_Sampler.MipLODBias * 16.0f );
			packed.m_MinLOD16 = Cast<uint16_t>( a_Sampler.MinLOD * 16.0f );
			packed.m_MaxLOD16 = Cast<uint16_t>( a_Sampler.MaxLOD * 16.0f );

			for ( int i = 0; i < 4; ++i )
				packed.m_BorderColor[i] = Cast<uint16_t>( Math::Clamp( a_Sampler.BorderColor[i], 0.0f, 1.0f ) * 65535.0f ); // Convert float [0, 1] to int16 [0, 65535]

			return packed;
		}

	private:
		PACKED_STRUCT( struct PackedFlags )
		{
			static_assert(ERHISamplerFlags::NUM_BITS <= 1, "ERHISamplerFlags must fit into 1 bit");
			uint8_t Filter : int( ERHISamplerFilter::NUM_BITS );    // ERHISamplerFilter
			uint8_t SamplerFlags : 1;                               // ERHISamplerFlags
			uint8_t AddressU : 2;                                   // ERHISamplerAddressMode

			uint8_t AddressV : 2;                                   // ERHISamplerAddressMode
			uint8_t AddressW : 2;                                   // ERHISamplerAddressMode
			uint8_t MaxAnisotropy : 4;                              // int - range [1, 16]

			uint8_t ComparisonFunc : 3;                             // ERHIComparison
			uint8_t IsValid : 1;                                    // bool - Is this sampler valid?
		} PACKED_STRUCT_END m_Flags;

		int16_t m_MipLODBias16;
		uint16_t m_MinLOD16;
		uint16_t m_MaxLOD16;
		uint16_t m_BorderColor[4]; // RGBA border color, stored as 16-bit unsigned integers
	};
	PACKED_STRUCT_END
	static_assert(sizeof( RHIPackedSampler ) <= 18, "RHIPackedSampler must be 18 bytes in size for memory efficiency");

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