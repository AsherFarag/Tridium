#pragma once
#include "RHIConfig.h"
#include "RHIDefinitions.h"
#include "RHIGlobals.h"
#include "DynamicRHI.h"
#include "RHIConstants.h"
#include <Tridium/Graphics/Color.h>
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Containers/Containers.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Utils/Todo.h>
#include <Tridium/Debug/SourceLocation.h>
#include <Shaders/ShaderInterop.h>

namespace Tridium {

	// Common RHI functions
	namespace RHI {

		// Get the name of the dynamically bound RHI.
		const char* GetRHIName();

		// Get the type of the dynamically bound RHI.
		ERHInterfaceType GetRHIType();

		// Get the global dynamically bound RHI.
		static IDynamicRHI* GetDynamicRHI() { return s_DynamicRHI; }

		// Get the name of the given RHI type.
		constexpr const char* GetRHIName( ERHInterfaceType a_API );
	}

	// RHI Query and Functions
	namespace RHIQuery {

		// Are we in debug mode?
		static bool IsDebug()
		{
		#if RHI_DEBUG_ENABLED
			return s_RHIGlobals.Config.UseDebug;
		#else
			return false;
		#endif
		}

		// Can samplers be used as separate objects?
		static bool SupportsSeparateSamplers()
		{
			if ( RHI::GetRHIType() == ERHInterfaceType::OpenGL )
			{
				return false;
			}

			return true;
		}

		static bool SupportsMultithreading()
		{
			return s_RHIGlobals.SupportsMultithreading;
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

		static ERHIShaderFormat GetShaderFormat()
		{
			switch ( RHI::GetRHIType() )
			{
				using enum ERHInterfaceType;
			case DirectX11:
			case DirectX12:
				return ERHIShaderFormat::HLSL6;

			case OpenGL:
			case Vulkan:
				return ERHIShaderFormat::SPIRV;

			default:
				return ERHIShaderFormat::Unknown;
			}
		}
	} // namespace RHIQuery

	//======================================================================
	// RHI Implementation
	//======================================================================

	constexpr const char* RHI::GetRHIName( ERHInterfaceType a_API )
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

} // namespace Tridium