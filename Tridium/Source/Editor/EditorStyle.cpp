#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorStyle.h"

namespace Tridium::Editor::Style {

	static ImVec4&& operator*( const ImVec4& a_Color, float a_Value )
	{
		return ImVec4( a_Color.x * a_Value, a_Color.y * a_Value, a_Color.z * a_Value, a_Color.w * a_Value );
	}

	namespace Themes {
		static void Midnight()
		{
			ImGui::GetStyle() = ImGuiStyle();

			ImVec4* colors = ImGui::GetStyle().Colors;
			ImGuiStyle& style = ImGui::GetStyle();

			const ImVec4 white = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
			const ImVec4 gray = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
			const ImVec4 black = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );

			const ImVec4 textColor = white;
			const ImVec4 disabledTextColor = gray;
			const ImVec4 windowBgColor = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
			const ImVec4 transparentColor = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			const ImVec4 popupBgColor = ImVec4( 0.19f, 0.19f, 0.19f, 0.92f );
			const ImVec4 borderColor = ImVec4( 0.00f, 0.00f, 0.00f, 0.29f );
			const ImVec4 frameBgColor = ImVec4( 0.05f, 0.05f, 0.05f, 0.54f );
			const ImVec4 hoverColor = ImVec4( 0.19f, 0.19f, 0.19f, 0.54f );
			const ImVec4 activeColor = ImVec4( 0.20f, 0.22f, 0.23f, 1.00f );
			const ImVec4 headerColor = ImVec4( 0.20f, 0.225f, 0.25f, 0.52f );
			const ImVec4 hoveredHeaderColor = ImVec4( 0.30f, 0.325f, 0.35f, 0.52f );
			const ImVec4 checkMarkColor = ImVec4( 0.33f, 0.67f, 0.86f, 1.00f );

			// Assign colors using the common variables
			colors[ImGuiCol_Text] = textColor;
			colors[ImGuiCol_TextDisabled] = disabledTextColor;
			colors[ImGuiCol_WindowBg] = windowBgColor;
			colors[ImGuiCol_ChildBg] = transparentColor;
			colors[ImGuiCol_PopupBg] = popupBgColor;
			colors[ImGuiCol_Border] = borderColor;
			colors[ImGuiCol_BorderShadow] = ImVec4( 1.00f, 1.00f, 1.00f, 0.24f );
			colors[ImGuiCol_FrameBg] = frameBgColor;
			colors[ImGuiCol_FrameBgHovered] = hoverColor;
			colors[ImGuiCol_FrameBgActive] = activeColor;
			colors[ImGuiCol_TitleBg] = transparentColor;
			colors[ImGuiCol_TitleBgActive] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ImGuiCol_TitleBgCollapsed] = transparentColor;
			colors[ImGuiCol_MenuBarBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ImGuiCol_ScrollbarBg] = frameBgColor;
			colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.34f, 0.34f, 0.34f, 0.54f );
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.40f, 0.40f, 0.40f, 0.54f );
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.56f, 0.56f, 0.56f, 0.54f );
			colors[ImGuiCol_CheckMark] = checkMarkColor;
			colors[ImGuiCol_SliderGrab] = ImVec4( 0.34f, 0.34f, 0.34f, 0.54f );
			colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.56f, 0.56f, 0.56f, 0.54f );
			colors[ImGuiCol_Button] = headerColor;
			colors[ImGuiCol_ButtonHovered] = hoveredHeaderColor;
			colors[ImGuiCol_ButtonActive] = hoveredHeaderColor * 2;
			colors[ImGuiCol_Header] = headerColor;
			colors[ImGuiCol_HeaderHovered] = hoveredHeaderColor;
			colors[ImGuiCol_HeaderActive] = hoveredHeaderColor * 2;
			colors[ImGuiCol_Separator] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.44f, 0.44f, 0.44f, 0.29f );
			colors[ImGuiCol_SeparatorActive] = ImVec4( 0.40f, 0.44f, 0.47f, 1.00f );
			colors[ImGuiCol_ResizeGrip] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.44f, 0.44f, 0.44f, 0.29f );
			colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.40f, 0.44f, 0.47f, 1.00f );
			colors[ImGuiCol_Tab] = ImVec4( 0.05f, 0.05f, 0.05f, 0.52f );
			colors[ImGuiCol_TabHovered] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ImGuiCol_TabActive] = ImVec4( 0.40f, 0.40f, 0.40f, 0.36f );
			colors[ImGuiCol_TabUnfocused] = transparentColor;
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ImGuiCol_DockingPreview] = checkMarkColor;
			colors[ImGuiCol_DockingEmptyBg] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_PlotHistogram] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_TableHeaderBg] = transparentColor;
			colors[ImGuiCol_TableBorderStrong] = transparentColor;
			colors[ImGuiCol_TableBorderLight] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ImGuiCol_TableRowBg] = transparentColor;
			colors[ImGuiCol_TableRowBgAlt] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
			colors[ImGuiCol_TextSelectedBg] = activeColor;
			colors[ImGuiCol_DragDropTarget] = checkMarkColor;
			colors[ImGuiCol_NavHighlight] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.20f );
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.30f );

			style.WindowPadding = ImVec2( 8.00f, 8.00f );
			style.FramePadding = ImVec2( 5.00f, 2.00f );
			style.CellPadding = ImVec2( 6.00f, 6.00f );
			style.ItemSpacing = ImVec2( 6.00f, 6.00f );
			style.ItemInnerSpacing = ImVec2( 6.00f, 6.00f );
			style.TouchExtraPadding = ImVec2( 0.00f, 0.00f );
			style.IndentSpacing = 25;
			style.ScrollbarSize = 15;
			style.GrabMinSize = 10;
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 0;
			style.PopupBorderSize = 1;
			style.FrameBorderSize = 1;
			style.TabBorderSize = 1;
			style.WindowRounding = 0;
			style.ChildRounding = 4;
			style.FrameRounding = 1;
			style.PopupRounding = 4;
			style.ScrollbarRounding = 9;
			style.GrabRounding = 3;
			style.LogSliderDeadzone = 4;
			style.TabRounding = 4;
		}

		static void Light()
		{
			ImGui::StyleColorsLight();
		}

		static void Dark()
		{
			ImGui::StyleColorsDark();
		}
	}

	void SetTheme( ETheme a_Theme )
	{
		switch ( a_Theme )
		{
		case ETheme::Midnight:
			Themes::Midnight();
			break;
		case ETheme::Light:
			Themes::Light();
			break;
		case ETheme::Dark:
			Themes::Dark();
			break;
		default:
			break;
		}
	}

}

#endif // IS_EDITOR