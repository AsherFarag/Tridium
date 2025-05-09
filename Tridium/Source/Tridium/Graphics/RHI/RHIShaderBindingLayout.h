#pragma once
#include "RHIResource.h"
#include "RHISampler.h"
#include <Tridium/Reflection/FieldReflection.h>

namespace Tridium {

	//==============================================
	// RHI Shader Binding Type
	//  Describes the type of shader binding.
	//  For example, in DirectX 12, a shader binding in a Root Signature can be a constant buffer, mutable buffer, texture, or sampler.
	enum class ERHIShaderBindingType : uint8_t
	{
		Unknown = 0,
		InlinedConstants, // Inlined constants.                             (DX12 Root Constants) / (Vulkan Push Constants)
		ConstantBuffer,	  // Constant buffer, uniform buffer, etc.          (DX12 CBV) / (Vulkan Uniform Buffer)
		MutableBuffer,	  // MutableBuffer buffer, structured buffer, etc.  (DX12 SRV) / (Vulkan Storage Buffer)
		Storage,	      // Storage buffer, RWStructuredBuffer, etc.       (DX12 UAV) / (Vulkan Storage Buffer)
		Texture,	      // Read only Texture resource.                    (DX12 SRV) / (Vulkan Sampled Image)
		RWTexture,	      // Read/Write Texture resource.                   (DX12 UAV) / (Vulkan Storage Image)
		Sampler,	      // Sampler state.                                 (DX12 SAM) / (Vulkan Sampler)
		CombinedSampler,  // Combined Texture Sampler.                      (DX12 SRV + SAM) / (Vulkan Combined Image Sampler)
	};

	//==============================================
	// RHI Shader Binding
	//  Represents a singular resource binding in a shader.
	struct RHIShaderBinding
	{
		static constexpr uint32_t InvalidSlot = ~0u;

		uint32_t Slot = InvalidSlot; // Register index in the shader.
		uint16_t Size = 0;           // Size in bytes of the binding.
		ERHIShaderBindingType Type = ERHIShaderBindingType::Unknown;

	#define RHI_SHADER_BINDING_INITIALIZER( _Type ) \
		constexpr RHIShaderBinding& As##_Type( const uint32_t a_Slot ) \
		{ \
			Slot = a_Slot; \
			Size = 0; \
			Type = ERHIShaderBindingType::_Type; \
			return *this; \
		} \
		static constexpr RHIShaderBinding _Type( const uint32_t a_Slot ) \
		{ \
			return RHIShaderBinding{}.As##_Type( a_Slot ); \
		}

		RHI_SHADER_BINDING_INITIALIZER( ConstantBuffer );
		RHI_SHADER_BINDING_INITIALIZER( MutableBuffer );
		RHI_SHADER_BINDING_INITIALIZER( Storage );
		RHI_SHADER_BINDING_INITIALIZER( Texture );
		RHI_SHADER_BINDING_INITIALIZER( RWTexture );
		RHI_SHADER_BINDING_INITIALIZER( Sampler );
		RHI_SHADER_BINDING_INITIALIZER( CombinedSampler );

		constexpr RHIShaderBinding& AsInlinedConstants( const uint32_t a_Slot, const uint16_t a_Size )
		{
			Slot = a_Slot;
			Size = a_Size;
			Type = ERHIShaderBindingType::CombinedSampler;
			return *this;
		}

		static constexpr RHIShaderBinding InlinedConstants( const uint32_t a_Slot, const uint16_t a_Size )
		{
			RHIShaderBinding binding;
			binding.Slot = a_Slot;
			binding.Size = a_Size;
			binding.Type = ERHIShaderBindingType::CombinedSampler;
			return binding;
		}

	#undef RHI_SHADER_BINDING_INITIALIZER
	};
	static_assert( sizeof( RHIShaderBinding ) == 8, "RHIShaderBinding size is not 8 bytes" );

	using RHIShaderBindingArray = InlineArray<RHIShaderBinding, RHIConstants::MaxShaderBindings>;

	//==============================================
	// RHI Shader Binding Layout
	//  Represents the layout for bindable resources in a shader.
	//  An API equivalent is a Root Signature in DirectX 12.
	//  For Vulkan, this would be a Descriptor Set Layout.
	//=======================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIShaderBindingLayoutDescriptor, RHIShaderBindingLayout )
	{
		ERHIShaderVisibility Visibility = ERHIShaderVisibility::All; // Visibility of the binding. (e.g. Vertex, Pixel, Compute, etc.)
		RHIShaderBindingArray Bindings{};
		UnorderedMap<hash_t, uint32_t> BindingMap{}; // Maps a hash of the binding name to the index in the Bindings array.

		// Add a binding to the layout.
		RHIShaderBinding& AddBinding( HashedString a_Name, uint32_t a_InputIndex = 0 )
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
			BindingMap[a_Name] = a_InputIndex;
			return Bindings[a_InputIndex];
		}

		// Get the index of a binding in the layout. 
		// Returns -1 if the binding does not exist.
		int32_t GetBindingIndex( hash_t a_Name ) const
		{
			auto it = BindingMap.find( a_Name );
			if ( it != BindingMap.end() )
			{
				return static_cast<int32_t>( it->second );
			}
			return -1;
		}

		// Get the binding at the specified index.
		const RHIShaderBinding& GetBindingFromName( hash_t a_Name ) const
		{
			int32_t index = GetBindingIndex( a_Name );
			RHI_DEV_CHECK( index != -1, "Binding with name hash does not exist" );
			return Bindings.At( index );
		}
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIShaderBindingLayout )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIShaderBindingLayout, ERHIResourceType::ShaderBindingLayout );
		virtual bool Commit( const RHIShaderBindingLayoutDescriptor& a_Desc ) = 0;
	};

} // namespace Tridium