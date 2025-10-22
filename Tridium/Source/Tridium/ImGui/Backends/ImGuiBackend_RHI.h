#pragma once
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/Graphics/RHI/RHIForward.h>

namespace Tridium {

	class IDynamicRHI;
	class IRHICommandList;

	bool ImGui_ImplRHI_Init( IDynamicRHI* a_RHI );

	void ImGui_ImplRHI_Shutdown();
	void ImGui_ImplRHI_NewFrame();
	void ImGui_ImplRHI_RenderDrawData( ImDrawData* a_DrawData, IDynamicRHI* a_RHI, RHITextureRef a_RenderTarget );

	bool ImGui_ImplRHI_CreateDeviceObjects();
	void ImGui_ImplRHI_DestroyDeviceObjects();

}