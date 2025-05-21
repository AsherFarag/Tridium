#pragma once
#include "RHIResource.h"

namespace Tridium {

	//==========================================================
	// RHI Buffer
	//  A buffer represents a contiguous block of memory on the GPU.
	//==========================================================

	struct RHIBufferDescriptor
	{
		using ResourceType = class RHIBuffer;
		// The size of the buffer in bytes. Uniform buffers must be aligned to 16 bytes.
		size_t Size = 0;
		ERHIBindFlags BindFlags = ERHIBindFlags::None;
		ERHIUsage Usage = ERHIUsage::Default;
		ERHICpuAccess CpuAccess = ERHICpuAccess::None;
		ERHIBufferType Type = ERHIBufferType::Undefined;
		ERHIFormat Format = ERHIFormat::Unknown;
		// For structured buffers, the stride of each element in the buffer. In bytes.
		uint32_t Stride = 0;
		String Name{};

		constexpr RHIBufferDescriptor() = default;
		constexpr RHIBufferDescriptor(
			StringView a_Name,
			size_t a_Size,
			ERHIBindFlags a_BindFlags,
			ERHIUsage a_Usage = RHIBufferDescriptor{}.Usage,
			ERHICpuAccess a_CpuAccess = RHIBufferDescriptor{}.CpuAccess,
			ERHIBufferType a_Type = RHIBufferDescriptor{}.Type,
			uint32_t a_Stride = RHIBufferDescriptor{}.Stride )
			: Name( a_Name )
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
		constexpr RHIBufferDescriptor& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIBuffer )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIBuffer, ERHIResourceType::Buffer );

		RHIBuffer( const RHIBufferDescriptor& a_Desc )
			: m_Desc( a_Desc ) 
		{
			RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::RenderTarget )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil ),
				std::format( "Buffer '{}' cannot be created with bind flags '{}'", a_Desc.Name, ToString( a_Desc.BindFlags ) ) );

		}

		virtual ~RHIBuffer() = default;

		// Returns the internal state of the buffer.
		ERHIResourceStates State() const { return m_State; }

		// Sets the internal state of the buffer.
		// NOTE: This does not perform a state transition. This only sets the internal state of the buffer.
		//       Should only be used if manual state transitions have been completed and you want to return state management to the RHI.
		void SetState( ERHIResourceStates a_State ) { m_State = a_State; }

	private:
		ERHIResourceStates m_State = ERHIResourceStates::Unknown;
	};

} // namespace Tridium