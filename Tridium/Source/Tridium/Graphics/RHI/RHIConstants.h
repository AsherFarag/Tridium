#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium::RHIConstants {

	// Binding slot offsets for Vulkan and OpenGL to avoid overlapping slots.
	#define RHI_SRV_BINDING_SLOT_OFFSET     0
	#define RHI_UAV_BINDING_SLOT_OFFSET     0
	#define RHI_SAMPLER_BINDING_SLOT_OFFSET 0

	static constexpr uint32_t MaxViewports            = 16u; 
	static constexpr uint32_t MaxVertexAttributes     = 16u; 
	static constexpr uint32_t MaxBindingLayouts       = 5u;  
	static constexpr uint32_t MaxShaderBindings       = 64u; 
	static constexpr uint32_t MaxTextureBindings      = 32u; 
	static constexpr uint32_t MaxColorTargets         = 8u;  
	static constexpr uint32_t MaxFramesInFlight       = 2u;  
	static constexpr uint32_t MaxInlinedConstantsSize = 256u; // D3D12 min is 256 bytes, but Vulkan only guarantees 128 bytes.
}