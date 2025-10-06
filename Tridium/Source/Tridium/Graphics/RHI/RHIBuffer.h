#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=================================================================================================
	// RHI Buffer Descriptor: Describes the properties and usage of a buffer resource in the RHI.
	//=================================================================================================
	struct RHIBufferDesc
	{
		using ResourceType = class IRHIBuffer;

		//=============================================================================================
		// The size of the buffer in bytes.
		size_t Size = 0;

		//=============================================================================================
		// Flags that specify how the buffer will be bound and used.
		ERHIBindFlags BindFlags = ERHIBindFlags::None;

		//=============================================================================================
		// The type of memory heap to allocate the texture from.
		ERHIHeapType HeapType = ERHIHeapType::Default;

		//=============================================================================================
		// The type of buffer, which determines how it is accessed in shaders.
		ERHIBufferType Type = ERHIBufferType::Unknown;

		//=============================================================================================
		// The format of the buffer elements, if applicable.
		ERHIFormat Format = ERHIFormat::Unknown;

		//=============================================================================================
		// For structured buffers, the stride of each element in the buffer. In bytes.
		uint32_t Stride = 0;

		//=============================================================================================
		// The initial resource state of the buffer when created.
		ERHIResourceStates InitialState = ERHIResourceStates::Common;

		//=============================================================================================
		// A human-readable name for the buffer, useful for debugging and profiling.
		String Name{};

		constexpr bool IsConstantBuffer() const { return EnumFlags( BindFlags ).HasFlag( ERHIBindFlags::ConstantBuffer ); }

		constexpr RHIBufferDesc() = default;
		constexpr RHIBufferDesc(
			StringView a_Name,
			size_t a_Size,
			ERHIBindFlags a_BindFlags,
			ERHIHeapType a_Usage = RHIBufferDesc{}.HeapType,
			ERHIBufferType a_Type = RHIBufferDesc{}.Type,
			uint32_t a_Stride = RHIBufferDesc{}.Stride,
			ERHIResourceStates a_InitialState = RHIBufferDesc{}.InitialState )
			: Name( a_Name )
			, Size( a_Size )
			, BindFlags( a_BindFlags )
			, HeapType( a_Usage )
			, Type( a_Type )
			, Stride( a_Stride )
			, InitialState( a_InitialState )
		{}

		constexpr auto& SetSize( size_t a_Size ) { Size = a_Size; return *this; }
		constexpr auto& SetBindFlags( ERHIBindFlags a_BindFlags ) { BindFlags = a_BindFlags; return *this; }
		constexpr auto& SetHeapType( ERHIHeapType a_Usage ) { HeapType = a_Usage; return *this; }
		constexpr auto& SetType( ERHIBufferType a_Type ) { Type = a_Type; return *this; }
		constexpr auto& SetFormat( ERHIFormat a_Format ) { Format = a_Format; return *this; }
		constexpr auto& SetStride( uint32_t a_Stride ) { Stride = a_Stride; return *this; }
		constexpr auto& SetInitialState( ERHIResourceStates a_State ) { InitialState = a_State; return *this; }
		constexpr auto& SetName( String a_Name ) { Name = std::move( a_Name ); return *this; }
	};

	//=================================================================================================
	// RHI Buffer Interface:
	// Represents a buffer resource in the RHI which is an allocation of memory that can store arbitrary data.
	// RHIBufferDesc::CpuAccess defines if the memory is allocated on GPU-only memory or is mappable to CPU memory.
	//=================================================================================================
	class IRHIBuffer : public IRHIResource
	{
		//=============================================================================================
		RHI_OBJECT_INTERFACE_BODY( Buffer );

		//=============================================================================================
		IRHIBuffer( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
			: IRHIResource( a_Device ), m_Desc( a_Desc )
		{
			RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::RenderTarget ) &&
						   !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil ),
						   "Buffer '{}' cannot be created with bind flags '{}'", a_Desc.Name, ToString( a_Desc.BindFlags ) );

			RHI_DEV_CHECK( a_Desc.Size > 0,
						   "Buffer '{}' cannot be created with size 0", a_Desc.Name );

			RHI_DEV_CHECK( m_Desc.HeapType != ERHIHeapType::Staging || m_Desc.BindFlags == ERHIBindFlags::None,
						   "Buffer '{}' cannot be created with staging heap and have bind flags", a_Desc.Name );

			RHI_DEV_CHECK( m_Desc.Type != ERHIBufferType::Formatted || m_Desc.Format != ERHIFormat::Unknown,
						   "Formatted buffer '{}' must have a valid format", a_Desc.Name );

			RHI_DEV_CHECK( m_Desc.Type != ERHIBufferType::Structured || m_Desc.Stride > 0,
						   "Structured buffer '{}' must have a valid stride", a_Desc.Name );

			RHI_DEV_CHECK( !m_Desc.IsConstantBuffer() || m_Desc.HeapType != ERHIHeapType::Staging,
						   "Constant buffer '{}' cannot be created with staging heap", a_Desc.Name );
		}

		//=============================================================================================
		virtual ~IRHIBuffer() = default;

		//=============================================================================================
		// Returns a CPU accessible pointer to the buffer memory.
		// The buffer must have been created with ERHIHeapType::Staging.
		virtual const void* Map() = 0;

		//=============================================================================================
		// Unmaps the buffer from CPU memory.
		virtual void Unmap() = 0;

	};

} // namespace Tridium