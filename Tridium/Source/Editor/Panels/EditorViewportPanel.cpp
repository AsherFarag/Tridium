#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorViewportPanel.h"

#include <Editor/Editor.h>
#include <Editor/EditorUtil.h>
#include <Editor/EditorCamera.h>
#include "SceneHeirarchyPanel.h"

#include <Tridium/ECS/Components/Types.h>`
#include <Tridium/Scene/Scene.h>

// TEMP ?
#include "Tridium/Core/AssetManager.h"
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/VertexArray.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium::Editor {

	EditorViewportPanel::EditorViewportPanel( const SharedPtr<EditorCamera>& editorCamera )
		: ViewportPanel( "Scene##EditorViewportPanel" ), m_EditorCamera( editorCamera )
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA8, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_FBO = Framebuffer::Create( FBOspecification );

		FBOspecification.Attachments = { EFramebufferTextureFormat::RED_INT };
		m_IDFBO = Framebuffer::Create( FBOspecification );

		m_GameObjectIDShader = AssetManager::LoadAsset<Shader>( "Content/Engine/Editor/Shaders/ID.glsl" );
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
				m_IDFBO->Resize( regionAvail.x, regionAvail.y );

				auto [mx, my] = ImGui::GetMousePos();
				mx -= viewportBoundsMin.x;
				my -= viewportBoundsMin.y;
				my = m_ViewportSize.y - my;
				int mouseX = (int)mx;
				int mouseY = (int)my;

				m_IDFBO->Bind();
				RenderGameObjectIDs();
				int goID = m_FBO->ReadPixel( 0, mouseX, mouseY );
				m_IDFBO->Unbind();

				GetSceneHeirarchy()->SetSelectedGameObject((EntityID)goID);
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

	void EditorViewportPanel::RenderGameObjectIDs()
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		m_GameObjectIDShader->Bind();

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();

		auto meshComponents = GetActiveScene()->GetRegistry().view<MeshComponent, TransformComponent>();
		RenderCommand::SetCullMode( true );
		meshComponents.each( 
			[&]( auto go, MeshComponent& meshComponent, TransformComponent& transform )
			{
				if ( meshComponent.GetMesh() )
				{
					m_GameObjectIDShader->SetInt( "uID", (uint32_t)go );
					m_GameObjectIDShader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );

					meshComponent.GetMesh()->GetVAO()->Bind();
					RenderCommand::DrawIndexed( meshComponent.GetMesh()->GetVAO() );
					meshComponent.GetMesh()->GetVAO()->Unbind();
				}
			} );

		RenderCommand::SetCullMode( false );

		//auto quadMeshVAO = Mesh::GetQuad()->GetVAO();
		//quadMeshVAO->Bind();
		//auto spriteComponents = GetActiveScene()->GetRegistry().view<SpriteComponent, TransformComponent>();
		//spriteComponents.each( 
		//	[&]( auto go, SpriteComponent& spriteComponent, TransformComponent& transform )
		//	{
		//		m_GameObjectIDShader->SetInt( "uID", (uint32_t)go );
		//		m_GameObjectIDShader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );
		//		RenderCommand::DrawIndexed( quadMeshVAO );
		//	}
		//);
		//quadMeshVAO->Unbind();

		//m_GameObjectIDShader->Unbind();
	}

	SceneHeirarchyPanel* EditorViewportPanel::GetSceneHeirarchy()
	{
		if ( !m_SceneHeirarchy )
			m_SceneHeirarchy = GetEditorLayer()->GetPanel<SceneHeirarchyPanel>();

		return m_SceneHeirarchy;
	}
}

#endif // IS_EDITOR
