#pragma once
#include "RHIResource.h"

namespace Tridium {

	enum class ERHIResourceAllocatorFlags : uint32_t
	{
		None = 0,
		ShaderVisible = 1 << 0,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHIResourceAllocatorFlags );

	//=======================================================
	// RHI Resource Allocator
	//  Resource allocator represents a collection of contiguous allocations of descriptors.
	//  Known as a descriptor heap in DirectX 12 or a descriptor pool in Vulkan.
	DEFINE_RHI_RESOURCE( ResourceAllocator,
		virtual bool Allocate( uint32_t a_Count, uint32_t* o_Offset = nullptr ) = 0;
		virtual bool Deallocate( RHIAllocatableResource& a_Resource ) = 0;
		virtual uint32_t GetSize() const = 0; // The total number of descriptors in the allocator
	)
	{
		ERHIResourceAllocatorType AllocatorType = ERHIResourceAllocatorType::Unknown;
		EnumFlags<ERHIResourceAllocatorFlags> Flags = ERHIResourceAllocatorFlags::None;
		uint32_t Capacity = 8u; // The maximum number of descriptors that can be allocated
	};

} // namespace Tridium