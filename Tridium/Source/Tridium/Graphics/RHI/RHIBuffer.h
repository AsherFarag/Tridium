#pragma once
#include "RHIResource.h"

namespace Tridium {

	//==========================================================
	// RHI Buffer Type
	//  Describes how the buffer is accessed.
	//===========================================================
	enum class ERHIBufferType : uint8_t
	{
		Undefined = 0,
		// This buffer is accessed via raw bytes.
		// RHIBufferDescriptor::Stride must specify the size of the format.
		Raw,
		// Accessing this buffer uses format conversion.
		// RHIBufferDescriptor::Stride must match the size of the format.
		Formatted,
		// This buffer is accessed via a structure. RHIBufferDescriptor::Stride defines the size of the structure.
		Structured,
	};

	//==========================================================
	// RHI Buffer
	//  A buffer represents a contiguous block of memory on the GPU.
	//==========================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIBufferDescriptor, RHIBuffer )
	{
		// The size of the buffer in bytes. Uniform buffers must be aligned to 16 bytes.
		size_t Size = 0;
		// Bind flags.
		ERHIBindFlags BindFlags = ERHIBindFlags::None;
		// Buffer usage hint.
		ERHIUsage Usage = ERHIUsage::Default;
		// CPU access flags. None by default.
		ERHICpuAccess CpuAccess = ERHICpuAccess::None;
		// The type of the buffer. This is used to determine how the buffer is accessed.
		ERHIBufferType Type = ERHIBufferType::Undefined;
		// For structured buffers, the stride of each element in the buffer.
		// In bytes.
		uint32_t Stride = 0;
		ERHIFormat Format = ERHIFormat::Unknown;

		constexpr RHIBufferDescriptor() = default;
		constexpr RHIBufferDescriptor(
			StringView a_Name,
			size_t a_Size,
			ERHIBindFlags a_BindFlags,
			ERHIUsage a_Usage = RHIBufferDescriptor{}.Usage,
			ERHICpuAccess a_CpuAccess = RHIBufferDescriptor{}.CpuAccess,
			ERHIBufferType a_Type = RHIBufferDescriptor{}.Type,
			uint32_t a_Stride = RHIBufferDescriptor{}.Stride )
			: Super( a_Name )
			, Size( a_Size )
			, BindFlags( a_BindFlags )
			, Usage( a_Usage )
			, CpuAccess( a_CpuAccess )
			, Type( a_Type )
			, Stride( a_Stride )
		{
		}

		constexpr RHIBufferDescriptor& SetSize( size_t a_Size ) { Size = a_Size; return *this; }
		constexpr RHIBufferDescriptor& SetBindFlags( ERHIBindFlags a_BindFlags ) { BindFlags = a_BindFlags; return *this; }
		constexpr RHIBufferDescriptor& SetUsage( ERHIUsage a_Usage ) { Usage = a_Usage; return *this; }
		constexpr RHIBufferDescriptor& SetCpuAccess( ERHICpuAccess a_CpuAccess ) { CpuAccess = a_CpuAccess; return *this; }
		constexpr RHIBufferDescriptor& SetType( ERHIBufferType a_Type ) { Type = a_Type; return *this; }
		constexpr RHIBufferDescriptor& SetStride( uint32_t a_Stride ) { Stride = a_Stride; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIBuffer )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIBuffer, ERHIResourceType::Buffer );

		RHIBuffer( const RHIBufferDescriptor& a_Desc )
			: m_Desc( a_Desc ) {}
		virtual ~RHIBuffer() = default;
		
		virtual bool Commit( const RHIBufferDescriptor& a_Desc ) = 0;

		// Returns the internal state of the buffer.
		ERHIResourceStates GetState() const { return m_State; }

		// Sets the internal state of the buffer.
		// NOTE: This does not perform a state transition. This only sets the internal state of the buffer.
		//       Should only be used if manual state transitions have been completed and you want to return state management to the RHI.
		void SetState( ERHIResourceStates a_State ) { m_State = a_State; }

	private:
		ERHIResourceStates m_State = ERHIResourceStates::Unknown;
	};

} // namespace Tridium