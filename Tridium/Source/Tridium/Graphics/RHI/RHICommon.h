#pragma once
#include "RHIConfig.h"
#include "RHIDefinitions.h"
#include "RHIToString.h"
#include "RHIGlobals.h"
#include "DynamicRHI.h"
#include "RHIConstants.h"
#include <Tridium/Graphics/Color.h>
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Core/Cast.h>
#include <Tridium/Containers/Containers.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Utils/Todo.h>
#include <Tridium/Debug/SourceLocation.h>
#include <Shaders/ShaderInterop.h>

namespace Tridium {

	// Common RHI functions
	namespace RHI {

		// Get the name of the dynamically bound RHI.
		StringView GetRHIName();

		// Get the type of the dynamically bound RHI.
		ERHInterfaceType GetRHIType();

		// Get the global dynamically bound RHI.
		static IDynamicRHI* GetDynamicRHI() { return s_DynamicRHI; }

		// Get the name of the given RHI type.
		constexpr StringView GetRHIName( ERHInterfaceType a_API );

		// Are we in debug mode?
		inline bool IsDebug()
		{
		#if RHI_DEBUG_ENABLED
			return s_RHIGlobals.Config.UseDebug;
		#else
			return false;
		#endif
		}

		// Can samplers be used as separate objects?
		inline bool SupportsSeparateSamplers()
		{
			if ( GetRHIType() == ERHInterfaceType::OpenGL )
			{
				return false;
			}

			return true;
		}

		inline bool SupportsMultithreading()
		{
			return s_RHIGlobals.SupportsMultithreading;
		}

		inline ERHITextureAlignment GetTextureAlignment()
		{
			switch ( GetRHIType() )
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

		inline ERHIShaderFormat GetShaderFormat()
		{
			switch ( GetRHIType() )
			{
				using enum ERHInterfaceType;
			case DirectX11:
			case DirectX12:
				return ERHIShaderFormat::HLSL6;
			case OpenGL:
				return ERHIShaderFormat::SPIRV_OpenGL;
			case Vulkan:
				return ERHIShaderFormat::SPIRV;
			default:
				return ERHIShaderFormat::Unknown;
			}
		}
	} // namespace RHI

	//======================================================================
	// RHI Implementation
	//======================================================================

	constexpr StringView RHI::GetRHIName( ERHInterfaceType a_API )
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