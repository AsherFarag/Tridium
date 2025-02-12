#include "tripch.h"
#if IS_EDITOR

#include "EditorPreferencesPanel.h"
#include <Tridium/Editor/Editor.h>

namespace Tridium {

	void EditorPreferencesPanel::OnImGuiDraw()
	{
		if ( !ImGuiBegin() )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::BeginMenu( "Set Editor Style" ) )
		{
			if ( ImGui::MenuItem( "Light" ) )    Editor::GetStyle().SetTheme( EditorStyle::ETheme::Light );
			if ( ImGui::MenuItem( "Dark" ) )     Editor::GetStyle().SetTheme( EditorStyle::ETheme::Dark );
			if ( ImGui::MenuItem( "Midnight" ) ) Editor::GetStyle().SetTheme( EditorStyle::ETheme::Midnight );

			ImGui::EndMenu();
		}

		ImGui::Separator();

		ImGuiEnd();
	}
}

#endif // IS_EDITOR