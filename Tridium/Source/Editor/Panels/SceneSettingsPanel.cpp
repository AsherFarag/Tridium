#include "tripch.h"
#ifdef IS_EDITOR

#include "SceneSettingsPanel.h"
#include <Editor/Editor.h>
#include <Editor/PropertyDrawers.h>
#include <Tridium/Rendering/EnvironmentMap.h>
#include <Tridium/Asset/EditorAssetManager.h>

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
			if ( ImGui::TreeNode( "HDRI" ) )
			{

				if ( DrawProperty( "Environment Map", m_Scene->GetSceneEnvironment().HDRI.EnvironmentMapHandle, EDrawPropertyFlags::Editable ) )
				{
					auto assetManager = AssetManager::Get<Editor::EditorAssetManager>();
					auto textureMetaData = assetManager->GetAssetMetaData( m_Scene->GetSceneEnvironment().HDRI.EnvironmentMapHandle );
					if ( textureMetaData.IsValid() )
						m_Scene->GetSceneEnvironment().HDRI.EnvironmentMap = EnvironmentMap::Create( assetManager->GetAbsolutePath( textureMetaData.Path ) );
				}

				ImGui::SliderFloat( "Exposure", &m_Scene->GetSceneEnvironment().HDRI.Exposure, 0.0f, 10.0f );
				ImGui::SliderFloat( "Gamma", &m_Scene->GetSceneEnvironment().HDRI.Gamma, 0.0f, 10.0f );
				ImGui::SliderFloat( "Blur", &m_Scene->GetSceneEnvironment().HDRI.Blur, 0.0f, 1.0f );
				DrawProperty( "Rotation", m_Scene->GetSceneEnvironment().HDRI.RotationEular, EDrawPropertyFlags::Editable );

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "Directional Light" ) )
			{
				Vector2 direction = m_Scene->GetSceneEnvironment().DirectionalLight.Direction;
				if ( DrawProperty( "Direction", direction, EDrawPropertyFlags::Editable ) )
					m_Scene->GetSceneEnvironment().DirectionalLight.Direction = { direction.x, direction.y, 0.0f };
				DrawProperty( "Color", m_Scene->GetSceneEnvironment().DirectionalLight.Color, EDrawPropertyFlags::Editable );
				ImGui::DragFloat( "Intensity", &m_Scene->GetSceneEnvironment().DirectionalLight.Intensity, 0.1f, 0.0f );

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}

}
#endif // IS_EDITOR