#pragma once
#include "RHIResource.h"
#include "RHISampler.h"

namespace Tridium {

	//==============================================
	// RHI Shader Binding Type
	//  Describes the type of shader binding.
	//  For example, in DirectX 12, a shader binding in a Root Signature can be a constant buffer, mutable buffer, texture, or sampler.
	enum class ERHIShaderBindingType : uint8_t
	{
		Unknown = 0,
		Constant,	  // Constant buffer, uniform buffer, etc.    (DX12 CBV) / (Vulkan Uniform Buffer)
		Mutable,	  // Mutable buffer, structured buffer, etc.  (DX12 SRV) / (Vulkan Storage Buffer)
		Storage,	  // Storage buffer, RWStructuredBuffer, etc. (DX12 UAV) / (Vulkan Storage Buffer)
		Texture,	  // Read only Texture resource.              (DX12 SRV) / (Vulkan Sampled Image)
		RWTexture,	  // Read/Write Texture resource.             (DX12 UAV) / (Vulkan Storage Image)
		Sampler,	  // Sampler state.                           (DX12 SAM) / (Vulkan Sampler)
		StaticSampler // Static sampler state.                  (DX12 SAM) / (Vulkan Sampler)
	};



	struct RHIStaticSampler
	{
		enum class EBorderColor : uint8_t
		{
			TransparentBlack = 0,
			OpaqueBlack,
			OpaqueWhite
		};

		ERHISamplerFilter Filter = ERHISamplerFilter::Bilinear;
		ERHISamplerAddressMode AddressU = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressV = ERHISamplerAddressMode::Repeat;
		ERHISamplerAddressMode AddressW = ERHISamplerAddressMode::Repeat;
		float MipLODBias = 0.0f;
		uint32_t MaxAnisotropy = 16;
		ERHISamplerComparison ComparisonFunc = ERHISamplerComparison::Never;
		EBorderColor BorderColor = EBorderColor::TransparentBlack;
		float MinLOD = 0.0f;
		float MaxLOD = FLT_MAX;
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
		uint8_t WordSize = 1;   // Number of 32-bit words for the binding. For example, a Vector4 would be 4 words ( 4 * 32 bit floats ).
		uint8_t Register = 0;   // Register index in the shader.
		RHIStaticSampler SamplerDesc{}; // Sampler descriptor for static samplers.

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
		constexpr RHIShaderBinding& AsInlinedConstants( uint8_t a_Register )
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

		constexpr RHIShaderBinding& AsReferencedStorages( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Storage;
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

		constexpr RHIShaderBinding& AsReferencedRWTextures( uint8_t a_Register, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::RWTexture;
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

		constexpr RHIShaderBinding& AsStaticSampler( uint8_t a_Register, const RHIStaticSampler& a_SamplerDesc )
		{
			BindingType = ERHIShaderBindingType::StaticSampler;
			SamplerDesc = a_SamplerDesc;
			Register = a_Register;
			return *this;
		}

		constexpr bool IsInlinedConstants() const { return IsInlined && BindingType == ERHIShaderBindingType::Constant; }
		constexpr bool IsReferencedConstants() const { return !IsInlined && BindingType == ERHIShaderBindingType::Constant; }
		constexpr bool IsReferencedMutables() const { return !IsInlined && BindingType == ERHIShaderBindingType::Mutable; }
		constexpr bool IsReferencedStorages() const { return !IsInlined && BindingType == ERHIShaderBindingType::Storage; }
		constexpr bool IsReferencedTextures() const { return !IsInlined && BindingType == ERHIShaderBindingType::Texture; }
		constexpr bool IsReferencedRWTextures() const { return !IsInlined && BindingType == ERHIShaderBindingType::RWTexture; }
		constexpr bool IsReferencedSamplers() const { return !IsInlined && BindingType == ERHIShaderBindingType::Sampler; }
		constexpr bool IsStaticSampler() const { return BindingType == ERHIShaderBindingType::StaticSampler; }

	};



	//==============================================
	// RHI Shader Binding Layout
	//  Represents the layout for bindable resources in a shader.
	//  An API equivalent is a Root Signature in DirectX 12.
	//  For Vulkan, this would be a Descriptor Set Layout.
	DEFINE_RHI_RESOURCE( ShaderBindingLayout )
	{
		Array<RHIShaderBinding> Bindings;
		UnorderedMap<hash_t, uint32_t> BindingMap; // Maps a hash of the binding name to the index in the Bindings array.

		// Add a binding to the layout.
		RHIShaderBinding& AddBinding( hash_t a_Name, uint32_t a_InputIndex = 0 )
		{
			if ( a_InputIndex == 0 && Bindings.Size() > 0 )
			{
				a_InputIndex = Bindings.Size();
			}

			if ( Bindings.Size() <= a_InputIndex )
			{
				Bindings.Resize( a_InputIndex + 1 );
			}

			ASSERT_LOG( BindingMap.find( a_Name ) == BindingMap.end(), "Binding with name hash already exists" );
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
	};



} // namespace Tridium