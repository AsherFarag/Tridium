#pragma once
#include "RHIResource.h"

namespace Tridium {

	inline constexpr uint32_t CalculateMipLevelCount( const uint32_t a_Width )
	{
		if ( a_Width == 0 )
			return 0;

		uint32_t mips = 0;
		while ( ( a_Width >> mips ) > 0 )
			++mips;

		CHECK( a_Width >= ( 1U << ( mips - 1 ) ) && a_Width < ( 1U << mips ), "Incorrect number of Mip levels" );
		return mips;
	}

	inline constexpr uint32_t CalculateMipLevelCount( const uint32_t a_Width, const uint32_t a_Height )
	{
		return CalculateMipLevelCount( Math::Max( a_Width, a_Height ) );
	}

	inline constexpr uint32_t CalculateMipLevelCount( const uint32_t a_Width, const uint32_t a_Height, const uint32_t a_Depth )
	{
		return CalculateMipLevelCount( Math::Max( Math::Max( a_Width, a_Height ), a_Depth ) );
	}


	//==========================================================================================
	// RHI Texture Subresource Data
	//  A structure that holds the data for a subresource of a texture.
	//  This is used for updating a texture with new data.
	//==========================================================================================
	struct RHITextureSubresourceData
	{
		// The data, in CPU memory, for the subresource.
		Span<const uint8_t> Data{};
		// The stride of a row of data in the texture, in bytes. For 2D and 3D textures.
		size_t RowStride = 0;
		// The stride of a slice of data in the texture, in bytes. For 3D textures.
		size_t DepthStride = 0;
	};

	namespace RHIConstants {
		static constexpr uint32_t AllMipLevels = ~0u;
		static constexpr uint32_t AllArraySlices = ~0u;
	}



	//==========================================================================================
	// RHI Texture
	//  A texture resource is a buffer of image data that can be used for rendering.
	//  Textures can be 1D, 2D, 3D, or cubemaps.
	//===========================================================================================

	struct RHITextureDescriptor
	{
		using ResourceType = class RHITexture;
		ERHITextureDimension Dimension = ERHITextureDimension::Unknown;
		uint32_t Width = 1;     
		uint32_t Height = 1;
		union
		{
			uint32_t Depth = 1; // Number of depth slices in a 3D texture.
			uint32_t ArraySize; // Number of array slices in a 1D or 2D texture array.
		};
		uint32_t Mips = 1; // Number of mip levels in the texture. NOTE: If set to 0, it will be the maximum number of mips for the texture size.
		uint32_t Samples = 1; // Number of samples. Only 2D and 2D array textures can be multisampled.
		Optional<RHIClearValue> ClearValue{};
		ERHIFormat Format = ERHIFormat::Unknown;
		ERHIBindFlags BindFlags = ERHIBindFlags::None;
		ERHIUsage Usage = ERHIUsage::Default;
		ERHICpuAccess CpuAccess = ERHICpuAccess::None;
		String Name{};

		constexpr RHITextureDescriptor() = default;
		constexpr RHITextureDescriptor( 
			StringView a_Name,
			ERHITextureDimension a_Dimension,
			uint32_t a_Width,
			uint32_t a_Height,
			uint32_t a_Depth,
			ERHIFormat a_Format,
			uint32_t a_Mips = RHITextureDescriptor{}.Mips,
			uint32_t a_Samples = RHITextureDescriptor{}.Samples,
			ERHIBindFlags a_BindFlags = RHITextureDescriptor{}.BindFlags,
			ERHIUsage a_Usage = RHITextureDescriptor{}.Usage,
			ERHICpuAccess a_CpuAccess = RHITextureDescriptor{}.CpuAccess,
			Optional<RHIClearValue> a_ClearValue = RHITextureDescriptor{}.ClearValue )
			: Name( a_Name )
			, Dimension( a_Dimension )
			, Width( a_Width )
			, Height( a_Height )
			, Depth( a_Depth )
			, Format( a_Format )
			, Mips( a_Mips )
			, Samples( a_Samples )
			, BindFlags( a_BindFlags )
			, Usage( a_Usage )
			, CpuAccess( a_CpuAccess )
			, ClearValue( a_ClearValue )
		{}

		constexpr bool IsArray() const
		{
			return Dimension == ERHITextureDimension::Texture1DArray
				|| Dimension == ERHITextureDimension::Texture2DArray
				|| Dimension == ERHITextureDimension::TextureCube
				|| Dimension == ERHITextureDimension::TextureCubeArray;
		}

		constexpr bool Is1D() const
		{
			return Dimension == ERHITextureDimension::Texture1D
				|| Dimension == ERHITextureDimension::Texture1DArray;
		}

		constexpr bool Is2D() const
		{
			return Dimension == ERHITextureDimension::Texture2D
				|| Dimension == ERHITextureDimension::Texture2DArray
				|| Dimension == ERHITextureDimension::TextureCube
				|| Dimension == ERHITextureDimension::TextureCubeArray;
		}

		constexpr bool Is3D() const
		{
			return Dimension == ERHITextureDimension::Texture3D;
		}

		constexpr bool IsCube() const
		{
			return Dimension == ERHITextureDimension::TextureCube
				|| Dimension == ERHITextureDimension::TextureCubeArray;
		}

		constexpr auto& SetDimension( ERHITextureDimension a_Dimension ) { Dimension = a_Dimension; return *this; }
		constexpr auto& SetWidth( uint32_t a_Width ) { Width = a_Width; return *this; }
		constexpr auto& SetHeight( uint32_t a_Height ) { Height = a_Height; return *this; }
		constexpr auto& SetDepth( uint32_t a_Depth ) { Depth = a_Depth; return *this; }
		constexpr auto& SetArraySize( uint32_t a_ArraySize ) { ArraySize = a_ArraySize; return *this; }
		constexpr auto& SetFormat( ERHIFormat a_Format ) { Format = a_Format; return *this; }
		constexpr auto& SetMips( uint32_t a_Mips ) { Mips = a_Mips; return *this; }
		constexpr auto& SetSamples( uint32_t a_Samples ) { Samples = a_Samples; return *this; }
		constexpr auto& SetBindFlags( ERHIBindFlags a_BindFlags ) { BindFlags = a_BindFlags; return *this; }
		constexpr auto& SetUsage( ERHIUsage a_Usage ) { Usage = a_Usage; return *this; }
		constexpr auto& SetCpuAccess( ERHICpuAccess a_CpuAccess ) { CpuAccess = a_CpuAccess; return *this; }
		constexpr auto& SetClearValue( Optional<RHIClearValue> a_ClearValue ) { ClearValue = a_ClearValue; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHITexture )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHITexture, ERHIResourceType::Texture );

		RHITexture( const RHITextureDescriptor & a_Desc )
			: m_Desc( a_Desc )
		{
			RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::ConstantBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndirectArgument ),
				std::format( "Texture '{}' cannot be created with bind flags '{}'", a_Desc.Name, ToString( a_Desc.BindFlags ) ) );

			if ( m_Desc.Mips == RHIConstants::AllMipLevels )
			{
				if ( m_Desc.Is1D() )
				{
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width );
				}
				else if ( m_Desc.Is2D() )
				{
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width, m_Desc.Height );
				}
				else if ( m_Desc.Is3D() )
				{
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width, m_Desc.Height, m_Desc.Depth );
				}
				else
				{
					ASSERT( false, "Invalid texture dimension" );
				}
			}
		}

		virtual ~RHITexture() = default;

		// Returns the internal state of the texture.
		ERHIResourceStates State() const { return m_State; }

		// Sets the internal state of the texture.
		// NOTE: This does not perform a state transition. This only sets the internal state of the texture.
		//       Should only be used if manual state transitions have been completed and you want to return state management to the RHI.
		void SetState( ERHIResourceStates a_State ) { m_State = a_State; }

		// The sampler associated with the texture that will be used when bound to a shader.
		// Note: Only required if RHI_SUPPORT_COMBINED_SAMPLERS is true.
		RHISamplerRef Sampler = nullptr;

	private:
		ERHIResourceStates m_State = ERHIResourceStates::Unknown;
	};

	//==========================================================================================
	// RHI Texture Subresource Set
	//  Describes a set of subresources in a texture.
	//==========================================================================================
	struct RHITextureSubresourceSet
	{
		uint32_t BaseMipLevel = 0;
		uint32_t NumMipLevels = 1;
		uint32_t BaseArraySlice = 0;
		uint32_t NumArraySlices = 1;

		constexpr RHITextureSubresourceSet() noexcept = default;
		constexpr RHITextureSubresourceSet( uint32_t a_BaseMipLevel, uint32_t a_NumMipLevels,
			uint32_t a_BaseArraySlice, uint32_t a_NumArraySlices ) noexcept
			: BaseMipLevel( a_BaseMipLevel ), NumMipLevels( a_NumMipLevels ),
			BaseArraySlice( a_BaseArraySlice ), NumArraySlices( a_NumArraySlices ) 
		{}

		constexpr bool operator==( const RHITextureSubresourceSet& a_Other ) const noexcept
		{
			return BaseMipLevel == a_Other.BaseMipLevel && NumMipLevels == a_Other.NumMipLevels &&
				BaseArraySlice == a_Other.BaseArraySlice && NumArraySlices == a_Other.NumArraySlices;
		}

		constexpr bool operator!=( const RHITextureSubresourceSet& a_Other ) const noexcept
		{
			return !operator==( a_Other );
		}

		constexpr bool IsEntireTexture( const RHITextureDescriptor& a_Desc ) const
		{
			return BaseMipLevel == 0 && NumMipLevels == a_Desc.Mips
				&& BaseArraySlice == 0 && NumArraySlices == ( a_Desc.IsArray() ? a_Desc.ArraySize : 1 );
		}

		static constexpr RHITextureSubresourceSet All() noexcept
		{
			return RHITextureSubresourceSet{ 0, RHIConstants::AllMipLevels, 0, RHIConstants::AllArraySlices };
		}
	};

} // namespace Tridium