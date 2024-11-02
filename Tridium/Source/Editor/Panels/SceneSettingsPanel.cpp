#include "tripch.h"
#ifdef IS_EDITOR

#include "SceneSettingsPanel.h"
#include <Editor/Editor.h>
#include <Editor/PropertyDrawers.h>
#include <Tridium/Rendering/EnvironmentMap.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Rendering/FrameBuffer.h>

namespace Tridium::Editor {

	SceneRendererPanel::SceneRendererPanel()
		: Panel( "Scene Renderer" )
	{
	}

	void SceneRendererPanel::OnImGuiDraw()
	{
		if ( !ImGui::Begin( m_Name.c_str() ) )
		{
			ImGui::End();
			return;
		}

		SharedPtr<Scene> scene = Application::GetScene();
		if ( ImGui::TreeNode( "Scene Environment" ) )
		{
			if ( ImGui::TreeNode( "HDRI" ) )
			{

				if ( DrawProperty( "Environment Map", scene->GetSceneEnvironment().HDRI.EnvironmentMapHandle, EDrawPropertyFlags::Editable ) )
				{
					auto assetManager = AssetManager::Get<Editor::EditorAssetManager>();
					auto textureMetaData = assetManager->GetAssetMetaData( scene->GetSceneEnvironment().HDRI.EnvironmentMapHandle );
					if ( textureMetaData.IsValid() )
						scene->GetSceneEnvironment().HDRI.EnvironmentMap = EnvironmentMap::Create( assetManager->GetAbsolutePath( textureMetaData.Path ) );
				}

				ImGui::SliderFloat( "Exposure", &scene->GetSceneEnvironment().HDRI.Exposure, 0.0f, 10.0f );
				ImGui::SliderFloat( "Gamma", &scene->GetSceneEnvironment().HDRI.Gamma, 0.0f, 10.0f );
				ImGui::SliderFloat( "Blur", &scene->GetSceneEnvironment().HDRI.Blur, 0.0f, 1.0f );
				ImGui::SliderFloat( "Intensity", &scene->GetSceneEnvironment().HDRI.Intensity, 0.0f, 10.0f );
				DrawProperty( "Rotation", scene->GetSceneEnvironment().HDRI.RotationEular, EDrawPropertyFlags::Editable );

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if ( ImGui::Button( "Temp! Recompile Shader" ) )
		{
			scene->GetSceneRenderer().m_DefaultShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/PBR-Shadows.glsl" );
		}

		ImGui::End();
	}

}
#endif // IS_EDITOR