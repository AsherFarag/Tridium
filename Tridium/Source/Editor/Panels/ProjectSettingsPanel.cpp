#include "tripch.h"
#include "ProjectSettingsPanel.h"
#include <Tridium/IO/ProjectSerializer.h>
#include <Tridium/Core/Application.h>

namespace Tridium::Editor {
	ProjectSettingsPanel::ProjectSettingsPanel()
		: Panel( "Project Settings" )
	{
		m_ProjectConfig = Application::GetActiveProject()->GetConfiguration();
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
		m_Modified |= DrawProperty( "Start Scene", m_ProjectConfig.StartScene, EDrawPropertyFlags::Editable );

		ImGui::Separator();

		ImGui::Dummy( ImVec2( 0.0f, 10.0f ) );

		if ( ImGui::Button( "Save" ) )
		{
			auto project = Application::GetActiveProject();
			project->GetConfiguration() = m_ProjectConfig;

			ProjectSerializer serializer( project );
			serializer.SerializeText( m_ProjectConfig.ProjectDirectory / m_ProjectConfig.Name );
			m_Modified = false;
		}

		ImGuiEnd();
	}

}
