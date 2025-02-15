#include "tripch.h"
#if IS_EDITOR

#include "EditorPreferencesPanel.h"
#include <Tridium/Editor/Editor.h>

namespace Tridium {

	static float s_GlobalBrightness = 1.0f;

	void SetGlobalBrightness( float a_Brightness, float a_OldBrightness = 1.0f );

	void EditorPreferencesPanel::OnImGuiDraw()
	{
		if ( !ImGuiBegin() )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::BeginMenu( "Set Editor Style" ) )
		{
			if ( ImGui::MenuItem( "Light" ) )      { Editor::GetStyle().SetTheme( EditorStyle::ETheme::Light ); SetGlobalBrightness( s_GlobalBrightness ); }
			if ( ImGui::MenuItem( "Dark" ) )       { Editor::GetStyle().SetTheme( EditorStyle::ETheme::Dark ); SetGlobalBrightness( s_GlobalBrightness ); }
			if ( ImGui::MenuItem( "Midnight" ) )   { Editor::GetStyle().SetTheme( EditorStyle::ETheme::Midnight ); SetGlobalBrightness( s_GlobalBrightness ); }
			if ( ImGui::MenuItem( "Retro Tech" ) ) { Editor::GetStyle().SetTheme( EditorStyle::ETheme::RetroTech ); SetGlobalBrightness( s_GlobalBrightness ); }

			ImGui::EndMenu();
		}

		float brightness = s_GlobalBrightness;
		if ( ImGui::SliderFloat( "Global Brightness", &brightness, 0.25f, 1.0f ) )
		{
			if ( brightness <= 0.0f )
				brightness = 0.25f;

			SetGlobalBrightness( brightness, s_GlobalBrightness );
			s_GlobalBrightness = brightness;
		}

		ImGui::Separator();

		ImGuiEnd();
	}

	void SetGlobalBrightness( float a_Brightness, float a_OldBrightness )
	{
		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ )
		{
			if ( i == ImGuiCol_Text )
				continue;

			ImVec4 prevColor = style.Colors[i];

			if ( a_OldBrightness != 0.0f )
			{
				prevColor.x /= a_OldBrightness;
				prevColor.y /= a_OldBrightness;
				prevColor.z /= a_OldBrightness;
			}

			style.Colors[i] = prevColor * a_Brightness;
		}
	}
}

#endif // IS_EDITOR