#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium::RHIConstants {
	static constexpr uint32_t MaxViewports            = 16u; 
	static constexpr uint32_t MaxVertexAttributes     = 16u; 
	static constexpr uint32_t MaxBindingLayouts       = 5u;  
	static constexpr uint32_t MaxShaderBindings       = 128u;
	static constexpr uint32_t MaxShaderInputs         = 64u; 
	static constexpr uint32_t MaxTextureBindings      = 32u; 
	static constexpr uint32_t MaxColorTargets         = 8u;  
	static constexpr uint32_t MaxFrameBuffers         = 2u;  
	static constexpr uint32_t MaxInlinedConstantsSize = 128u; // D3D12 allows 256 bytes, but Vulkan only guarantees 128 bytes.
}