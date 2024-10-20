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

		if ( ImGui::TreeNode( "Render Environment" ) )
		{
			DrawProperty( "Environment Map", m_Scene->GetRenderEnvironment().EnvironmentMap, EDrawPropertyFlags::Editable );

			ImGui::TreePop();
		}

		ImGui::End();
	}

}