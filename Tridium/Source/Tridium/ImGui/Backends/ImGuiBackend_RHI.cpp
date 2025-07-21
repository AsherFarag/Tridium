#include "tripch.h"
#include "ImGuiBackend_RHI.h"

namespace Tridium {

    struct ImGui_ImplRHI_Data
    {
		IDynamicRHI* RHI = nullptr;
    };

    // Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
    // It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
    static ImGui_ImplRHI_Data* ImGui_ImplRHI_GetBackendData()
    {
        return ImGui::GetCurrentContext() ? ( ImGui_ImplRHI_Data* )ImGui::GetIO().BackendRendererUserData : nullptr;
    }

    //struct 

    bool ImGui_ImplRHI_Init( IDynamicRHI* a_RHI )
    {
        return false;
    }

}