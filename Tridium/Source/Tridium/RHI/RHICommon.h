#pragma once
#include <Tridium/Utils/Todo.h>
#include "RHIConfig.h"
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include "DynamicRHI.h"
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Containers/Containers.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	namespace RHI {

		constexpr const char* GetRHIName( ERHInterfaceType a_API )
		{
			switch ( a_API )
			{
				using enum ERHInterfaceType;
				case OpenGL:    return "OpenGL";
				case DirectX11: return "DirectX 11";
				case DirectX12: return "DirectX 12";
				case Vulkan:    return "Vulkan";
				case Metal:     return "Metal";
				default:        return "Null";
			}
		}


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