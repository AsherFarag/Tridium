#include "tripch.h"
#if IS_EDITOR

#include "EditorPreferencesPanel.h"
#include <Tridium/Editor/Editor.h>

namespace Tridium {

	static float s_GlobalBrightness = 1.0f;

	void SetGlobalBrightness( float a_Brightness );

	void EditorPreferencesPanel::OnImGuiDraw()
	{
		if ( !ImGuiBegin() )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::BeginMenu( "Set Editor Style" ) )
		{
			if ( ImGui::MenuItem( "Light" ) )      Editor::GetStyle().SetTheme( EditorStyle::ETheme::Light );
			if ( ImGui::MenuItem( "Dark" ) )       Editor::GetStyle().SetTheme( EditorStyle::ETheme::Dark );
			if ( ImGui::MenuItem( "Midnight" ) )   Editor::GetStyle().SetTheme( EditorStyle::ETheme::Midnight );
			if ( ImGui::MenuItem( "Retro Tech" ) ) Editor::GetStyle().SetTheme( EditorStyle::ETheme::RetroTech );

			ImGui::EndMenu();
		}

		float brightness = s_GlobalBrightness;
		if ( ImGui::SliderFloat( "Global Brightness", &brightness, 0.25f, 1.0f ) )
		{
			SetGlobalBrightness( brightness );
		}

		ImGui::Separator();

		ImGuiEnd();
	}

	void SetGlobalBrightness( float a_Brightness )
	{
		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ )
		{
			ImVec4 prevColor = style.Colors[i];

			if ( s_GlobalBrightness != 0.0f )
			{
				prevColor.x /= s_GlobalBrightness;
				prevColor.y /= s_GlobalBrightness;
				prevColor.z /= s_GlobalBrightness;
			}

			style.Colors[i] = prevColor * a_Brightness;
		}

		s_GlobalBrightness = a_Brightness;
	}
}

#endif // IS_EDITOR