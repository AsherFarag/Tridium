#include "tripch.h"

#ifdef IS_EDITOR
#include "ViewportPanel.h"
#include <Tridium/Rendering/Camera.h>
#include <Editor/EditorLayer.h>
#include "SceneHeirarchy.h"
#include <Tridium/Scene/Scene.h>
#include <Tridium/ECS/Components/Types/Common/TransformComponent.h>

namespace Tridium::Editor {

	ViewportPanel::ViewportPanel()
		: Panel( "Viewport ##" )
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_ViewedCameraFBO = Framebuffer::Create( FBOspecification );
	}

	void ViewportPanel::OnImGuiDraw()
	{
		if ( !ViewedCamera )
			return;

		if ( m_SceneHeirarchy == nullptr )
			m_SceneHeirarchy = EditorLayer::Get().GetPanel<SceneHeirarchy>();

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		ImGui::Begin( m_Name.c_str() );
		{
			m_IsHovered = ImGui::IsWindowHovered();
			m_IsFocused = ImGui::IsWindowFocused();

			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewedCameraFBO->Bind();
			EditorLayer::Get().GetActiveScene()->Render(*ViewedCamera, CameraViewMatrix);
			m_ViewedCameraFBO->Unbind();

			ImGui::Image( (ImTextureID)m_ViewedCameraFBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

			// ImGuizmo Manipulation
			GameObject selectedGO = m_SceneHeirarchy->GetSelectedGameObject();
			if ( selectedGO )
			{
				ImGuizmo::SetOrthographic( false );
				ImGuizmo::SetDrawlist();

				ImGuizmo::SetRect( viewportBoundsMin.x, viewportBoundsMin.y,
					viewportBoundsMax.x - viewportBoundsMin.x, viewportBoundsMax.y - viewportBoundsMin.y );

				// Editor Camera
				const Matrix4& camProjection = ViewedCamera->GetProjection();

				// Selected Game Object
				TransformComponent& goTransform = selectedGO.GetTransform();
				Matrix4 goWorldTransform = goTransform.GetWorldTransform();

				// Create a Manipulation Gizmo that allows the user to easily modify the Game-Object's transform
				ImGuizmo::Manipulate( &CameraViewMatrix[0][0], &camProjection[0][0],
					(ImGuizmo::OPERATION)m_GizmoState, ImGuizmo::LOCAL,
					&goWorldTransform[0][0] );

				ImGuizmo::ViewManipulate( &CameraViewMatrix[0][0], 10.0f, { 0,0 }, { 32,32 }, IM_COL32_WHITE );

				if ( ImGuizmo::IsUsing() )
				{
					if ( goTransform.GetParent() )
						goWorldTransform * glm::inverse( goTransform.GetParent().GetWorldTransform() );

					Vector3 rot = {};
					Math::DecomposeTransform( goWorldTransform, goTransform.Position, rot, goTransform.Scale);
					goTransform.Rotation = Quaternion(rot);
				}
			}

			// If the viewport size has changed, update the FBO and Camera
			if ( m_ViewportSize != regionAvail )
			{
				m_ViewportSize = regionAvail;
				ViewedCamera->SetViewportSize( regionAvail.x, regionAvail.y );
				m_ViewedCameraFBO->Resize( regionAvail.x, regionAvail.y );
			}
		}
		ImGui::End();
	}

	bool ViewportPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		if ( !ImGuizmo::IsUsing() )
		{
			switch ( e.GetKeyCode() )
			{
				case Input::KEY_E:
				{
					m_GizmoState = EGizmoState::Translate;
					return true;
				}
				case Input::KEY_R:
				{
					m_GizmoState = EGizmoState::Rotate;
					return true;
				}
				case Input::KEY_T:
				{
					if ( Input::IsKeyPressed( Input::KEY_LEFT_CONTROL ) )
						m_GizmoState = EGizmoState::Universal_Scale;
					else
						m_GizmoState = EGizmoState::Scale;

					return true;
				}
			}
		}
	}

}


#endif // IS_EDITOR
