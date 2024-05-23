#include "tripch.h"
#ifdef IS_EDITOR

#include "EditorPreferences.h"
#include "imgui.h"

namespace Tridium::Editor {
	EditorPreferences::EditorPreferences( const std::string& name )
		: Layer(name)
	{
	}

	void EditorPreferences::OnAttach()
	{
	}

	void EditorPreferences::OnDetach()
	{
	}

	void EditorPreferences::OnUpdate()
	{
	}

	void EditorPreferences::OnImGuiDraw()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_Popup;
		ImGui::Begin( "Editor Preferences" );

		ImGui::End();
	}

	void EditorPreferences::OnEvent( Event& e )
	{
	}
}

#endif // IS_EDITOR