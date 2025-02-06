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

	// Common RHI functions
	namespace RHI {

		// Get the name of the dynamically bound RHI.
		const char* GetRHIName();

		// Get the type of the dynamically bound RHI.
		ERHInterfaceType GetRHIType();

		// Get the global dynamically bound RHI.
		template<typename T = DynamicRHI>
		T* GetDynamicRHI() requires Concepts::IsDynamicRHI<T>
		{
			return static_cast<T*>( s_DynamicRHI );
		}

		// Get the name of the given RHI type.
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
	}

	// RHI Query Constants and Functions
	namespace RHIQuery {
		constexpr uint32_t MaxTextureBindings = 32u;
		constexpr uint32_t MaxColourTargets = 4u;
		constexpr uint32_t MaxVertexAttributes = 8u;
		constexpr uint32_t MaxShaderInputs = 32u;

		static bool IsDebug()
		{
		#if RHI_DEBUG_ENABLED
			return s_RHIGlobals.Config.UseDebug;
		#else
			return false;
		#endif
		}

		static ERHITextureAlignment GetTextureAlignment()
		{
			switch ( RHI::GetRHIType() )
			{
				using enum ERHInterfaceType;

				case DirectX11:
				case DirectX12:
				case Metal:
					return ERHITextureAlignment::TopLeft;

				case OpenGL:
				case Vulkan:
					return ERHITextureAlignment::BottomLeft;

				default:
					return ERHITextureAlignment::TopLeft;
			}

		}
	}

} // namespace Tridium