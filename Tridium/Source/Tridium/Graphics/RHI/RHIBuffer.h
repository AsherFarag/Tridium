#pragma once
#include "RHIResource.h"

namespace Tridium {

	//==========================================================
	// RHI Buffer
	//  A buffer represents a contiguous block of memory on the GPU.
	//==========================================================

	struct RHIBufferDesc
	{
		using ResourceType = class IRHIBuffer;
		// The size of the buffer in bytes.
		size_t Size = 0;
		ERHIBindFlags BindFlags = ERHIBindFlags::None;
		ERHIUsage Usage = ERHIUsage::Default;
		ERHICpuAccess CpuAccess = ERHICpuAccess::None;
		ERHIBufferType Type = ERHIBufferType::Unknown;
		ERHIFormat Format = ERHIFormat::Unknown;
		// For structured buffers, the stride of each element in the buffer. In bytes.
		uint32_t Stride = 0;
		ERHIResourceStates InitialState = ERHIResourceStates::Common;
		String Name{};

		constexpr bool IsConstantBuffer() const { return EnumFlags( BindFlags ).HasFlag( ERHIBindFlags::ConstantBuffer ); }

		constexpr RHIBufferDesc() = default;
		constexpr RHIBufferDesc(
			StringView a_Name,
			size_t a_Size,
			ERHIBindFlags a_BindFlags,
			ERHIUsage a_Usage = RHIBufferDesc{}.Usage,
			ERHICpuAccess a_CpuAccess = RHIBufferDesc{}.CpuAccess,
			ERHIBufferType a_Type = RHIBufferDesc{}.Type,
			uint32_t a_Stride = RHIBufferDesc{}.Stride,
			ERHIResourceStates a_InitialState = RHIBufferDesc{}.InitialState )
			: Name( a_Name )
			, Size( a_Size )
			, BindFlags( a_BindFlags )
			, Usage( a_Usage )
			, CpuAccess( a_CpuAccess )
			, Type( a_Type )
			, Stride( a_Stride )
			, InitialState( a_InitialState )
		{}

		constexpr auto& SetSize( size_t a_Size ) { Size = a_Size; return *this; }
		constexpr auto& SetBindFlags( ERHIBindFlags a_BindFlags ) { BindFlags = a_BindFlags; return *this; }
		constexpr auto& SetUsage( ERHIUsage a_Usage ) { Usage = a_Usage; return *this; }
		constexpr auto& SetCpuAccess( ERHICpuAccess a_CpuAccess ) { CpuAccess = a_CpuAccess; return *this; }
		constexpr auto& SetType( ERHIBufferType a_Type ) { Type = a_Type; return *this; }
		constexpr auto& SetFormat( ERHIFormat a_Format ) { Format = a_Format; return *this; }
		constexpr auto& SetStride( uint32_t a_Stride ) { Stride = a_Stride; return *this; }
		constexpr auto& SetInitialState( ERHIResourceStates a_State ) { InitialState = a_State; return *this; }
		constexpr auto& SetName( String a_Name ) { Name = std::move( a_Name ); return *this; }
	};

	class IRHIBuffer : public IRHIResource
	{
		RHI_OBJECT_INTERFACE_BODY( Buffer );
		IRHIBuffer( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
			: IRHIResource( a_Device ), m_Desc( a_Desc )
		{
			RHI_DEV_CHECK( !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::RenderTarget )
						&& !EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil ),
				std::format( "Buffer '{}' cannot be created with bind flags '{}'", a_Desc.Name, ToString( a_Desc.BindFlags ) ) );

		}

		virtual ~IRHIBuffer() = default;
	};

} // namespace Tridium