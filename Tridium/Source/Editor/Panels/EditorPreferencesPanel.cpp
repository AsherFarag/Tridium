#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorPreferencesPanel.h"
#include <Editor/EditorStyle.h>

namespace Tridium::Editor {

	void EditorPreferencesPanel::OnImGuiDraw()
	{
		if ( !ImGuiBegin() )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::BeginMenu( "Set Editor Style" ) )
		{
			if ( ImGui::MenuItem( "Light" ) ) Style::SetTheme( Style::ETheme::Light );
			if ( ImGui::MenuItem( "Dark" ) ) Style::SetTheme( Style::ETheme::Dark );
			if ( ImGui::MenuItem( "Midnight" ) ) Style::SetTheme( Style::ETheme::Midnight );

			ImGui::EndMenu();
		}

		ImGui::Separator();

		ImGuiEnd();
	}
}

#endif // IS_EDITOR