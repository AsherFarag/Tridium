#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
#else
	#define RHI_DEBUG_ENABLED 0
#endif // CONFIG_DEBUG

namespace Tridium {

	namespace RHI {
		ERHInterfaceType GetRHInterfaceType();
		const char* GetRHIName( ERHInterfaceType a_API = ERHInterfaceType::Null );
	}

	// RHI Query Constants
	namespace RHIQuery {
		constexpr uint32_t MaxTextureBindings = 32u;
		constexpr uint32_t MaxColourTargets = 4u;
		constexpr uint32_t MaxVertexAttributes = 8u;
		constexpr uint32_t MaxShaderInputs = 32u;
	}

} // namespace Tridium