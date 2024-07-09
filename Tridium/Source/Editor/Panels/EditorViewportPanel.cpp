#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorViewportPanel.h"

#include <Editor/Editor.h>
#include <Editor/EditorCamera.h>
#include "SceneHeirarchyPanel.h"

#include <Tridium/ECS/Components/Types.h>`

namespace Tridium::Editor {

	EditorViewportPanel::EditorViewportPanel( const Ref<EditorCamera>& editorCamera )
		: ViewportPanel( "Scene##EditorViewportPanel" ), m_EditorCamera( editorCamera )
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA8, EFramebufferTextureFormat::RED_INT, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_FBO = Framebuffer::Create( FBOspecification );
	}

	bool EditorViewportPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		if ( !ImGuizmo::IsUsing() )
		{
			switch ( e.GetKeyCode() )
			{
			case Input::KEY_W:
			{
				m_GizmoState = EGizmoState::Translate;
				return true;
			}
			case Input::KEY_E:
			{
				m_GizmoState = EGizmoState::Rotate;
				return true;
			}
			case Input::KEY_R:
			{
				if ( Input::IsKeyPressed( Input::KEY_LEFT_CONTROL ) )
					m_GizmoState = EGizmoState::Universal_Scale;
				else
					m_GizmoState = EGizmoState::Scale;

				return true;
			}
			}
		}

		return false;
	}

	void EditorViewportPanel::OnImGuiDraw()
	{
		if ( !m_EditorCamera )
			return;

		if ( m_SceneHeirarchy == nullptr )
			m_SceneHeirarchy = GetEditorLayer()->GetPanel<SceneHeirarchyPanel>();

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			// Update the viewport size
			m_ViewportSize = regionAvail;
			m_EditorCamera->SetViewportSize( regionAvail.x, regionAvail.y );
			m_FBO->Resize( regionAvail.x, regionAvail.y );

			m_FBO->Bind();
			GetEditorLayer()->GetActiveScene()->Render( *m_EditorCamera, m_EditorCamera->GetViewMatrix() );
			m_FBO->Unbind();

			// Draw the Editor Camera ViewPort
			ImGui::Image( (ImTextureID)m_FBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

			// Drag-Drop for scenes
			DragDropTarget();

			DrawManipulationGizmos( viewportBoundsMin, viewportBoundsMax );

			if ( m_IsFocused && ImGui::IsItemClicked() && !ImGuizmo::IsUsingAny() )
			{
				auto [mx, my] = ImGui::GetMousePos();
				mx -= viewportBoundsMin.x;
				my -= viewportBoundsMin.y;
				my = m_ViewportSize.y - my;
				int mouseX = (int)mx;
				int mouseY = (int)my;

				m_FBO->Bind();
				int goID = m_FBO->ReadPixel( 1, mouseX, mouseY );
				m_FBO->Unbind();

				GetEditorLayer()->GetPanel<SceneHeirarchyPanel>()->SetSelectedGameObject( (EntityID)goID );
			}
		}
		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();
		m_EditorCamera->Focused = m_IsFocused && !ImGuizmo::IsUsingAny();
		ImGui::End();
	}

	void EditorViewportPanel::DragDropTarget()
	{
		ImGui::ScopedDragDropTarget scopedDragDropTarget;
		if ( !( scopedDragDropTarget ) )
			return;

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_CONTENT_BROWSER_ITEM, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
		if ( !payload )
			return;

		fs::path filePath( (const char*)payload->Data );

		if ( !filePath.has_extension() )
			return;

		if ( filePath.extension() == ".tridium" )
		{
			GetEditorLayer()->LoadScene( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );
		}
	}

	void EditorViewportPanel::DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax )
	{
		// Editor Camera
		const Matrix4& camProjection = m_EditorCamera->GetProjection();
		Matrix4 camView = m_EditorCamera->GetViewMatrix();

		// Set up ImGuizmo
		ImGuizmo::SetOrthographic( false );
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect( viewportBoundsMin.x, viewportBoundsMin.y,
			viewportBoundsMax.x - viewportBoundsMin.x, viewportBoundsMax.y - viewportBoundsMin.y );

		if ( GameObject selectedGO = GetSceneHeirarchy()->GetSelectedGameObject() )
		{
			// Selected Game Object
			TransformComponent& goTransform = selectedGO.GetTransform();
			Matrix4 goWorldTransform = goTransform.GetWorldTransform();

			bool shouldSnap = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );

			Vector3 snapVals( 0.25f );
			if ( m_GizmoState == EGizmoState::Rotate )
				snapVals = Vector3( 45.0f );

			// Create a Manipulation Gizmo that allows the user to easily modify the Game-Object's transform
			ImGuizmo::Manipulate( &camView[0][0], &camProjection[0][0],
				(ImGuizmo::OPERATION)m_GizmoState, ImGuizmo::LOCAL,
				&goWorldTransform[0][0], nullptr, shouldSnap ? &snapVals[0] : nullptr );

			ImGuizmo::ViewManipulate( &camView[0][0], 8.0f, { viewportBoundsMax.x - 75, viewportBoundsMin.y }, { 75, 75 }, 0x10101010 );

			if ( ImGuizmo::IsUsingAny() )
			{
				if ( goTransform.GetParent() )
					goWorldTransform = glm::inverse( goTransform.GetParent().GetWorldTransform() ) * goWorldTransform;

				Math::DecomposeTransform( goWorldTransform, goTransform.Position, goTransform.Rotation, goTransform.Scale );
			}
		}
		else
		{
			Matrix4 identity( 1.0f );
			ImGuizmo::ViewManipulate( &camView[0][0], &camProjection[0][0],
				(ImGuizmo::OPERATION)m_GizmoState, ImGuizmo::LOCAL,
				&identity[0][0], 8.0f, ImVec2( viewportBoundsMax.x - 75, viewportBoundsMin.y ), ImVec2( 75, 75 ), 0x10101010 );
		}
	}

	SceneHeirarchyPanel* EditorViewportPanel::GetSceneHeirarchy()
	{
		if ( !m_SceneHeirarchy )
			m_SceneHeirarchy = GetEditorLayer()->GetPanel<SceneHeirarchyPanel>();

		return m_SceneHeirarchy;
	}
}

#endif // IS_EDITOR
