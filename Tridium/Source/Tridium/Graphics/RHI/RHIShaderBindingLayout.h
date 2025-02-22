#pragma once
#include "RHIResource.h"

namespace Tridium {

	//==============================================
	// RHI Shader Binding Type
	//  Describes the type of shader binding.
	//  For example, in DirectX 12, a shader binding in a Root Signature can be a constant buffer, mutable buffer, texture, or sampler.
	enum class ERHIShaderBindingType : uint8_t
	{
		Unknown = 0,
		Constant,	// Constant buffer, uniform buffer, etc.
		Mutable,	// Mutable buffer, structured buffer, etc.
		Texture,	// Texture
		Sampler,	// Sampler
	};



	//==============================================
	// RHI Shader Binding
	//  Describes where a shader resource is bound to the shader.
	struct RHIShaderBinding
	{
		ERHIShaderVisibility Visibility = ERHIShaderVisibility::All;
		ERHIShaderBindingType BindingType = ERHIShaderBindingType::Unknown;
		RHITensorType InlinedConstantType = RHITensorType::From<int32_t>();
		bool IsInlined = false; // If the data is inlined in the shader.
		uint8_t WordSize = 1; // Number of 32-bit words for the binding. For example, a Vector4 would be 4 words ( 4 * 32 bit floats ).
		uint8_t Register = 0; // Register index in the shader.

		constexpr uint32_t GetSizeInBytes() const
		{
			return WordSize * 4;
		}


		constexpr RHIShaderBinding& SetVisibility( ERHIShaderVisibility a_Visibility )
		{
			Visibility = a_Visibility;
			return *this;
		}

		template<typename T>
		constexpr RHIShaderBinding& SetInlinedConstants( uint8_t a_Register )
		{
			BindingType = ERHIShaderBindingType::Constant;
			InlinedConstantType = RHITensorType::From<T>();
			IsInlined = true;
			WordSize = sizeof( T ) >> 2;
			Register = a_Register;
			return *this;
		}

		constexpr RHIShaderBinding& AsReferencedConstants( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Constant;
			IsInlined = false;
			WordSize = a_CountInAllocRange;
			Register = a_Register;
			return *this;
		}

		constexpr RHIShaderBinding& AsReferencedMutables( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Mutable;
			IsInlined = false;
			WordSize = a_CountInAllocRange;
			Register = a_Register;
			return *this;
		}

		constexpr RHIShaderBinding& AsReferencedTextures( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Texture;
			IsInlined = false;
			WordSize = a_CountInAllocRange;
			Register = a_Register;
			return *this;
		}

		constexpr RHIShaderBinding& AsReferencedSamplers( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Sampler;
			IsInlined = false;
			WordSize = a_CountInAllocRange;
			Register = a_Register;
			return *this;
		}

	};



	//==============================================
	// RHI Shader Binding Layout
	//  Represents the layout for bindable resources in a shader.
	//  An API equivalent is a Root Signature in DirectX 12.
	//  For Vulkan, this would be a Descriptor Set Layout.
	RHI_RESOURCE_BASE_TYPE( ShaderBindingLayout )
	{
		Array<RHIShaderBinding> Bindings;
		UnorderedMap<hash_t, uint32_t> BindingMap; // Maps a hash of the binding name to the index in the Bindings array.

		// Add a binding to the layout.
		RHIShaderBinding& AddBinding( hash_t a_NameHash, uint32_t a_InputIndex = 0 )
		{
			if ( Bindings.Size() <= a_InputIndex )
			{
				Bindings.Resize( a_InputIndex + 1 );
			}

			ASSERT_LOG( BindingMap.find( a_NameHash ) == BindingMap.end(), "Binding with name hash already exists" );
			BindingMap[a_NameHash] = a_InputIndex;
			return Bindings[a_InputIndex];
		}

		// Get the index of a binding in the layout. 
		// Returns -1 if the binding does not exist.
		int32_t GetBindingIndex( hash_t a_NameHash ) const
		{
			auto it = BindingMap.find( a_NameHash );
			if ( it != BindingMap.end() )
			{
				return static_cast<int32_t>( it->second );
			}
			return -1;
		}
	};



} // namespace Tridium