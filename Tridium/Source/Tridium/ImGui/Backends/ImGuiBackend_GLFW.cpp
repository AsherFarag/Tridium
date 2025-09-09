#include "tripch.h"
#include "ImGuiBackend_GLFW.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#undef GLFW_EXPOSE_NATIVE_WIN32
#include <backends/imgui_impl_glfw.cpp>

namespace Tridium::GLFW {
    bool PlatformImGuiInterface_GLFW::Init( void* a_Data, bool a_InstallCallbacks, EImGuiRenderer a_Renderer )
    {
        Window = Cast<GLFWwindow*>( a_Data );
        ENSURE( Window != nullptr, "GLFW window is null!" );

        switch ( a_Renderer )
        {
            case EImGuiRenderer::OpenGL:
            {
                if ( !ImGui_ImplGlfw_InitForOpenGL( Window, a_InstallCallbacks ) )
                {
                    ENSURE( false, "Failed to initialize ImGui GLFW backend!" );
                    return false;
                }
                break;
            }
            case EImGuiRenderer::Vulkan:
            {
                if ( !ImGui_ImplGlfw_InitForVulkan( Window, a_InstallCallbacks ) )
                {
                    ENSURE( false, "Failed to initialize ImGui GLFW Vulkan backend!" );
                    return false;
                }
                break;
            }
            default:
            {
                ENSURE( false, "Unknown ImGui renderer!" );
                return false;
            }
        }

        return true;
    }

    void PlatformImGuiInterface_GLFW::Shutdown()
    {
		ImGui_ImplGlfw_Shutdown();
    }

    void PlatformImGuiInterface_GLFW::NewFrame()
    {
        ImGui_ImplGlfw_NewFrame();
	}

    void PlatformImGuiInterface_GLFW::InstallCallbacks()
    {
        ImGui_ImplGlfw_InstallCallbacks( Window );
    }

    void PlatformImGuiInterface_GLFW::RestoreCallbacks()
    {
        ImGui_ImplGlfw_RestoreCallbacks( Window );
	}
}
