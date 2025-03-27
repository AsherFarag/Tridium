#pragma once
#include <Tridium/Core/Config.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
	#define RHI_DEBUG_OP( x ) x
#else
	#define RHI_DEBUG_ENABLED 0
	#define RHI_DEBUG_OP( x )
#endif // CONFIG_DEBUG

//=====================================================================
// Backends
#define RHI_ENABLE_BACKEND_DIRECTX11 0
#define RHI_ENABLE_BACKEND_DIRECTX12 1
#define RHI_ENABLE_BACKEND_OPENGL 1
#define RHI_ENABLE_BACKEND_VULKAN 0
#define RHI_ENABLE_BACKEND_METAL 0
//=====================================================================

//=====================================================================
// Shader Compiler
#define RHI_ENABLE_SHADER_COMPILER 1
#define RHI_ENABLE_SHADER_COMPILER_DX 1
#define RHI_ENABLE_SHADER_COMPILER_PSSL 0
//=====================================================================


// RHI SUPPORT OPENGL
// OpenGL has specific requirements that must be met in order to be supported.
// These requirements can have a performance impact on the RHI even if OpenGL is not used.
// If you do not plan to use OpenGL, you can disable it here.
#define RHI_SUPPORT_OPENGL RHI_ENABLE_BACKEND_OPENGL

// RHI SUPPORT COMBINED SAMPLERS
// OpenGL requires that samplers and textures are combined into a single binding.
// This can cause systems to miss out on potential optimizations if support is required.
#define RHI_SUPPORT_COMBINED_SAMPLERS RHI_SUPPORT_OPENGL