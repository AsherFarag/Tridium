#pragma once
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>

namespace Tridium {

	// Core RHI functions
	namespace RHI {
		bool Initialise( const RHIConfig& a_Config );
		bool Shutdown();
		bool Present();
		const RHIConfig& GetConfig();
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