#pragma once
#include "RHIResource.h"
#include "RHISampler.h"

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

		constexpr auto& SetData( const void* a_Data ) { Data = a_Data; return *this; }
		constexpr auto& SetRowStride( size_t a_RowStride ) { RowStride = a_RowStride; return *this; }
		constexpr auto& SetDepthStride( size_t a_DepthStride ) { DepthStride = a_DepthStride; return *this; }
	};

	//==========================================================================================
	// RHI Texture Slice
	//  Describes a region (2D or 3D) of a single mip level and array slice of a texture.
	struct RHITextureSlice
	{
		uint32_t OffsetX = 0;
		uint32_t OffsetY = 0;
		uint32_t OffsetZ = 0;

		uint32_t Width = ~0u;
		uint32_t Height = ~0u;
		uint32_t Depth = ~0u;

		uint32_t MipLevel = 0;
		uint32_t ArraySlice = 0;

		constexpr RHITextureSlice Resolve( const RHITextureDesc& a_TextureDesc ) const;

		constexpr bool operator==( const RHITextureSlice& a_Other ) const
		{
			return OffsetX == a_Other.OffsetX &&
				OffsetY == a_Other.OffsetY &&
				OffsetZ == a_Other.OffsetZ &&
				Width == a_Other.Width &&
				Height == a_Other.Height &&
				Depth == a_Other.Depth &&
				MipLevel == a_Other.MipLevel &&
				ArraySlice == a_Other.ArraySlice;
		}

		static constexpr RHITextureSlice EntireTexture()
		{
			return RHITextureSlice{ 0, 0, 0, ~0u, ~0u, ~0u, 0, 0 };
		}

		constexpr auto& SetOffsetX( uint32_t a_OffsetX ) { OffsetX = a_OffsetX; return *this; }
		constexpr auto& SetOffsetY( uint32_t a_OffsetY ) { OffsetY = a_OffsetY; return *this; }
		constexpr auto& SetOffsetZ( uint32_t a_OffsetZ ) { OffsetZ = a_OffsetZ; return *this; }
		constexpr auto& SetWidth( uint32_t a_Width ) { Width = a_Width; return *this; }
		constexpr auto& SetHeight( uint32_t a_Height ) { Height = a_Height; return *this; }
		constexpr auto& SetDepth( uint32_t a_Depth ) { Depth = a_Depth; return *this; }
		constexpr auto& SetMipLevel( uint32_t a_MipLevel ) { MipLevel = a_MipLevel; return *this; }
		constexpr auto& SetArraySlice( uint32_t a_ArraySlice ) { ArraySlice = a_ArraySlice; return *this; }

		constexpr auto& SetOffset( uint32_t a_OffsetX, uint32_t a_OffsetY = 0, uint32_t a_OffsetZ = 0 )
		{
			OffsetX = a_OffsetX;
			OffsetY = a_OffsetY;
			OffsetZ = a_OffsetZ;
			return *this;
		}

		constexpr auto& SetSize( uint32_t a_Width, uint32_t a_Height = 1, uint32_t a_Depth = 1 )
		{
			Width = a_Width;
			Height = a_Height;
			Depth = a_Depth;
			return *this;
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
		// The dimension of the texture.
		ERHITextureDimension Dimension = ERHITextureDimension::Unknown;
		// Width of the texture. Valid for all texture dimensions.
		uint32_t Width = 0;
		// Height of the texture. Valid for 2D, 2D array, and cubemap textures.
		uint32_t Height = 0;
		// Depth or array size of the texture. Valid for 3D textures and array textures.
		uint32_t DepthOrArraySize = 1;
		// Number of mip levels in the texture. NOTE: If set to 0, it will be the maximum number of mips for the texture size.
		uint32_t Mips = 1;
		// Number of samples. Only 2D and 2D array textures can be multisampled.
		uint32_t Samples = 1;
		// Optimised clear value for the Render target/ Depth stencil texture. NOTE: 'UseClearValue' must be true for this to be used.
		RHIClearValue ClearValue{};
		// Indicates if 'ClearValue' should be used when the texture is created.
		bool UseClearValue = false;
		// The data format of each texel in the texture.
		ERHIFormat Format = ERHIFormat::Unknown;
		// Specifies how the texture can be bound in the pipeline.
		ERHIBindFlags BindFlags = ERHIBindFlags::None;
		// Specifies how often the texture will be modified.
		ERHIUsage Usage = ERHIUsage::Default;
		// Specifies how the texture can be accessed by the CPU.
		ERHICpuAccess CpuAccess = ERHICpuAccess::None;
		// Initial state of the texture when created. This is used to optimise the creation of the texture.
		ERHIResourceStates InitialState = ERHIResourceStates::Common;
		// Default sampler to use when sampling this texture in shaders.
		RHISampler DefaultSampler{};
		// Debug name for the texture.
		String Name{};

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
		constexpr auto& SetDepth( uint32_t a_Depth ) { DepthOrArraySize = a_Depth; return *this; }
		constexpr auto& SetArraySize( uint32_t a_ArraySize ) { DepthOrArraySize = a_ArraySize; return *this; }
		constexpr auto& SetFormat( ERHIFormat a_Format ) { Format = a_Format; return *this; }
		constexpr auto& SetMips( uint32_t a_Mips ) { Mips = a_Mips; return *this; }
		constexpr auto& SetSamples( uint32_t a_Samples ) { Samples = a_Samples; return *this; }
		constexpr auto& SetBindFlags( ERHIBindFlags a_BindFlags ) { BindFlags = a_BindFlags; return *this; }
		constexpr auto& SetUsage( ERHIUsage a_Usage ) { Usage = a_Usage; return *this; }
		constexpr auto& SetCpuAccess( ERHICpuAccess a_CpuAccess ) { CpuAccess = a_CpuAccess; return *this; }
		constexpr auto& SetDefaultSampler( const RHISampler& a_Sampler ) { DefaultSampler = a_Sampler; return *this; }
		constexpr auto& SetInitialState( ERHIResourceStates a_State ) { InitialState = a_State; return *this; }
		constexpr auto& SetClearValue( RHIClearValue a_ClearValue ) { ClearValue = a_ClearValue; return *this; }
		constexpr auto& SetUseClearValue( bool a_UseClearValue ) { UseClearValue = a_UseClearValue; return *this; }
		          auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	class IRHITexture : public IRHIResource
	{
		RHI_OBJECT_INTERFACE_BODY( Texture );

		IRHITexture( IDynamicRHI* a_Device, const RHITextureDesc& a_Desc )
			: IRHIResource( a_Device ), m_Desc( a_Desc )
		{
			RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::VertexBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndexBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::ConstantBuffer )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::IndirectArgument ),
				std::format( "Texture '{}' cannot be created with bind flags '{}'", a_Desc.Name, ToString( a_Desc.BindFlags ) ) );

			RHI_DEV_CHECK( m_Desc.Format != ERHIFormat::Unknown,
						   "Texture '{}' cannot be created with unknown format", a_Desc.Name );

			//RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil )
			//	|| GetRHIFormatInfo( a_Desc.Format ).HasDepth 
			//	|| GetRHIFormatInfo( a_Desc.Format ).HasStencil,
			//	"Texture '{}' cannot be created with bind flag 'DepthStencil' and format '{}'",
			//	a_Desc.Name, GetRHIFormatInfo( a_Desc.Format ).Name );

			RHI_DEV_WARN( !a_Desc.UseClearValue 
				|| EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::RenderTarget )
				|| EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil ),
				"Clear value will not be used for texture '{}' as it is not a render target or depth stencil texture", a_Desc.Name );

			if ( m_Desc.Mips == RHIConstants::AllMipLevels )
			{
				if ( m_Desc.Is1D() )
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width );
				else if ( m_Desc.Is2D() )
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width, m_Desc.Height );
				else if ( m_Desc.Is3D() )
					m_Desc.Mips = CalculateMipLevelCount( m_Desc.Width, m_Desc.Height, m_Desc.DepthOrArraySize );
				else
					ASSERT( false, "Invalid texture dimension" );
			}
		}

		virtual ~IRHITexture() = default;
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
				&& BaseArraySlice == 0 && NumArraySlices == ( a_Desc.IsArray() ? a_Desc.DepthOrArraySize : 1 );
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
				int lastArraySlice = Math::Min( BaseArraySlice + NumArraySlices, a_Desc.DepthOrArraySize );
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


	struct RHIFramebuffer
	{
		struct Attachment
		{
			RHITextureRef Texture = nullptr;
			bool ReadOnly = false;

			operator bool() const { return Texture != nullptr; }

			bool operator==( const Attachment& a_Other ) const
			{
				return Texture == a_Other.Texture && ReadOnly == a_Other.ReadOnly;
			}
		};

		InlineArray<Attachment, RHIConstants::MaxColorTargets> ColorAttachments{};
		Attachment DepthStencilAttachment{};

		bool Valid() const
		{
			return !ColorAttachments.Empty() || ( bool )DepthStencilAttachment;
		}

		auto& AddColorAttachment( RHITextureRef a_Texture, bool a_ReadOnly = false )
		{
			RHI_DEV_CHECK( ColorAttachments.Size() < ColorAttachments.MaxSize(),
						   "Maximum number of color attachments exceeded!" );

			ColorAttachments.PushBack( Attachment{ a_Texture, a_ReadOnly } );

			return *this;
		}

		auto& SetDepthStencilAttachment( RHITextureRef a_Texture, bool a_ReadOnly = false )
		{
			DepthStencilAttachment = Attachment{ a_Texture, a_ReadOnly };
			return *this;
		}

		operator bool() const
		{
			return Valid();
		}

		bool operator==( const RHIFramebuffer& a_Other ) const
		{
			if ( DepthStencilAttachment != a_Other.DepthStencilAttachment )
				return false;

			if ( ColorAttachments.Size() != a_Other.ColorAttachments.Size() )
				return false;

			for ( size_t i = 0; i < ColorAttachments.Size(); ++i )
			{
				if ( ColorAttachments[ i ] != a_Other.ColorAttachments[ i ] )
					return false;
			}

			return true;
		}
	};

	inline constexpr RHITextureSlice RHITextureSlice::Resolve( const RHITextureDesc& a_TextureDesc ) const
	{
		RHITextureSlice result = *this;
		result.Width = Width == ~0u ? a_TextureDesc.Width - OffsetX : Width;
		result.Height = Height == ~0u ? a_TextureDesc.Height - OffsetY : Height;
		result.Depth = Depth == ~0u ? a_TextureDesc.DepthOrArraySize - OffsetZ : Depth;
		result.OffsetX = Math::Min( OffsetX, a_TextureDesc.Width );
		result.OffsetY = Math::Min( OffsetY, a_TextureDesc.Height );
		result.OffsetZ = Math::Min( OffsetZ, a_TextureDesc.DepthOrArraySize );
		result.MipLevel = MipLevel < a_TextureDesc.Mips ? MipLevel : 0;
		result.ArraySlice = ArraySlice < a_TextureDesc.DepthOrArraySize ? ArraySlice : 0;
		return result;
	}

} // namespace Tridium