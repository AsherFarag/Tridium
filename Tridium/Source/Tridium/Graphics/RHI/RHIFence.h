#pragma once
#include "RHIResource.h"

namespace Tridium {

	//=======================================================
	// RHI Fence
	//  A fence is a synchronization primitive that can be used to synchronize the CPU and GPU.
	//=======================================================

	struct RHIFenceDescriptor
	{
		using ResourceType = class RHIFence;
		ERHIFenceType Type = ERHIFenceType::CPUWaitOnly;
		String Name{};

		constexpr auto& SetType( ERHIFenceType a_Type ) { Type = a_Type; return *this; }
		constexpr auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIFence )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIFence, ERHIResourceType::Fence );
		RHIFence( const DescriptorType& a_Desc )
			: m_Desc( a_Desc )
		{}

		virtual uint64_t GetCompletedValue() = 0;
		virtual void Signal( uint64_t a_Value ) = 0;
		virtual void Wait( uint64_t a_Value ) = 0;
	};

} // namespace Tridium