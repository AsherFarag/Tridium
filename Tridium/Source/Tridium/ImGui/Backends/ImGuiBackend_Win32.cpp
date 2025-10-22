#include "tripch.h"
#include <GLFW/glfw3.h>
#include <Tridium/ImGui/ImGui.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#undef GLFW_EXPOSE_NATIVE_WIN32
//#include <backends/imgui_impl_win32.cpp>
#include <backends/imgui_impl_glfw.h>
#include "ImGuiBackend_Win32.h"

namespace Tridium::Win32{

    bool PlatformImGuiInterface_Win32::Init( void* a_Data, bool a_InstallCallbacks, EImGuiRenderer a_Renderer )
    {
        Window = Cast<GLFWwindow*>( a_Data );
        ENSURE( Window != nullptr, "GLFW window is null!" );

        switch ( a_Renderer )
        {
            case EImGuiRenderer::DX11:
            {
                NOT_IMPLEMENTED;
                break;
            }
            case EImGuiRenderer::DX12:
            {
                //HWND hwnd = glfwGetWin32Window( Window );
                //if ( hwnd == nullptr )
                //{
                //    ENSURE( false, "Failed to get Win32 window from GLFW!" );
                //    return false;
                //}

                //if ( !ImGui_ImplWin32_Init( hwnd ) )
                //{
                //    ENSURE( false, "Failed to initialize ImGui Win32 backend!" );
                //    return false;
                //}

				return ImGui_ImplGlfw_InitForOther( Window, a_InstallCallbacks );
            }
            default:
            {
                ENSURE( false, "Unknown ImGui renderer!" );
                break;
            }
        }

        return false;
    }

    void PlatformImGuiInterface_Win32::Shutdown()
    {
		//ImGui_ImplWin32_Shutdown();
		ImGui_ImplGlfw_Shutdown();
    }

    void PlatformImGuiInterface_Win32::NewFrame()
    {
        //ImGui_ImplWin32_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
}
