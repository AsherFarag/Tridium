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
		Constant,	  // Constant buffer, uniform buffer, etc.    (DX12 CBV) / (Vulkan Uniform Buffer)
		Mutable,	  // Mutable buffer, structured buffer, etc.  (DX12 SRV) / (Vulkan Storage Buffer)
		Storage,	  // Storage buffer, RWStructuredBuffer, etc. (DX12 UAV) / (Vulkan Storage Buffer)
		Texture,	  // Read only Texture resource.              (DX12 SRV) / (Vulkan Sampled Image)
		RWTexture,	  // Read/Write Texture resource.             (DX12 UAV) / (Vulkan Storage Image)
		Sampler,	  // Sampler state.                           (DX12 SAM) / (Vulkan Sampler)
	};

	//==============================================
	// RHI Shader Binding
	//  Describes where a shader resource is bound to the shader.
	struct RHIShaderBinding
	{
		// Represents a list of tensors and their name, that can be inlined in the shader.
		struct InlinedConstant
		{
			TODO( "Make a proper Max value" );
			static constexpr size_t s_MaxInlinedTensors = 32;
			HashedString TypeNameHash{};
			InlineArray<Pair<String, RHITensorType>, s_MaxInlinedTensors> Tensors{};
		};

		ERHIShaderVisibility Visibility = ERHIShaderVisibility::All;
		ERHIShaderBindingType BindingType = ERHIShaderBindingType::Unknown;
		Optional<InlinedConstant> InlinedConstant{};
		uint8_t WordSize = 1; // Number of 32-bit words for the binding. For example, a Vector4 would be 4 words ( 4 * 32 bit floats ).
		uint8_t BindSlot = 0; // Register index in the shader.
		HashedString Name{};  // Name of the binding.

		constexpr uint32_t GetSizeInBytes() const
		{
			return WordSize * 4;
		}

		constexpr RHIShaderBinding& SetVisibility( ERHIShaderVisibility a_Visibility )
		{
			Visibility = a_Visibility;
			return *this;
		}

		constexpr bool IsInlined() const
		{
			return InlinedConstant.has_value();
		}

		// Set the binding as inlined constants, such as a Vector4.
		// The data is inlined in the shader and is not referenced from a buffer, making it faster to access.
		// This can be used for small data types such as colors, matrices, etc.
		// In DirectX 12, this would be a Root Constant, and in Vulkan, this would be a Push Constant.
		template<typename T>
		constexpr RHIShaderBinding& AsInlinedConstants( uint8_t a_BindSlotStart )
		{
			BindingType = ERHIShaderBindingType::Constant;
			WordSize = sizeof( T ) >> 2;
			BindSlot = a_BindSlotStart;
			InlinedConstant.emplace().TypeNameHash = GetStrippedTypeName<T>();
			if constexpr ( IsRHITensorType<T> )
			{
				InlinedConstant->Tensors.EmplaceBack( Name, RHITensorType::From<T>() );
			}
			else if constexpr ( Concepts::Aggregate<T> )
			{
				ForEachField( T{}, [this]( StringView a_Name, auto& a_Field )
					{
						if constexpr ( IsRHITensorType<std::decay_t<decltype( a_Field )>> )
						{
							String name;
							name.reserve( name.size() + a_Name.size() + 1 );
							name.append( Name.String() );
							name.append( "." );
							name.append( a_Name );
							InlinedConstant->Tensors.EmplaceBack( std::move( name ), RHITensorType::From<std::decay_t<decltype( a_Field )>>());
						}
						else if constexpr ( Concepts::Aggregate<std::decay_t<decltype( a_Field )>> )
						{
							ForEachField( a_Field, [this, a_Name]( StringView a_SubName, auto& a_SubField )
								{
									if constexpr ( IsRHITensorType<std::decay_t<decltype( a_SubField )>> )
									{
										String name;
										name.reserve( a_Name.size() + a_SubName.size() + 1 );
										name.append( a_Name );
										name.append( "." );
										name.append( a_SubName );
										InlinedConstant.emplace().Tensors.EmplaceBack( std::move( name ), RHITensorType::From<std::decay_t<decltype( a_SubField )>>());
									}
								} );
						}
					} );
			}
			else
			{
				static_assert( false, "Unsupported type for InlinedConstant" );
			}

			return *this;
		}

		// Set the binding as referenced constants, such as a constant buffer.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for larger data types such as transformation matrices, etc.
		// In DirectX 12, this would be a Constant Buffer, and in Vulkan/OpenGL, this would be a Uniform Buffer.
		constexpr RHIShaderBinding& AsReferencedConstants( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Constant;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		// Set the binding as referenced mutables, such as a structured buffer.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for mutable data types such as instance data, etc.
		// In DirectX 12, this would be a Shader Resource View, and in Vulkan, this would be a Storage Buffer.
		constexpr RHIShaderBinding& AsReferencedMutables( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Mutable;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		// Set the binding as referenced storages, such as a RWStructuredBuffer.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for read/write data types such as compute shaders, etc.
		// In DirectX 12, this would be an Unordered Access View, and in Vulkan, this would be a Storage Buffer.
		constexpr RHIShaderBinding& AsReferencedStorages( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Storage;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		// Set the binding as referenced textures, such as a Texture2D.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for texture data types such as textures, etc.
		// In DirectX 12, this would be a Shader Resource View, and in Vulkan, this would be a Sampled Image.
		constexpr RHIShaderBinding& AsReferencedTextures( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Texture;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		// Set the binding as referenced RWTextures, such as a RWTexture2D.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for texture data types such as textures, etc.
		// In DirectX 12, this would be an Unordered Access View, and in Vulkan, this would be a Storage Image.
		constexpr RHIShaderBinding& AsReferencedRWTextures( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::RWTexture;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		// Set the binding as referenced samplers, such as a SamplerState.
		// The data is referenced from a buffer and is not inlined in the shader.
		// This can be used for texture data types such as textures, etc.
		// In DirectX 12, this would be a Sampler, and in Vulkan, this would be a Sampler.
		constexpr RHIShaderBinding& AsReferencedSamplers( uint8_t a_BindSlotStart, uint8_t a_CountInAllocRange = 1 )
		{
			BindingType = ERHIShaderBindingType::Sampler;
			WordSize = a_CountInAllocRange;
			BindSlot = a_BindSlotStart;
			return *this;
		}

		constexpr bool IsInlinedConstants() const { return BindingType == ERHIShaderBindingType::Constant && IsInlined(); }
		constexpr bool IsReferencedConstants() const { return BindingType == ERHIShaderBindingType::Constant && !IsInlined(); }
		constexpr bool IsReferencedMutables() const { return BindingType == ERHIShaderBindingType::Mutable; }
		constexpr bool IsReferencedStorages() const { return BindingType == ERHIShaderBindingType::Storage; }
		constexpr bool IsReferencedTextures() const { return BindingType == ERHIShaderBindingType::Texture; }
		constexpr bool IsReferencedRWTextures() const { return BindingType == ERHIShaderBindingType::RWTexture; }
		constexpr bool IsReferencedSamplers() const { return BindingType == ERHIShaderBindingType::Sampler; }
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

			ASSERT_LOG( BindingMap.find( a_Name ) == BindingMap.end(), "Binding with name hash already exists" );
			BindingMap[a_Name] = a_InputIndex;
			RHIShaderBinding& binding = Bindings[a_InputIndex];
			binding.Name = a_Name;
			return binding;
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
			ASSERT_LOG( index != -1, "Binding with name hash does not exist" );
			return Bindings[index];
		}
	};



} // namespace Tridium