#include "tripch.h"
#if IS_EDITOR
#include "ProjectSettingsPanel.h"
#include <Tridium/IO/ProjectSerializer.h>
#include <Tridium/Engine/Engine.h>
#include <Editor/PropertyDrawers.h>

namespace Tridium {
	ProjectSettingsPanel::ProjectSettingsPanel()
		: Panel( "Project Settings" )
	{
		m_ProjectConfig = Engine::Get()->GetActiveProject().Config;
	}

	void ProjectSettingsPanel::OnImGuiDraw()
	{
		ImGuiWindowFlags flags = m_Modified ? ImGuiWindowFlags_UnsavedDocument : ImGuiWindowFlags_None;
		if ( !ImGuiBegin( flags ) )
		{
			ImGuiEnd();
			return;
		}

		m_Modified |= ImGui::InputText( "Project Name", &m_ProjectConfig.Name );
		m_Modified |= Editor::DrawProperty( "Start Scene", m_ProjectConfig.StartScene, EDrawPropertyFlags::Editable );

		ImGui::Separator();

		ImGui::Dummy( ImVec2( 0.0f, 10.0f ) );

		if ( ImGui::Button( "Save" ) )
		{
			Project& project = Engine::Get()->GetActiveProject();
			project.Config = m_ProjectConfig;
			ProjectSerializer::SerializeText( project.Config, project.Config.Editor.ProjectDirectory / project.Config.Editor.ProjectName );
			m_Modified = false;
		}

		ImGuiEnd();
	}

}

#endif // IS_EDITOR