#include "tripch.h"
#if IS_EDITOR

#include "GameViewportPanel.h"
#include <Tridium/ECS/Components/Types/Rendering/CameraComponent.h>
#include <Editor/Editor.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/ECS/Components/Types.h>

namespace Tridium::Editor {

	GameViewportPanel::GameViewportPanel( GameObject camera )
		: ViewportPanel("Game##GameViewportPanel"), m_Camera(camera)
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA16F, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_FBO = Framebuffer::Create( FBOspecification );
	}

	void GameViewportPanel::OnImGuiDraw()
	{
		auto sceneCameraInfo = GetSceneCamera();
		if ( !sceneCameraInfo )
			return;

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );
		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			auto&& [camera, view, position] = sceneCameraInfo.value();

			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			// Update the viewport size
			m_ViewportSize = regionAvail;
			camera.SetViewportSize( regionAvail.x, regionAvail.y );
			m_FBO->Resize( regionAvail.x, regionAvail.y );

			GetEditorLayer()->GetActiveScene()->GetSceneRenderer().Render( m_FBO, camera, view, position );

			// Draw Debug Lines
			{
				m_FBO->Bind();
				Debug::DebugDrawer::Get()->Draw( camera.GetProjection() * view );
				m_FBO->Unbind();
			}

			ImGui::Image( (ImTextureID)m_FBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();

		ImGui::End();
	}

	std::optional< std::tuple<Camera&, Matrix4, Vector3> > GameViewportPanel::GetSceneCamera() const
	{
		CameraComponent* camera = Application::GetScene()->GetMainCamera();
		if ( !camera )
			return {};

		TransformComponent* transform = camera->GetGameObject().TryGetComponent<TransformComponent>();
		if ( !transform )
			return {};

		return { { camera->SceneCamera, camera->GetView(), transform->GetWorldPosition()}};
	}
}

#endif // IS_EDITOR