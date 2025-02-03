#pragma once
#include "RHIConfig.h"
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include "DynamicRHI.h"
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>
#include <type_traits>

namespace Tridium {

	namespace RHI {
		ERHInterfaceType GetRHInterfaceType();
		const char* GetRHIName( ERHInterfaceType a_API = ERHInterfaceType::Null );

		template<typename T = DynamicRHI>
		T* GetDynamicRHI() requires Concepts::IsDynamicRHI<T>
		{
			return static_cast<T*>( s_DynamicRHI );
		}
	}

	// RHI Query Constants
	namespace RHIQuery {
		constexpr uint32_t MaxTextureBindings = 32u;
		constexpr uint32_t MaxColourTargets = 4u;
		constexpr uint32_t MaxVertexAttributes = 8u;
		constexpr uint32_t MaxShaderInputs = 32u;
	}

} // namespace Tridium