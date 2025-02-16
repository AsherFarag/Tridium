#pragma once
#include <Tridium/Core/Config.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
#else
	#define RHI_DEBUG_ENABLED 0
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