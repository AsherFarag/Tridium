#pragma once
#include "RHIResource.h"
#include "RHISampler.h"
#include "RHIBuffer.h"
#include "RHITexture.h"

namespace Tridium {

	//==============================================
	// RHI Shader Binding Type
	//  Describes the type of shader resource bound to a shader stage.
	//  Maps to concepts in D3D12 Root Signatures and Vulkan Descriptor Sets.
	enum class ERHIShaderBindingType : uint8_t
	{
		Unknown = 0,

		// Inlined scalar/vector constants embedded directly into the root signature or push constant block.
		InlinedConstants, // (DX12: Root Constants) / (Vulkan: Push Constants)

		// A constant buffer resource (Uniform Buffer in Vulkan).
		ConstantBuffer,   // (DX12: CBV) / (Vulkan: Uniform Buffer)

		// A read-only structured or raw buffer.
		StructuredBuffer, // (DX12: SRV) / (Vulkan: Storage Buffer w/ read-only access)

		// A read/write structured or raw buffer.
		StorageBuffer,    // (DX12: UAV) / (Vulkan: Storage Buffer)

		// A read-only texture.
		Texture,          // (DX12: SRV) / (Vulkan: Sampled Image)

		// A read/write texture.
		StorageTexture,   // (DX12: UAV) / (Vulkan: Storage Image)

		// A sampler state object.
		Sampler,          // (DX12: Sampler) / (Vulkan: Sampler)

		// A combined image-sampler object. ( Required for OpenGL )
		CombinedSampler,  // (DX12: SRV + Sampler manually combined) / (Vulkan: Combined Image Sampler)
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// RHI Shader Binding Layouts
	//  An immutable layout that describes what resources can be bound to what slot, in a shader.
	//  NOTE: Shader Binding Layouts only describe the layout of the bindings and do not handle the binding of resources.
	//        That is handled by the RHIShaderBindingSet.
	/////////////////////////////////////////////////////////////////////////////////////////////////

	//==============================================
	// RHI Shader Binding
	//  Describes a singular resource binding in a shader binding layout.
	struct RHIShaderBinding
	{
		static constexpr uint16_t InvalidSlot = ~0u;

		uint32_t Slot : 16; // Register index in the shader.
		uint32_t Size : 8;  // Size in bytes of the binding.
		uint32_t PackedType : 8;
		hash_t NameHash; // Hash of the binding name. NOTE: This is set by the RHIShaderBindingLayout and is not set here.

		// Default constructor does not initialize the binding for performance, as RHIShaderBinding are stored in a large fixed size array.
		constexpr RHIShaderBinding() {}

		constexpr ERHIShaderBindingType Type() const
		{
			return Cast<ERHIShaderBindingType>(PackedType);
		}

		constexpr bool Valid() const
		{
			return Slot != InvalidSlot && Type() != ERHIShaderBindingType::Unknown;
		}

		constexpr operator bool() const
		{
			return Valid();
		}

		constexpr bool operator==( const RHIShaderBinding& a_Other ) const
		{
			return Slot == a_Other.Slot && Size == a_Other.Size && Type() == a_Other.Type();
		}

		constexpr bool operator!=( const RHIShaderBinding& a_Other ) const
		{
			return !operator==( a_Other );
		}

	#define RHI_SHADER_BINDING_INITIALIZER( _Type ) \
		constexpr RHIShaderBinding& As##_Type( const uint32_t a_Slot ) \
		{ \
			Slot = a_Slot; \
			Size = sizeof( uint32_t ); \
			PackedType = Cast<uint32_t>( ERHIShaderBindingType::_Type ); \
			return *this; \
		} \
		[[nodiscard]] static constexpr RHIShaderBinding _Type( const uint32_t a_Slot ) \
		{ \
			return RHIShaderBinding{}.As##_Type( a_Slot ); \
		}

		RHI_SHADER_BINDING_INITIALIZER( ConstantBuffer );
		RHI_SHADER_BINDING_INITIALIZER( StructuredBuffer );
		RHI_SHADER_BINDING_INITIALIZER( StorageBuffer );
		RHI_SHADER_BINDING_INITIALIZER( Texture );
		RHI_SHADER_BINDING_INITIALIZER( StorageTexture );
		RHI_SHADER_BINDING_INITIALIZER( Sampler );
		RHI_SHADER_BINDING_INITIALIZER( CombinedSampler );

		constexpr RHIShaderBinding& AsInlinedConstants( const uint32_t a_Slot, const uint16_t a_Size )
		{
			Slot = a_Slot;
			Size = a_Size;
			PackedType = Cast<uint32_t>( ERHIShaderBindingType::InlinedConstants );
			return *this;
		}

		[[nodiscard]] static constexpr RHIShaderBinding InlinedConstants( const uint32_t a_Slot, const uint16_t a_Size )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.Size = a_Size;
			binding.PackedType = Cast<uint32_t>( ERHIShaderBindingType::InlinedConstants );
			return binding;
		}

		// This is used for creating an invalid binding.
		// As the default constructor does not initialize the memory.
		[[nodiscard]] static constexpr RHIShaderBinding Invalid()
		{
			RHIShaderBinding binding;
			binding.Slot = InvalidSlot;
			binding.Size = 0;
			binding.PackedType = Cast<uint32_t>( ERHIShaderBindingType::Unknown );
			return binding;
		}

	#undef RHI_SHADER_BINDING_INITIALIZER
	};
	//==============================================

	static_assert( sizeof( RHIShaderBinding ) == 8, "RHIShaderBinding size is not 8 bytes" );

	using RHIShaderBindingArray = InlineArray<RHIShaderBinding, RHIConstants::MaxShaderBindings>;

	//==============================================
	// RHI Shader Binding Layout Descriptor
	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIBindingLayoutDescriptor, RHIBindingLayout )
	{
		ERHIShaderVisibility Visibility = ERHIShaderVisibility::All; // Visibility of the binding. (e.g. Vertex, Pixel, Compute, etc.)
		RHIShaderBindingArray Bindings{};
		UnorderedMap<hash_t, Pair<uint32_t, String>> BindingMap{}; // Maps a hashed binding name to the index and string name of an item in the Bindings array.

		// Users should only use this function to add bindings, as the BindingMap needs to be updated.
		[[nodiscard]] RHIShaderBinding& AddBinding( HashedString a_Name, uint32_t a_InputIndex = 0 )
		{
			if ( a_InputIndex == 0 && Bindings.Size() > 0 )
			{
				a_InputIndex = Bindings.Size();
			}

			if ( Bindings.Size() <= a_InputIndex )
			{
				Bindings.Resize( a_InputIndex + 1 );
			}

			RHI_DEV_CHECK( BindingMap.find( a_Name ) == BindingMap.end(), "Binding with name hash already exists" );
			BindingMap[a_Name] = Pair{ a_InputIndex, a_Name.String() };

			Bindings[a_InputIndex].NameHash = a_Name;
			return Bindings[a_InputIndex];
		}

		// Users should only use this function to remove bindings, as the BindingMap needs to be updated.
		void RemoveBinding( HashedString a_Name )
		{
			if ( Bindings.IsEmpty() )
				return;

			auto it = BindingMap.find( a_Name );
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

		// Get the binding at the specified index.
		// Returns an invalid binding if the index is out of range.
		[[nodiscard]] RHIShaderBinding GetBindingFromName( hash_t a_Name ) const
		{
			int32_t index = GetBindingIndex( a_Name );
			RHI_DEV_CHECK( index != -1, "Binding with name hash does not exist" );
			return index >= 0 ? Bindings[index] : RHIShaderBinding::Invalid();
		}

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
	};
	//==============================================

	//==============================================
	// RHI Shader Binding Layout Interface
	DECLARE_RHI_RESOURCE_INTERFACE( RHIBindingLayout )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIBindingLayout, ERHIResourceType::BindingLayout );
		RHIBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
			: m_Desc( a_Desc ) {}
	};
	//==============================================



	/////////////////////////////////////////////////////////////////////////////////////////////////
	// RHI Shader Binding Sets
	//  A set of shader bindings that can be bound to a command list.
	/////////////////////////////////////////////////////////////////////////////////////////////////

	//==============================================
	// RHI Binding Set Item
	//  Represents a single shader binding in a set.
	struct RHIBindingSetItem
	{
		RHIResource* Resource;
		uint32_t Slot;
		ERHIShaderBindingType Type;
		uint8_t Unused[3];

		union
		{
			RHITextureSubresourceSet Subresources; // Valid only for textures.
			RHIBufferRange Range; // Valid only for buffers.
			uint64_t RawData[2];
		};

		bool operator==( const RHIBindingSetItem& a_Other ) const
		{
			return Resource == a_Other.Resource
				&& Slot == a_Other.Slot
				&& Type == a_Other.Type
				&& RawData[0] == a_Other.RawData[0]
				&& RawData[1] == a_Other.RawData[1];
		}

		bool operator!=( const RHIBindingSetItem& a_Other ) const
		{
			return !operator==( a_Other );
		}

		RHIBindingSetItem() {}

		static RHIBindingSetItem None( uint32_t a_Slot = RHIShaderBinding::InvalidSlot )
		{
			RHIBindingSetItem item;
			item.Resource = nullptr;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::Unknown;
			item.RawData[0] = 0;
			item.RawData[1] = 0;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem InlinedConstants( uint32_t a_Slot, uint32_t a_SizeInBytes )
		{
			RHIBindingSetItem item;
			item.Resource = nullptr;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::InlinedConstants;
			item.Range.Offset = 0;
			item.Range.Size = a_SizeInBytes;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem ConstantBuffer( uint32_t a_Slot, RHIBuffer& a_Buffer, RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::ConstantBuffer;
			item.Range = a_Range;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem StructuredBuffer( uint32_t a_Slot, RHIBuffer& a_Buffer, RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::StructuredBuffer;
			item.Range = a_Range;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem StorageBuffer( uint32_t a_Slot, RHIBuffer& a_Buffer, RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Buffer;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::StorageBuffer;
			item.Range = a_Range;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem Texture( uint32_t a_Slot, RHITexture& a_Texture, RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Texture;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::Texture;
			item.Subresources = a_Subresources;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem StorageTexture( uint32_t a_Slot, RHITexture& a_Texture, RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Texture;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::StorageTexture;
			item.Subresources = a_Subresources;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem Sampler( uint32_t a_Slot, RHISampler& a_Sampler )
		{
			RHIBindingSetItem item;
			item.Resource = &a_Sampler;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::Sampler;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}

		static RHIBindingSetItem CombinedSampler( uint32_t a_Slot, RHITexture& a_Texture, RHITextureSubresourceSet a_Subresources = RHITextureSubresourceSet::All() )
		{
			RHI_DEV_CHECK( a_Texture.Sampler != nullptr, "Texture does not have a sampler!" );
			RHIBindingSetItem item;
			item.Resource = &a_Texture;
			item.Slot = a_Slot;
			item.Type = ERHIShaderBindingType::CombinedSampler;
			item.Subresources = a_Subresources;
			item.Unused[0] = 0; item.Unused[1] = 0; item.Unused[2] = 0;
			return item;
		}
	};
	static_assert(sizeof( RHIBindingSetItem ) == 32, "RHIBindingSetItem size is not 32 bytes");
	using RHIBindingSetItemArray = InlineArray<RHIBindingSetItem, RHIConstants::MaxShaderBindings>;

	//==============================================
	// RHI Shader Binding Set Descriptor
	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIBindingSetDescriptor, RHIBindingSet )
	{
		RHIBindingSetItemArray Bindings{};

		RHIBindingSetDescriptor& AddBinding( const RHIBindingSetItem& a_Binding )
		{
			Bindings.PushBack( a_Binding );
			return *this;
		}

		bool operator==( const RHIBindingSetDescriptor& a_Other ) const
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

		bool operator!=( const RHIBindingSetDescriptor& a_Other ) const
		{
			return !(operator==( a_Other ));
		}
	};

	//==============================================
	// RHI Shader Binding Set Interface
	DECLARE_RHI_RESOURCE_INTERFACE( RHIBindingSet )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIBindingSet, ERHIResourceType::BindingSet );

		RHIBindingSet( const RHIBindingSetDescriptor& a_Desc, const RHIBindingLayoutRef& a_Layout )
			: m_Desc( a_Desc ), m_Layout( a_Layout )
		{}

	protected:
		RHIBindingLayoutRef m_Layout{};
	};
	

} // namespace Tridium