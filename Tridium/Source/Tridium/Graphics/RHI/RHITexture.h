#pragma once
#include "RHIResource.h"

namespace Tridium {

	// Forward declarations
	using RHISamplerRef = SharedPtr<class IRHISampler>;

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
		const void* Data = nullptr;
		// The stride of a row of data in the texture, in bytes. For 2D and 3D textures.
		size_t RowStride = 0;
		// The stride of a slice of data in the texture, in bytes. For 3D textures.
		size_t DepthStride = 0;
	};

	//==========================================================================================
	// RHI Texture Slice
	//  Describes a region (2D or 3D) of a single mip level and array slice of a texture.
	struct RHITextureSlice
	{
		uint32_t OffsetX = 0;
		uint32_t OffsetY = 0;
		uint32_t OffsetZ = 0;

		uint32_t Width = ~0;
		uint32_t Height = ~0;
		uint32_t Depth = ~0;

		uint32_t MipLevel = 0;
		uint32_t ArraySlice = 0;
	};

	struct RHIFramebuffer
	{
		struct Attachment
		{
			IRHITexture* Texture = nullptr;
			bool ReadOnly = false;

			operator bool() const { return Texture != nullptr; }

			bool operator==( const Attachment& a_Other ) const
			{
				return Texture == a_Other.Texture && ReadOnly == a_Other.ReadOnly;
			}

			bool operator!=( const Attachment& a_Other ) const
			{
				return !operator==( a_Other );
			}
		};

		InlineArray<Attachment, RHIConstants::MaxColorTargets> ColorAttachments{};
		Attachment DepthStencilAttachment{};

		bool operator==( const RHIFramebuffer& a_Other ) const
		{
			if ( DepthStencilAttachment != a_Other.DepthStencilAttachment )
				return false;

			if ( ColorAttachments.Size() != a_Other.ColorAttachments.Size() )
				return false;

			for ( size_t i = 0; i < ColorAttachments.Size(); ++i )
			{
				if ( ColorAttachments[i] != a_Other.ColorAttachments[i] )
					return false;
			}
		}
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

	struct RHITextureDesc
	{
		using ResourceType = class IRHITexture;
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

		constexpr RHITextureDesc() = default;
		constexpr RHITextureDesc( 
			StringView a_Name,
			ERHITextureDimension a_Dimension,
			uint32_t a_Width,
			uint32_t a_Height,
			uint32_t a_Depth,
			ERHIFormat a_Format,
			uint32_t a_Mips = RHITextureDesc{}.Mips,
			uint32_t a_Samples = RHITextureDesc{}.Samples,
			ERHIBindFlags a_BindFlags = RHITextureDesc{}.BindFlags,
			ERHIUsage a_Usage = RHITextureDesc{}.Usage,
			ERHICpuAccess a_CpuAccess = RHITextureDesc{}.CpuAccess,
			Optional<RHIClearValue> a_ClearValue = RHITextureDesc{}.ClearValue )
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
		constexpr auto& SetClearValue( RHIClearValue a_ClearValue ) { ClearValue.emplace( a_ClearValue ); return *this; }
		          auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	class IRHITexture : public IRHIResource
	{
		RHI_OBJECT_INTERFACE_BODY( Texture );

		IRHITexture( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
			: IRHIResource( a_Device ), m_Desc( a_Desc )
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

		virtual ~IRHITexture() = default;

		// The sampler associated with the texture that will be used when bound to a shader.
		// Note: Only required if RHI_SUPPORT_COMBINED_SAMPLERS is true.
		RHISamplerRef Sampler = nullptr;
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

		constexpr bool IsEntireTexture( const RHITextureDesc& a_Desc ) const
		{
			return BaseMipLevel == 0 && NumMipLevels == a_Desc.Mips
				&& BaseArraySlice == 0 && NumArraySlices == ( a_Desc.IsArray() ? a_Desc.ArraySize : 1 );
		}

		constexpr RHITextureSubresourceSet Resolve( const RHITextureDesc& a_Desc, bool a_SingleMipLevel )
		{
			RHITextureSubresourceSet result = *this;

			if ( a_SingleMipLevel )
			{
				result.NumMipLevels = 1;
			}
			else
			{
				int lastMipLevel = Math::Min( BaseMipLevel + NumMipLevels, a_Desc.Mips );
				result.NumMipLevels = uint32_t( Math::Max( 0u, lastMipLevel - BaseMipLevel ) );
			}

			switch ( a_Desc.Dimension )
			{
			case ERHITextureDimension::Texture1DArray:
			case ERHITextureDimension::Texture2DArray:
			case ERHITextureDimension::TextureCube:
			case ERHITextureDimension::TextureCubeArray:
			{
				int lastArraySlice = Math::Min( BaseArraySlice + NumArraySlices, a_Desc.ArraySize );
				result.NumArraySlices = uint32_t( Math::Max( 0u, lastArraySlice - BaseArraySlice ) );
				break;
			}
			default:
				result.BaseArraySlice = 0;
				result.NumArraySlices = 1;
				break;
			}

			return result;
		}

		static constexpr RHITextureSubresourceSet All() noexcept
		{
			return RHITextureSubresourceSet{ 0, RHIConstants::AllMipLevels, 0, RHIConstants::AllArraySlices };
		}
	};

} // namespace Tridium