#pragma once
#include "RHIResource.h"

namespace Tridium {

	enum class ERHIResourceAllocatorType : uint8_t
	{
		Unknown = 0,
		RenderResource, // Resources such as textures, buffers, etc.
		Sampler,
		RenderTarget,
		DepthStencil,
	};

	enum class ERHIResourceAllocatorFlags : uint32_t
	{
		None = 0,
		ShaderVisible = 1 << 0,
	};

	//=======================================================
	// RHI Resource Allocator
	//  Resource allocator represents a collection of contiguous allocations of descriptors.
	//  Known as a descriptor heap in DirectX 12 or a descriptor pool in Vulkan.
	DEFINE_RHI_RESOURCE( ResourceAllocator )
	{
		ERHIResourceAllocatorType AllocatorType = ERHIResourceAllocatorType::Unknown;
		ERHIResourceAllocatorFlags Flags = ERHIResourceAllocatorFlags::None;
		uint32_t Capacity = 8u;
	};

} // namespace Tridium