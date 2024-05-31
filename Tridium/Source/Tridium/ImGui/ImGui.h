#pragma once
#include <imgui.h>

namespace ImGui {

	class ScopedStyleVar
	{
	public:
		ScopedStyleVar( ImGuiStyleVar_ styleVar, float val ) { ImGui::PushStyleVar( styleVar, val ); }
		ScopedStyleVar( ImGuiStyleVar_ styleVar, ImVec2 val ) { ImGui::PushStyleVar( styleVar, val ); }
		~ScopedStyleVar() { ImGui::PopStyleVar(); }
	};

	class ScopedStyleCol
	{
	public:
		ScopedStyleCol( ImGuiCol styleCol, ImVec4 col ) { ImGui::PushStyleColor( styleCol, col ); }
		ScopedStyleCol( ImGuiCol styleCol, ImU32 col ) { ImGui::PushStyleColor( styleCol, col ); }
		~ScopedStyleCol() { ImGui::PopStyleColor(); }
	};

}