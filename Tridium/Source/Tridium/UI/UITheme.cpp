#include "UITheme.h"
#include <Tridium/UI/UI.h>

namespace Tridium::UI {

    static UITheme s_Theme{};

    const UITheme& GetTheme()
    {
        return s_Theme;
    }

    void ApplyTheme( const UITheme& a_Theme )
    {
		s_Theme = a_Theme;

        ImGuiStyle& Style = ImGui::GetStyle();

        Style.ItemSpacing = ImVec2( 12, 8 );
        Style.ItemInnerSpacing = ImVec2( 8, 6 );
        Style.IndentSpacing = 25.0f;
        Style.ScrollbarSize = 15.0f;
        Style.ScrollbarRounding = 9.0f;
        Style.GrabMinSize = 5.0f;
        Style.GrabRounding = 3.0f;

        // Enable window borders
        Style.WindowBorderSize = 0.0f;     // Thickness of outer window borders
        Style.FrameBorderSize = 1.0f;      // Thickness of frame borders (buttons, inputs, etc.)
        Style.TabBorderSize = 1.0f;        // Thickness around tabs
        Style.TabBarBorderSize = 0.0f;     // Disable tab bar border
        Style.DockingSeparatorSize = 10.0f;

        Style.WindowRounding = 5.0f;
        Style.FrameRounding = 5.0f;
        Style.TabRounding = 5.0f;
        Style.ChildRounding = 5.0f;
        Style.PopupRounding = 5.0f;

        Style.WindowPadding = ImVec2( 10.0f, 10.0f );
        Style.FramePadding = ImVec2( 8.0f, 4.0f );

        // TODO
        Style.ScaleAllSizes( 1.0f );
        Style.FontScaleDpi = 1.0f;

        //===============================================================================
        // Set up colors
        //===============================================================================

        ImVec4* Colors = Style.Colors;

        const ImVec4 BackgroundColour = ImGui::ColorConvertU32ToFloat4( s_Theme.Background );
        const ImVec4 BrightBackgroundColor = ImGui::ColorConvertU32ToFloat4( s_Theme.BrightBackground );
        const ImVec4 DarkBackgroundColor = ImGui::ColorConvertU32ToFloat4( s_Theme.DarkBackground );
        const ImVec4 SelectedBackgroundColor = ImGui::ColorConvertU32ToFloat4( s_Theme.SelectedBackground );

        const ImVec4 AccentColor = ImGui::ColorConvertU32ToFloat4( s_Theme.Selected );

        // Text colors
        Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4( s_Theme.Text );
        Colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4( s_Theme.DarkText );
        Colors[ImGuiCol_TextSelectedBg] = BrightBackgroundColor;

        // Background colors
        Colors[ImGuiCol_DockingEmptyBg] = DarkBackgroundColor;
        Colors[ImGuiCol_WindowBg] = BackgroundColour;
        Colors[ImGuiCol_ChildBg] = DarkBackgroundColor;
        Colors[ImGuiCol_PopupBg] = BackgroundColour;

        // Border colors
        Colors[ImGuiCol_Border] = DarkBackgroundColor;
        Colors[ImGuiCol_BorderShadow] = DarkBackgroundColor;

        // Frame Background colors
        Colors[ImGuiCol_FrameBg] = DarkBackgroundColor;
        Colors[ImGuiCol_FrameBgHovered] = BrightBackgroundColor;
        Colors[ImGuiCol_FrameBgActive] = SelectedBackgroundColor;

        // Title Background colors
        Colors[ImGuiCol_TitleBg] = DarkBackgroundColor;
        Colors[ImGuiCol_TitleBgActive] = DarkBackgroundColor;
        Colors[ImGuiCol_TitleBgCollapsed] = DarkBackgroundColor;

        // Menu Bar colors
        Colors[ImGuiCol_MenuBarBg] = DarkBackgroundColor;

        // Scrollbar colors
        Colors[ImGuiCol_ScrollbarBg] = DarkBackgroundColor;
        Colors[ImGuiCol_ScrollbarGrab] = BrightBackgroundColor;
        Colors[ImGuiCol_ScrollbarGrabHovered] = SelectedBackgroundColor;
        Colors[ImGuiCol_ScrollbarGrabActive] = AccentColor;

        // Checkmark colors
        Colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4( s_Theme.Text );

        // Slider colors
        Colors[ImGuiCol_SliderGrab] = BrightBackgroundColor;
        Colors[ImGuiCol_SliderGrabActive] = AccentColor;

        // Button colors
        Colors[ImGuiCol_Button] = BackgroundColour;
        Colors[ImGuiCol_ButtonHovered] = BrightBackgroundColor;
        Colors[ImGuiCol_ButtonActive] = SelectedBackgroundColor;

        // Header colors
        Colors[ImGuiCol_Header] = BackgroundColour;
        Colors[ImGuiCol_HeaderHovered] = BrightBackgroundColor;
        Colors[ImGuiCol_HeaderActive] = SelectedBackgroundColor;

        // Resize grip colors
        Colors[ImGuiCol_ResizeGripHovered] = SelectedBackgroundColor;
        Colors[ImGuiCol_ResizeGripActive] = AccentColor;

        // Separator colors
        Colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4( s_Theme.Underline );
        Colors[ImGuiCol_SeparatorHovered] = BrightBackgroundColor;
        Colors[ImGuiCol_SeparatorActive] = AccentColor;

        // Tab colors
        Colors[ImGuiCol_Tab] = DarkBackgroundColor;
        Colors[ImGuiCol_TabHovered] = BrightBackgroundColor;
        Colors[ImGuiCol_TabSelected] = BackgroundColour;
        Colors[ImGuiCol_TabSelectedOverline] = BackgroundColour;
        Colors[ImGuiCol_TabDimmed] = DarkBackgroundColor;
        Colors[ImGuiCol_TabDimmedSelected] = BackgroundColour;
        Colors[ImGuiCol_TabDimmedSelectedOverline] = BackgroundColour;

        // Docking colours
        Colors[ImGuiCol_DockingPreview] = SelectedBackgroundColor; // Preview color when docking a window
        Colors[ImGuiCol_DockingEmptyBg] = DarkBackgroundColor; // Background color for empty docking nodes
        //Colors[ ImGuiCol_ModalWindowDimBg ] = AccentColor;

        // Navigation colors
        Colors[ImGuiCol_NavCursor] = AccentColor;
        Colors[ImGuiCol_NavWindowingHighlight] = BrightBackgroundColor;
        Colors[ImGuiCol_NavWindowingDimBg] = DarkBackgroundColor;

        // Miscellaneous colors
        Colors[ImGuiCol_DragDropTarget] = AccentColor;
    }

} // namespace Tridium::UI
