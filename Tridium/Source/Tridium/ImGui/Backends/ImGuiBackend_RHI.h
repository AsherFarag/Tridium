#pragma once
#include <imgui.h>
namespace Tridium {

	class IDynamicRHI;
	class IRHICommandList;

	bool ImGui_ImplRHI_Init( IDynamicRHI* a_RHI );

	void ImGui_ImplRHI_Shutdown();
	void ImGui_ImplRHI_NewFrame();
	void ImGui_ImplRHI_RenderDrawData( ImDrawData* a_DrawData, IRHICommandList* a_RHI );

}