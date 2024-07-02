#include "tripch.h"
#ifdef IS_EDITOR

#include "GameViewportPanel.h"
#include <Tridium/ECS/Components/Types/Rendering/CameraComponent.h>
#include <Editor/EditorLayer.h>

namespace Tridium::Editor {

	GameViewportPanel::GameViewportPanel( GameObject camera )
		: ViewportPanel("Game##GameViewportPanel"), m_Camera(camera)
	{
	}

	void GameViewportPanel::OnImGuiDraw()
	{
		if ( !m_Camera )
		{
			CameraComponent* mainCam = EditorLayer::Get().GetActiveScene()->GetMainCamera();
			if ( mainCam )
				m_Camera = mainCam->GetGameObject();
		}

		auto camera = m_Camera.TryGetComponent<CameraComponent>();

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );
		if ( ImGui::Begin( m_Name.c_str() ) && camera )
		{
			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			Camera& sceneCamera = camera->SceneCamera;
			// Update the viewport size
			m_ViewportSize = regionAvail;
			sceneCamera.SetViewportSize( regionAvail.x, regionAvail.y );
			m_FBO->Resize( regionAvail.x, regionAvail.y );

			m_FBO->Bind();
			EditorLayer::Get().GetActiveScene()->Render( sceneCamera, camera->GetView() );
			m_FBO->Unbind();

			ImGui::Image( (ImTextureID)m_FBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );
		}
		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();
		ImGui::End();
	}
}

#endif // IS_EDITOR