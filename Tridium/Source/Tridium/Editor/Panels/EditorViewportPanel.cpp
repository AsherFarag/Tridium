#include "tripch.h"
#if IS_EDITOR
#include "EditorViewportPanel.h"

#include <Tridium/ECS/GameObject.h>

#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/EditorUtil.h>
#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Editor/EditorStyle.h>

#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Graphics/Rendering/SceneRenderer.h>
#include <Tridium/Asset/EditorAssetManager.h>

#include <Tridium/Debug/DebugDrawer.h>

// TEMP ?
#include "Tridium/Asset/AssetManager.h"
#include <Tridium/Graphics/Rendering/RenderCommand.h>
#include <Tridium/Graphics/Rendering/VertexArray.h>
#include <Tridium/Graphics/Rendering/Shader.h>

namespace Tridium {

	void SetImGuizmoColors()
	{
		ImGuizmo::GetStyle() = ImGuizmo::Style();
		EditorStyle::Pallete& pal = Editor::GetPallete();

		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_X] = pal.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Y] = pal.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Z] = pal.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_X] = pal.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Y] = pal.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Z] = pal.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::SELECTION] = pal.Orange;
		ImGuizmo::GetStyle().RotationOuterLineThickness *= 2.0f;
		ImGuizmo::GetStyle().RotationLineThickness *= 2.0f;
		ImGuizmo::GetStyle().TranslationLineThickness *= 2.0f;
		//ImGuizmo::GetStyle().TranslationLineArrowSize *= 2.0f;
		//ImGuizmo::GetStyle().ScaleLineCircleSize *= 2.0f;
		ImGuizmo::GetStyle().ScaleLineThickness *= 2.0f;
	}

	EditorViewportPanel::EditorViewportPanel( const SharedPtr<EditorCamera>& editorCamera )
		: ViewportPanel( "Scene##EditorViewportPanel" ), m_EditorCamera( editorCamera )
	{
		m_OnGameObjectSelectedHandle = Editor::Events::OnGameObjectSelected.Add<&EditorViewportPanel::SetSelectedGameObject>( this );

		// Set up ID Selection
		{
			FramebufferSpecification FBOspecification;
			FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA16F, EFramebufferTextureFormat::Depth };
			FBOspecification.Width = 1280;
			FBOspecification.Height = 720;
			m_FBO = Framebuffer::Create( FBOspecification );

			FBOspecification.Attachments = { EFramebufferTextureFormat::RED_INT, EFramebufferTextureFormat::Depth };
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
			m_OutlineShader->Compile( Engine::Get()->GetEngineAssetsDirectory() / "Shaders/Simple.glsl" );
		}
	}

	EditorViewportPanel::~EditorViewportPanel()
	{
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
			case Input::KEY_C:
			{
				if ( Input::IsKeyPressed( Input::KEY_LEFT_CONTROL ) )
				{
					if ( m_SelectedGameObject )
					{
						EditorApplication::Get()->GetPayloadManager().SetPayload( "GameObject", m_SelectedGameObject );
					}
				}
				return true;
			}
			case Input::KEY_V:
			{
				if ( Input::IsKeyPressed( Input::KEY_LEFT_CONTROL ) )
				{
					EditorPayload* payload = EditorApplication::Get()->GetPayloadManager().GetPayload( "GameObject" );
					if ( payload && !payload->IsEmpty() )
					{
						GameObject go = payload->As<GameObject>();
						GameObject newGO = SceneManager::GetActiveScene()->InstantiateGameObjectFrom(go);
						Editor::Events::OnGameObjectSelected.Broadcast( newGO );
					}
				}
				return true;
			}
			case Input::KEY_DELETE:
			{
				if ( m_SelectedGameObject )
				{
					m_SelectedGameObject.Destroy();
					Editor::Events::OnGameObjectSelected.Broadcast( GameObject() );
				}
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

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) )
				ImGui::SetWindowFocus();

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

			SceneManager::GetActiveScene()->GetSceneRenderer().Render(m_FBO, *m_EditorCamera, m_EditorCamera->GetViewMatrix(), m_EditorCamera->Position);

			// Draw Debug Lines
			{
				m_FBO->Bind();
				Debug::DebugDrawer::Get()->Draw( m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix() );
				m_FBO->Unbind();
			}

			RenderSelectionOutline();

			// Draw the Editor Camera ViewPort
			ImGui::Image( (ImTextureID)m_FBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

			DragDropTarget();

			DrawManipulationGizmos( viewportBoundsMin, viewportBoundsMax );

			if ( m_IsHovered && ImGui::IsItemClicked() && !ImGuizmo::IsUsingAny() )
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

				Editor::Events::OnGameObjectSelected.Broadcast( static_cast<EntityID>( goID ) );
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
				SceneManager::GetActiveScene()->Clear();
				// Load the scene
				SceneManager::SetActiveScene( scene.get() );
			}
			break;
		}
		case EAssetType::StaticMesh:
		{
			if ( SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( assetHandle ) )
			{
				GameObject go = SceneManager::GetActiveScene()->InstantiateGameObject();
				go.AddComponent<StaticMeshComponent>().Mesh = mesh->GetHandle();

				Vector3 position = m_EditorCamera->Position + m_EditorCamera->GetForwardDirection() * 5.0f;

				go.GetTransform().SetWorldPosition( position );

				Editor::Events::OnGameObjectSelected.Broadcast( go );
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

		if ( m_SelectedGameObject.IsValid() )
		{
			// Selected Game Object
			TransformComponent& goTransform = m_SelectedGameObject.GetTransform();
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
		RenderCommand::SetDepthTest( true );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		m_GameObjectIDShader->Bind();

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();

		auto meshComponents = SceneManager::GetActiveScene()->GetECS().View<StaticMeshComponent, TransformComponent>();
		meshComponents.each( 
			[&]( auto go, StaticMeshComponent& meshComponent, TransformComponent& transform )
			{
				if ( !meshComponent.Mesh.IsValid() )
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
		if ( !m_FBO || !m_SelectedGameObject.IsValid() )
			return;

		StaticMeshComponent* meshComponent = m_SelectedGameObject.TryGetComponent<StaticMeshComponent>();
		if ( !meshComponent )
			return;

		if ( !meshComponent->Mesh.IsValid() )
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
		m_OutlineShader->SetFloat4( "u_Color", { 1.0f, 0.85f, 0.0f, 1.0f } );

		RenderCommand::SetLineWidth( 5 );
		RenderCommand::SetPolygonMode( EFaces::FrontAndBack, EPolygonMode::Line );

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();
		for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
		{
			const SubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
			if ( const SharedPtr<VertexArray>& vao = subMesh.VAO )
			{
				m_OutlineShader->SetMatrix4( "u_PVM", pvm * m_SelectedGameObject.GetWorldTransform() * subMesh.Transform );

				vao->Bind();
				RenderCommand::DrawIndexed( vao );
				vao->Unbind();
			}
		}

		m_OutlineShader->Unbind();
		m_FBO->Unbind();

		RenderCommand::SetPolygonMode( EFaces::FrontAndBack, EPolygonMode::Fill );
	}
}

#endif // IS_EDITOR
