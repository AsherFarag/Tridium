#pragma once
#include <imgui.h>

#define TE_PAYLOAD_CONTENT_BROWSER_ITEM "ContentBrowserItem"

namespace ImGui {
	
	bool BorderedSelectable( const char* label, bool selected, ImGuiSelectableFlags flags = 0, const float borderThickness = 1.0f, ImU32 borderColor = IM_COL32( 255, 255, 255, 255 ), float rounding = 0.0f, const ImVec2& size = ImVec2( 0, 0 ) );


	struct ScopedDragDropTarget
	{
	public:
		ScopedDragDropTarget() { m_Successful = ImGui::BeginDragDropTarget(); }
		~ScopedDragDropTarget() { if ( m_Successful ) ImGui::EndDragDropTarget(); }

		operator bool() { return m_Successful; }
		operator const bool() const { return m_Successful; }

	private:
		bool m_Successful = false;
	};

	struct ScopedStyleVar
	{
	public:
		ScopedStyleVar( ImGuiStyleVar_ styleVar, float val ) { ImGui::PushStyleVar( styleVar, val ); }
		ScopedStyleVar( ImGuiStyleVar_ styleVar, ImVec2 val ) { ImGui::PushStyleVar( styleVar, val ); }
		~ScopedStyleVar() { ImGui::PopStyleVar(); }
	};

	struct ScopedStyleCol
	{
	public:
		ScopedStyleCol( ImGuiCol styleCol, ImVec4 col ) { ImGui::PushStyleColor( styleCol, col ); }
		ScopedStyleCol( ImGuiCol styleCol, ImU32 col ) { ImGui::PushStyleColor( styleCol, col ); }
		~ScopedStyleCol() { ImGui::PopStyleColor(); }
	};

	ImFont* GetLightFont();
	ImFont* GetRegularFont();
	ImFont* GetBoldFont();
	ImFont* GetExtraBoldFont();
}