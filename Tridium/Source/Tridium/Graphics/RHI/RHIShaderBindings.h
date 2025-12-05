#pragma once
#include <Tridium/Graphics/RHI/RHIResource.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHIBuffer.h>
#include <Tridium/Graphics/RHI/RHITexture.h>

namespace Tridium {

	struct ShaderReflectionData;

	namespace RHI {

		extern RHIBindingLayoutDesc BuildLayoutFromShader( String a_Name, const ShaderReflectionData& a_ShaderReflection );

	} // namespace RHI

	//=================================================================================================
	// RHI Shader Binding Layouts
	// An immutable layout that describes what resources can be bound to what slot, in a shader.
	// NOTE: Shader Binding Layouts only describe the layout of the bindings and do not handle the binding of resources.
	// That is handled by the RHIShaderBindingSet.
	//=================================================================================================

	//=================================================================================================
	// RHI Shader Binding: Describes a singular resource binding in a shader binding layout.
	//=================================================================================================
	struct RHIShaderBinding
	{
		//=============================================================================================
		static constexpr uint32_t InvalidSlot = ~0u;

		//=============================================================================================
		// Register index in the shader.
		uint32_t Slot : 16;

		//=============================================================================================
		// Size in bytes for inlined constants, or texture dimension for textures.
		uint32_t SizeOrDimension : 8;

		//=============================================================================================
		// The ERHIBindingType packed into a uint32_t. Call Type() to get the enum value
		uint32_t PackedType : 8;

		//=============================================================================================
		// Hash of the binding name. NOTE: This is set by the RHIShaderBindingLayout and is not set here.
		hash_t NameHash;

		//=============================================================================================
		// Default constructor does not initialize the binding for performance,
		// as RHIShaderBinding are stored in a large fixed size array.
		constexpr RHIShaderBinding() {}

		//=============================================================================================
		// Returns the type of the binding.
		constexpr ERHIBindingType Type() const
		{
			return Cast<ERHIBindingType>( PackedType );
		}

		//=============================================================================================
		// If this is an inlined constant binding, returns the size in bytes of the inlined constants.
		constexpr uint16_t InlinedConstantSize() const
		{
			if ( Type() == ERHIBindingType::InlinedConstants )
			{
				return SizeOrDimension;
			}

			return 0;
		}

		//=============================================================================================
		// If this is a texture or storage texture binding, returns the texture dimension.
		constexpr ERHITextureDimension TextureDimension() const
		{
			if ( Type() == ERHIBindingType::Texture || Type() == ERHIBindingType::StorageTexture )
			{
				return Cast<ERHITextureDimension>( SizeOrDimension );
			}

			return ERHITextureDimension::Unknown;
		}

		//=============================================================================================
		constexpr bool Valid() const
		{
			return Slot != InvalidSlot && Type() != ERHIBindingType::Unknown;
		}

		//=============================================================================================
		constexpr operator bool() const
		{
			return Valid();
		}

		//=============================================================================================
		constexpr bool operator==( const RHIShaderBinding& a_Other ) const
		{
			return Slot == a_Other.Slot && SizeOrDimension == a_Other.SizeOrDimension && Type() == a_Other.Type();
		}

		//=============================================================================================
		constexpr bool operator!=( const RHIShaderBinding& a_Other ) const
		{
			return !operator==( a_Other );
		}

		//=============================================================================================
		// This is used for creating an invalid binding.
		// As the default constructor does not initialize the memory.
		[[nodiscard]] static constexpr RHIShaderBinding Invalid()
		{
			RHIShaderBinding binding;
			binding.Slot = InvalidSlot;
			binding.SizeOrDimension = 0;
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::Unknown );
			return binding;
		}

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding InlinedConstants( const uint16_t a_Size )
		{
			RHIShaderBinding binding;
			binding.Slot = 0;
			binding.SizeOrDimension = a_Size;
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::InlinedConstants );
			return binding;
		}

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding ConstantBuffer( const uint32_t a_Slot )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = 0;
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::ConstantBuffer );
			return binding;
		};

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding StructuredBuffer( const uint32_t a_Slot )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = 0;
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::StructuredBuffer );
			return binding;
		};

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding StorageBuffer( const uint32_t a_Slot )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = 0;
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::StorageBuffer );
			return binding;
		};

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding Texture( const uint32_t a_Slot, ERHITextureDimension a_Dimension )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = Cast<uint32_t>( a_Dimension );
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::Texture );
			return binding;
		}

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding StorageTexture( const uint32_t a_Slot, ERHITextureDimension a_Dimension )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = Cast<uint32_t>( a_Dimension );
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::StorageTexture );
			return binding;
		}

		//=============================================================================================
		[[nodiscard]] static constexpr RHIShaderBinding BindlessTextureArray( const uint32_t a_Slot, ERHITextureDimension a_Dimension )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.SizeOrDimension = Cast<uint32_t>( a_Dimension );
			binding.PackedType = Cast<uint32_t>( ERHIBindingType::BindlessTextureArray );
			return binding;
		}

	};

	//=================================================================================================
	static_assert( sizeof( RHIShaderBinding ) == 8, "RHIShaderBinding size is not 8 bytes" );
	using RHIShaderBindingArray = InlineArray<RHIShaderBinding, RHIConstants::MaxShaderBindings>;

	//=================================================================================================
	// RHI Binding Layout Descriptor:
	//=================================================================================================
	struct RHIBindingLayoutDesc
	{
		using ResourceType = class IRHIBindingLayout;
		uint32_t RegisterSpace = 0;
		ERHIShaderVisibility Visibility = ERHIShaderVisibility::All; // Visibility of the binding. (e.g. Vertex, Pixel, Compute, etc.)
		RHIShaderBindingArray Bindings{};
		UnorderedMap<hash_t, Pair<uint32_t, String>> BindingMap{}; // Maps a hashed binding name to the index and string name of an item in the Bindings array.
		String Name{};

		//=============================================================================================
		constexpr auto& SetVisibility( ERHIShaderVisibility a_Visibility ) { Visibility = a_Visibility; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }

		//=============================================================================================
		// Users should only use this function to add bindings, as the BindingMap needs to be updated.
		auto& AddBinding( const HashedString& a_Name, RHIShaderBinding a_Binding, uint32_t a_InputIndex = 0 )
		{
			if ( a_InputIndex == 0 && Bindings.Size() > 0 )
			{
				a_InputIndex = Bindings.Size();
			}

			if ( Bindings.Size() <= a_InputIndex )
			{
				Bindings.Resize( a_InputIndex + 1 );
			}

			RHI_DEV_CHECK( BindingMap.find( a_Name.Hash() ) == BindingMap.end(), "Binding with name hash already exists" );
			BindingMap[ a_Name.Hash() ] = Pair{ a_InputIndex, a_Name.String() };

			Bindings[ a_InputIndex ] = a_Binding;
			Bindings[ a_InputIndex ].NameHash = a_Name.Hash();
			return *this;
		}

		//=============================================================================================
		// Users should only use this function to remove bindings, as the BindingMap needs to be updated.
		void RemoveBinding( HashedString a_Name )
		{
			if ( Bindings.Empty() )
				return;

			auto it = BindingMap.find( a_Name.Hash() );
			if ( it == BindingMap.end() )
				return;

			const uint32_t index = it->second.first;
			if ( index == Bindings.Size() - 1 )
			{
				// The binding is at the end of the array, just pop it.
				Bindings.PopBack();
			}
			else
			{
				// The binding is not at the end of the array, swap it with the last element.
				Bindings[index] = Bindings[Bindings.Size() - 1];
				Bindings.PopBack();

				// Update the index of the swapped element in the BindingMap.
				auto swappedIt = BindingMap.find( Bindings[index].NameHash );
				ASSERT( swappedIt != BindingMap.end(), "Binding with name hash does not exist, must be an invalid name binding somewhere" );
				swappedIt->second.first = index;
			}

			BindingMap.erase( it );
		}

		//=============================================================================================
		// Get the index of a binding in the layout.
		// Returns -1 if the binding does not exist.
		[[nodiscard]] int32_t GetBindingIndex( hash_t a_Name ) const
		{
			auto it = BindingMap.find( a_Name );
			if ( it != BindingMap.end() )
			{
				return static_cast<int32_t>( it->second.first );
			}
			return -1;
		}

		//=============================================================================================
		// Get the binding at the specified index.
		// Returns an invalid binding if the index is out of range.
		[[nodiscard]] RHIShaderBinding GetBindingFromName( hash_t a_Name ) const
		{
			int32_t index = GetBindingIndex( a_Name );
			RHI_DEV_CHECK( index != -1, "Binding with name hash does not exist" );
			return index >= 0 ? Bindings[index] : RHIShaderBinding::Invalid();
		}

		//=============================================================================================
		// Returns the string name of the binding from the given name hash.
		[[nodiscard]] StringView GetBindingName( hash_t a_Name ) const
		{
			auto it = BindingMap.find( a_Name );
			if ( it != BindingMap.end() )
			{
				return it->second.second;
			}
			return StringView{};
		}

		//=============================================================================================
		// Appends the bindings from another layout descriptor into this one.
		void Append( const RHIBindingLayoutDesc& a_Other )
		{
			for ( const auto& [nameHash, pair] : a_Other.BindingMap )
			{
				AddBinding( HashedString{ pair.second }, a_Other.Bindings[pair.first] );
			}
		}
	};

	//=================================================================================================
	// RHI Binding Layout Interface
	//=================================================================================================
	class IRHIBindingLayout : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( BindingLayout )
		IRHIBindingLayout( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
			: IRHIObject( a_Device ), m_Desc( a_Desc ) {}
		virtual ~IRHIBindingLayout() = default;
	};

	//=================================================================================================
	// RHI Binding Set Item: Represents a single shader binding in a set.
	//=================================================================================================
	struct RHIBindingSetItem
	{
		IRHIResource* Resource;
		uint32_t Slot;
		ERHIBindingType Type;
		ERHIFormat Format;

		union
		{
			ERHITextureDimension TextureDimension;
			ERHIBufferType BufferType;
		};

		RHIPackedSampler Sampler;

		union
		{
			RHITextureSubresourceSet Subresources; // Valid only for textures.
			RHIBufferRange Range; // Valid only for buffers.
			uint64_t RawData[2];
		};

		bool operator==( const RHIBindingSetItem& a_Other ) const
		{
			return std::memcmp( this, &a_Other, sizeof( RHIBindingSetItem ) ) == 0;
		}

		bool operator!=( const RHIBindingSetItem& a_Other ) const
		{
			return !operator==( a_Other );
		}

		// Default constructor does not initialize the item for performance, as RHIBindingSetItem are stored in a large fixed size array.
		// Use the static None() function to create an empty item.
		RHIBindingSetItem() {}

		static RHIBindingSetItem None( uint32_t a_Slot = RHIShaderBinding::InvalidSlot )
		{
			RHIBindingSetItem item;
			item.Resource = nullptr;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::Unknown;
			item.Format = ERHIFormat::Unknown;
			item.TextureDimension = ERHITextureDimension::Unknown;
			item.Sampler = RHIPackedSampler{};
			item.RawData[0] = 0; item.RawData[1] = 0;
			return item;
		}
	};
	static_assert(sizeof( RHIBindingSetItem ) <= 48, "RHIBindingSetItem size is not 48 bytes");
	using RHIBindingSetItemArray = InlineArray<RHIBindingSetItem, RHIConstants::MaxShaderBindings>;

	//=================================================================================================
	// RHI Shader Binding Set: A set of shader bindings that can be bound to a command list.
	//=================================================================================================
	struct RHIBindingSetDesc
	{
		using ResourceType = class IRHIBindingSet;
		InlineArray<RHIBindingSetItem, RHIConstants::MaxShaderBindings> Bindings{};
		RHIBindingLayoutRef Layout{}; // The layout that this binding set is based on.
		String Name{};

		RHIBindingSetDesc() = default;
		RHIBindingSetDesc( const RHIBindingLayoutRef& a_Layout )
			: Layout( a_Layout ) {}

		auto& SetName ( String a_Name )
		{
			Name = std::move( a_Name );
			return *this;
		}

		auto& SetLayout( const RHIBindingLayoutRef& a_Layout )
		{
			Layout = a_Layout;
			return *this;
		}

		auto& AddBinding( const RHIBindingSetItem& a_Binding )
		{
			Bindings.PushBack( a_Binding );
			return *this;
		}

		auto& AddConstantBuffer(
			uint32_t a_Slot, IRHIBuffer* a_Buffer,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
			item.Resource = a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::ConstantBuffer;
			item.Range = a_Range;
			return *this;
		}

		auto& AddStructuredBuffer(
			uint32_t a_Slot, IRHIBuffer* a_Buffer,
			ERHIBufferType a_BufferType = ERHIBufferType::Unknown,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
			item.Resource = a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::StructuredBuffer;
			item.Range = a_Range;
			item.BufferType = a_BufferType == ERHIBufferType::Unknown && a_Buffer ? a_Buffer->Desc().Type : a_BufferType;
			item.Format = a_Format;
			return *this;
		}

		auto& AddStorageBuffer(
			uint32_t a_Slot, IRHIBuffer* a_Buffer,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
			item.Resource = a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::StorageBuffer;
			item.Range = a_Range;
			return *this;
		}

		auto& AddTexture(
			uint32_t a_Slot, IRHITexture* a_Texture, const RHISampler* a_Sampler = nullptr,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			ERHITextureDimension a_TextureDimension = ERHITextureDimension::Unknown,
			RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
			item.Resource = a_Texture;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::Texture;
			item.Subresources = a_Subresources;
			item.Format = a_Format;
			item.TextureDimension = a_TextureDimension == ERHITextureDimension::Unknown && a_Texture ? a_Texture->Desc().Dimension : a_TextureDimension;
			if ( a_Sampler )
				item.Sampler = RHIPackedSampler::Pack( *a_Sampler );
			return *this;
		}

		auto& AddStorageTexture(
			uint32_t a_Slot, IRHITexture* a_Texture, RHISampler* a_Sampler = nullptr,
			RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
			item.Resource = a_Texture;
			item.Slot = a_Slot;
			item.Type = ERHIBindingType::StorageTexture;
			item.Subresources = a_Subresources;
			if ( a_Sampler )
				item.Sampler = RHIPackedSampler::Pack( *a_Sampler );
			return *this;
		}

		// Add an array of textures for bindless rendering
		// For bindless texture arrays, each texture gets its own descriptor at consecutive indices
		// starting from the base slot.
		auto& AddBindlessTextureArray(
			uint32_t a_BaseSlot, Span<IRHITexture* const> a_Textures,
			const RHISampler* a_Sampler = nullptr,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			ERHITextureDimension a_TextureDimension = ERHITextureDimension::Unknown )
		{
			for ( size_t i = 0; i < a_Textures.size(); ++i )
			{
				if ( a_Textures[i] != nullptr )
				{
					RHIBindingSetItem& item = Bindings.EmplaceBack( RHIBindingSetItem::None() );
					item.Resource = a_Textures[i];
					item.Slot = a_BaseSlot + static_cast<uint32_t>( i );
					item.Type = ERHIBindingType::BindlessTextureArray;
					item.Subresources = RHITextureSubresourceSet::All();
					item.Format = a_Format;
					item.TextureDimension = a_TextureDimension == ERHITextureDimension::Unknown && a_Textures[i]
						? a_Textures[i]->Desc().Dimension
						: a_TextureDimension;
					if ( a_Sampler )
						item.Sampler = RHIPackedSampler::Pack( *a_Sampler );
				}
			}
			return *this;
		}

		//==========================================
		// Convenience functions for adding bindings by name.
		// These functions are only available if the layout is set.
		//===========================================

		auto& AddConstantBuffer(
			HashedString a_Name, IRHIBuffer* a_Buffer,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::ConstantBuffer );
			return AddConstantBuffer( binding.Slot, a_Buffer, a_Range );
		}

		auto& AddStructuredBuffer(
			HashedString a_Name, IRHIBuffer* a_Buffer,
			ERHIBufferType a_BufferType = ERHIBufferType::Unknown,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::StructuredBuffer );
			return AddStructuredBuffer( binding.Slot, a_Buffer, a_BufferType, a_Format, a_Range );
		}

		auto& AddStorageBuffer(
			HashedString a_Name, IRHIBuffer* a_Buffer,
			RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::StorageBuffer );
			return AddStorageBuffer( binding.Slot, a_Buffer, a_Range );
		}

		auto& AddTexture(
			HashedString a_Name, IRHITexture* a_Texture,
			const RHISampler* a_Sampler = nullptr,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			ERHITextureDimension a_TextureDimension = ERHITextureDimension::Unknown,
			RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::Texture );
			return AddTexture( binding.Slot, a_Texture, a_Sampler, a_Format, a_TextureDimension, a_Subresources );
		}

		auto& AddStorageTexture(
			HashedString a_Name, IRHITexture* a_Texture,
			RHISampler* a_Sampler = nullptr,
			RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::StorageTexture );
			return AddStorageTexture( binding.Slot, a_Texture, a_Sampler, a_Subresources );
		}

		auto& AddBindlessTextureArray(
			HashedString a_Name, Span<IRHITexture* const> a_Textures,
			const RHISampler* a_Sampler = nullptr,
			ERHIFormat a_Format = ERHIFormat::Unknown,
			ERHITextureDimension a_TextureDimension = ERHITextureDimension::Unknown )
		{
			RHI_DEV_CHECK( Layout != nullptr, "Layout is null!" );
			auto binding = Layout->Desc().GetBindingFromName( a_Name.Hash() );
			ValidateBinding( binding, ERHIBindingType::BindlessTextureArray );
			return AddBindlessTextureArray( binding.Slot, a_Textures, a_Sampler, a_Format, a_TextureDimension );
		}

		bool operator==( const RHIBindingSetDesc& a_Other ) const
		{
			if ( Bindings.Size() != a_Other.Bindings.Size() )
				return false;

			for ( size_t i = 0; i < Bindings.Size(); ++i )
			{
				if ( Bindings[i] != a_Other.Bindings[i] )
					return false;
			}

			return true;
		}

		bool operator!=( const RHIBindingSetDesc& a_Other ) const
		{
			return !(operator==( a_Other ));
		}

	private:
		void ValidateBinding( RHIShaderBinding& a_Binding, ERHIBindingType a_RequiredType )
		{
			RHI_DEV_CHECK( a_Binding.Valid(), "Binding is invalid!" );
			RHI_DEV_CHECK( a_Binding.Type() == a_RequiredType, "Binding type mismatch!" );
		}
	};

	//==============================================
	// RHI Shader Binding Set Interface
	class IRHIBindingSet : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( BindingSet );

		IRHIBindingSet( IDynamicRHI* a_Device, const RHIBindingSetDesc& a_Desc)
			: IRHIObject( a_Device ), m_Desc( a_Desc )
		{
			ENSURE( m_Desc.Layout != nullptr, "Binding layout is null!" );
			RHI_DEV_CHECK( a_Desc.Bindings.Size() <= m_Desc.Layout->Desc().Bindings.Size(), "Binding set has more bindings than the provided layout!" );

			// We want to store hard references to the resources in the binding set.
			m_ResourceHandles.Reserve( m_Desc.Bindings.Size() );
			for ( const RHIBindingSetItem& binding : m_Desc.Bindings )
			{
				if ( binding.Resource != nullptr )
				{
					RHI_DEV_CHECK( binding.Resource->Type() == ERHIObjectType::Texture
						|| binding.Resource->Type() == ERHIObjectType::Buffer,
						std::format( "Invalid resource type '{}' in shader binding set '{}'", ToString( binding.Resource->Type() ), a_Desc.Name ) );

					m_ResourceHandles.EmplaceBack( std::move( binding.Resource->Shared() ) );
				}
			}
		}

	protected:
		Array<RHIObjectRef> m_ResourceHandles{};
	};

} // namespace Tridium