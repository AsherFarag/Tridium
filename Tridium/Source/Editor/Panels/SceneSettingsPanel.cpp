#include "tripch.h"
#include "SceneSettingsPanel.h"
#include <Editor/Editor.h>
#include <Editor/PropertyDrawers.h>

namespace Tridium::Editor {

	SceneSettingsPanel::SceneSettingsPanel()
		: Panel( "Scene Settings" )
	{
		m_Scene = GetEditorLayer()->GetActiveScene();
	}

	void SceneSettingsPanel::OnImGuiDraw()
	{
		if ( !ImGui::Begin( m_Name.c_str() ) )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::TreeNode( "Scene Environment" ) )
		{
			DrawProperty( "Environment Map", m_Scene->GetSceneEnvironment().HDRI.EnvironmentMap, EDrawPropertyFlags::Editable );
			ImGui::SliderFloat( "Exposure", &m_Scene->GetSceneEnvironment().HDRI.Exposure, 0.0f, 10.0f );
			ImGui::SliderFloat( "Gamma", &m_Scene->GetSceneEnvironment().HDRI.Gamma, 0.0f, 10.0f );
			DrawProperty( "Rotation", m_Scene->GetSceneEnvironment().HDRI.RotationEular, EDrawPropertyFlags::Editable );

			ImGui::TreePop();
		}

		ImGui::End();
	}

}