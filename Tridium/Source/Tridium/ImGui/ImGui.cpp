#include "tripch.h"
#include "ImGui.h"
#include <Tridium/Core/Application.h>

namespace ImGui {

	float s_FontSize = 17.5f;

	bool BorderedSelectable( const char* label, bool selected, ImGuiSelectableFlags flags, const float borderThickness, ImU32 borderColor, float rounding, const ImVec2& size )
	{
		// Get the ImGui window draw list
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Draw the selectable
		bool wasSelected = ImGui::Selectable( label, selected, flags, size );

		// Get the position and size of the last item
		ImVec2 itemMin = ImGui::GetItemRectMin();
		ImVec2 itemMax = ImGui::GetItemRectMax();

		// Adjust the position for the border to be centered correctly
		ImVec2 borderMin = ImVec2( itemMin.x - borderThickness * 0.5f * 0.5f, itemMin.y - borderThickness * 0.5f * 0.5f );
		ImVec2 borderMax = ImVec2( itemMax.x + borderThickness * 0.5f * 0.5f, itemMax.y + borderThickness * 0.5f * 0.5f );

		// Draw the border
		draw_list->AddRect( borderMin, borderMax, borderColor, rounding, 0, borderThickness );

		return wasSelected;
	}

	ImFont* GetLightFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Light.ttf" ).ToString().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetRegularFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Regular.ttf" ).ToString().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Bold.ttf" ).ToString().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetExtraBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-ExtraBold.ttf" ).ToString().c_str(), s_FontSize );

		return s_Font;
	}
}