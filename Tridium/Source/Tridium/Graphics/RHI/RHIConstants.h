#pragma once
#include <Tridium/Core/Types.h>

namespace RHIConstants {

	static constexpr uint32_t MaxShaderInputs = 64u;    // Maximum number of shader inputs in DirectX 12 is 64 DWORDs.
	static constexpr uint32_t MaxTextureBindings = 32u; // Maximum number of textures that can be bound to a shader stage.
	static constexpr uint32_t MaxColorTargets = 4u;     // Maximum number of color targets that can be bound to the output merger stage.
	static constexpr uint32_t MaxVertexAttributes = 8u; // Maximum number of vertex attributes that can be bound to a vertex layout.
	static constexpr uint32_t MaxFrameBuffers = 2u;     // Maximum number of frame buffers that can be used in a swap chain.
}