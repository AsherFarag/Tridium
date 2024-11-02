#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorViewportPanel.h"

#include <Editor/Editor.h>
#include <Editor/EditorUtil.h>
#include <Editor/EditorCamera.h>
#include "SceneHeirarchyPanel.h"

#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Asset/EditorAssetManager.h>

// TEMP ?
#include "Tridium/Asset/AssetManager.h"
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/VertexArray.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium::Editor {

	void SetImGuizmoColors()
	{
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_X] = Style::Colors::Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Y] = Style::Colors::Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Z] = Style::Colors::Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_X] = Style::Colors::Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Y] = Style::Colors::Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Z] = Style::Colors::Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::SELECTION] = Style::Colors::Orange;
	}

	EditorViewportPanel::EditorViewportPanel( const SharedPtr<EditorCamera>& editorCamera )
		: ViewportPanel( "Scene##EditorViewportPanel" ), m_EditorCamera( editorCamera )
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA16F, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_FBO = Framebuffer::Create( FBOspecification );

		FBOspecification.Attachments = { EFramebufferTextureFormat::RED_INT };
		m_IDFBO = Framebuffer::Create( FBOspecification );

		std::string idVert =
			R"(
			#version 420

			layout( location = 0 ) in vec3 aPosition;

			uniform int uID;
			uniform mat4 uPVM;

			flat out int vID;

			void main()
			{
				gl_Position = uPVM * vec4( aPosition, 1 );
				vID = uID;
			}
		)";


		std::string idFrag =
			R"(
			#version 420 core
			
			layout(location = 0) out int oID;
			
			flat in int vID;						
			
			void main()
			{
				oID = vID;
			}
		)";

		m_GameObjectIDShader.reset( Shader::Create( idVert, idFrag ) );

		m_OutlineShader.reset( Shader::Create() );
		m_OutlineShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Simple.glsl");
	}

	bool EditorViewportPanel::OnKeyPressed( KeyPressedEvent& e )
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

			GetEditorLayer()->GetActiveScene()->GetSceneRenderer().Render(m_FBO, *m_EditorCamera, m_EditorCamera->GetViewMatrix(), m_EditorCamera->Position);
			RenderSelectionOutline();

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

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
		if ( !payload )
			return;

		AssetHandle assetHandle( *(AssetHandle*)payload->Data );

		const AssetMetaData& assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( assetHandle );

		switch ( assetMetaData.AssetType )
		{
		case EAssetType::Scene:
		{
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( assetHandle ) )
			{
				GetEditorLayer()->GetActiveScene()->Clear();
				// Load the scene
				GetEditorLayer()->SetActiveScene( scene );
			}
			break;
		}
		default:
			break;
		}
	}

	void EditorViewportPanel::DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax )
	{
		// Editor Camera
		const Matrix4& camProjection = m_EditorCamera->GetProjection();
		Matrix4 camView = m_EditorCamera->GetViewMatrix();

		// Set up ImGuizmo
		SetImGuizmoColors();
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

				Quaternion rotation = goTransform.Rotation.Quat;
				Math::DecomposeTransform( goWorldTransform, goTransform.Position, rotation, goTransform.Scale );
				goTransform.Rotation.SetFromQuaternion( rotation );
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
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		m_GameObjectIDShader->Bind();

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();

		auto meshComponents = GetActiveScene()->GetRegistry().view<StaticMeshComponent, TransformComponent>();
		meshComponents.each( 
			[&]( auto go, StaticMeshComponent& meshComponent, TransformComponent& transform )
			{
				if ( !meshComponent.Mesh.Valid() )
					return;

				SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( meshComponent.Mesh );
				if ( !mesh )
					return;

				SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
				if ( !meshSource )
					return;

				m_GameObjectIDShader->SetInt( "uID", static_cast<uint32_t>( go ) );
				for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
				{
					const SubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
					if ( const SharedPtr<VertexArray>& vao = subMesh.VAO )
					{
						m_GameObjectIDShader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() * subMesh.Transform );

						vao->Bind();
						RenderCommand::DrawIndexed( vao );
						vao->Unbind();
					}
				}
			} );

		m_GameObjectIDShader->Unbind();
	}

	// TEMP!
#include "glad/glad.h"


	void EditorViewportPanel::RenderSelectionOutline()
	{
		if ( !m_FBO )
			return;

		GameObject go = GetSceneHeirarchy()->GetSelectedGameObject();
		if ( !go )
			return;

		StaticMeshComponent* meshComponent = go.TryGetComponent<StaticMeshComponent>();
		if ( !meshComponent )
			return;

		if ( !meshComponent->Mesh.Valid() )
			return;

		SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( meshComponent->Mesh );
		if ( !mesh )
			return;

		SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
		if ( !meshSource )
			return;

		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetCullMode( ECullMode::Front );

		m_FBO->Bind();
		m_OutlineShader->Bind();

		TODO( "Remove this opengl code!" );
		glLineWidth( 5 );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();
		for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
		{
			const SubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
			if ( const SharedPtr<VertexArray>& vao = subMesh.VAO )
			{
				m_OutlineShader->SetMatrix4( "u_PVM", pvm * go.GetWorldTransform() * subMesh.Transform );

				vao->Bind();
				RenderCommand::DrawIndexed( vao );
				vao->Unbind();
			}
		}

		m_OutlineShader->Unbind();
		m_FBO->Unbind();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	SceneHeirarchyPanel* EditorViewportPanel::GetSceneHeirarchy()
	{
		if ( !m_SceneHeirarchy )
			m_SceneHeirarchy = GetEditorLayer()->GetPanel<SceneHeirarchyPanel>();

		return m_SceneHeirarchy;
	}
}

#endif // IS_EDITOR
