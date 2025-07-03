#pragma once
#include <Tridium/ImGui/ImGuiBackend.h>
#include <GLFW/glfw3.h>

namespace Tridium {

    namespace GLFW {

        class PlatformImGuiInterface_GLFW : public IPlatformImGuiInterface
        {
        public:
            bool Init( void* a_Data, bool a_InstallCallbacks, EImGuiRenderer a_Renderer ) override;
            void Shutdown() override;
            void NewFrame() override;
            void InstallCallbacks() override;
            void RestoreCallbacks() override;

            GLFWwindow* Window = nullptr;
        };

    }

}