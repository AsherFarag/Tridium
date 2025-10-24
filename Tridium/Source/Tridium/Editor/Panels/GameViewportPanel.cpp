#include "tripch.h"
#if IS_EDITOR

#include "GameViewportPanel.h"
#include <Tridium/ECS/Components/Types/Rendering/CameraComponent.h>
#include <Tridium/Editor/Editor.h>
#include <Tridium/Graphics/oldRendering/SceneRenderer.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Scene/SceneManager.h>

namespace Tridium {

	GameViewportPanel::GameViewportPanel( OldGameObject camera )
		: ViewportPanel("Game##GameViewportPanel"), m_Camera(camera)
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA16F, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		//m_FBO = Framebuffer::Create( FBOspecification );
	}

	void GameViewportPanel::OnImGuiDraw()
	{
#if 0
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
			camera.SetViewportSize( regionAvail.X, regionAvail.Y );
			//m_FBO->Resize( regionAvail.X, regionAvail.Y );

			OldSceneManager::GetActiveScene()->GetSceneRenderer().Render( m_FBO, camera, view, position );

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

#endif
	}

	std::optional< std::tuple<Camera&, Matrix4, Vector3> > GameViewportPanel::GetSceneCamera() const
	{
		if ( OldSceneManager::GetActiveScene() == nullptr )
			return {};
		OldCameraComponent* camera = OldSceneManager::GetActiveScene()->GetMainCamera();
		if ( !camera )
			return {};

		OldTransformComponent* transform = camera->GetGameObject().TryGetComponent<OldTransformComponent>();
		if ( !transform )
			return {};

		return { { camera->SceneCamera, camera->GetView(), transform->GetWorldPosition()}};
	}
}

#endif // IS_EDITOR