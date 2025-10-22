#include "tripch.h"
#include "ImGuiBackend.h"
#include "Backends/ImGuiBackend_GLFW.h"
#include "Backends/ImGuiBackend_Win32.h"

namespace Tridium {

    UniquePtr<IPlatformImGuiInterface> IPlatformImGuiInterface::Create( EImGuiPlatform a_Platform, EImGuiRenderer a_Renderer )
    {
        switch ( a_Platform )
        {
            case EImGuiPlatform::GLFW:
            {
                switch ( a_Renderer )
                {
                    case EImGuiRenderer::OpenGL:
                    case EImGuiRenderer::Vulkan:
                        return MakeUnique<GLFW::PlatformImGuiInterface_GLFW>();
                    case EImGuiRenderer::DX11:
                    case EImGuiRenderer::DX12:
                        return MakeUnique<Win32::PlatformImGuiInterface_Win32>();
                    default:
					    ENSURE( false, "Unknown ImGui renderer!" );
                }
            }
            default:
                ENSURE( false, "Unknown ImGui platform!" );
                return nullptr;
		}
    }
}