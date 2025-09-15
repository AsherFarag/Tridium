#include "tripch.h"

#if IS_EDITOR
#include "EditorViewportPanel.h"
#include <Tridium/Debug/DebugDrawer.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/EditorUtil.h>
#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Editor/EditorStyle.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Scene/Scene.h>

#include <ImGuizmo.h>

// TEMP ?
#include <Tridium/Graphics/oldRendering/SceneRenderer.h>
#include <Tridium/oldAsset/EditorAssetManager.h>
#include "Tridium/oldAsset/AssetManager.h"
#include <Tridium/Graphics/oldRendering/RenderCommand.h>
#include <Tridium/Graphics/oldRendering/VertexArray.h>
#include <Tridium/Graphics/oldRendering/Shader.h>
#include <Tridium/Graphics/Renderer/SceneRenderer.h>

namespace Tridium {

	void SetImGuizmoColors()
	{
		ImGuizmo::GetStyle() = ImGuizmo::Style();
		EditorStyle::Pallete& pal = Editor::GetStyle().Colors;

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
		if ( 0 )
		{

			//FBOspecification.Attachments = { EFramebufferTextureFormat::RED_INT, EFramebufferTextureFormat::Depth };
			//m_IDFBO = Framebuffer::Create( FBOspecification );

			String idVert =
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


			String idFrag =
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
		if ( e.IsRepeat )
			return false;

		if ( !ImGuizmo::IsUsing() )
		{
			switch ( e.KeyCode )
			{
			case EInputKey::E:
			{
				m_GizmoState = EGizmoState::Translate;
				return true;
			}
			case EInputKey::R:
			{
				m_GizmoState = EGizmoState::Rotate;
				return true;
			}
			case EInputKey::T:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
					m_GizmoState = EGizmoState::UniversalScale;
				else
					m_GizmoState = EGizmoState::Scale;

				return true;
			}
			case EInputKey::C:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
				{
					if ( m_SelectedGameObject )
					{
						Editor::GetPayloadManager().SetPayload( "GameObject", m_SelectedGameObject );
					}
				}
				return true;
			}
			case EInputKey::V:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
				{
					EditorPayload* payload = Editor::GetPayloadManager().GetPayload( "GameObject" );
					if ( payload && !payload->IsEmpty() )
					{
						GameObject go = payload->As<GameObject>();
						GameObject newGO = SceneManager::GetActiveScene()->InstantiateGameObjectFrom(go);
						Editor::Events::OnGameObjectSelected.Broadcast( newGO );
					}
				}
				return true;
			}
			case EInputKey::Delete:
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

			const Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const auto viewportOffset = ImGui::GetWindowPos();
			const Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			const Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			// Update the viewport size
			m_ViewportSize = regionAvail;
			m_EditorCamera->SetViewportSize( regionAvail.X, regionAvail.X );

			// Draw the Editor Camera ViewPort
			//ImTextureID textureID = ( ImTextureID )( *colorTarget->NativePtrAs<uint32_t>() );
			//ImGui::Image( textureID, ImGui::GetContentRegionAvail() );

			{
				static SceneRenderer renderer( nullptr );
				renderer.SetViewportSize( ( uint32_t )m_ViewportSize.X, ( uint32_t )m_ViewportSize.Y );
				renderer.Open( *m_EditorCamera, m_EditorCamera->GetViewMatrix(), m_EditorCamera->Position );
				renderer.Close();

				ImTextureID textureID = ( ImTextureID )( renderer.GetOutputTexture().get() );
				ImGui::Image( textureID, ImGui::GetContentRegionAvail() );
			}

			//DragDropTarget();

			//DrawManipulationGizmos( viewportBoundsMin, viewportBoundsMax );
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

		const OldAssetMetaData& assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( assetHandle );

		switch ( assetMetaData.AssetType )
		{
		case EAssetTypeOld::Scene:
		{
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( assetHandle ) )
			{
				SceneManager::GetActiveScene()->Clear();
				// Load the scene
				SceneManager::SetActiveScene( scene.get() );
			}
			break;
		}
		case EAssetTypeOld::StaticMesh:
		{
			if ( SharedPtr<OldStaticMesh> mesh = AssetManager::GetAsset<OldStaticMesh>( assetHandle ) )
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
		const Matrix4 oldCamView = m_EditorCamera->GetViewMatrix();
		Matrix4 camView = oldCamView;

		// Set up ImGuizmo
		SetImGuizmoColors();
		ImGuizmo::SetOrthographic( false );
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect( viewportBoundsMin.X, viewportBoundsMin.X,
			viewportBoundsMax.X - viewportBoundsMin.X, viewportBoundsMax.X - viewportBoundsMin.X );

		if ( m_SelectedGameObject.IsValid() )
		{
			// Selected Game Object
			TransformComponent& goTransform = m_SelectedGameObject.GetTransform();
			Matrix4 goWorldTransform = goTransform.GetWorldTransform();

			bool shouldSnap = Input::IsKeyPressed( EInputKey::LeftControl );

			Vector3 snapVals( 0.25f );
			if ( m_GizmoState == EGizmoState::Rotate )
				snapVals = Vector3( 45.0f );

			// Convert the Gizmo operation to ImGuizmo operation
			ImGuizmo::OPERATION gizmoOperation;
			switch ( m_GizmoState )
			{
				case EGizmoState::Translate: gizmoOperation = ImGuizmo::TRANSLATE; break;
				case EGizmoState::Rotate: gizmoOperation = ImGuizmo::ROTATE; break;
				case EGizmoState::Scale: gizmoOperation = ImGuizmo::SCALE; break;
				case EGizmoState::UniversalScale: gizmoOperation = ImGuizmo::SCALEU; break;
				default: gizmoOperation = ( ImGuizmo::OPERATION )0; break;
			}

			// Create a Manipulation Gizmo that allows the user to easily modify the Game-Object's transform
			ImGuizmo::Manipulate(
				&camView[0][0], &camProjection[0][0],
				gizmoOperation, ImGuizmo::LOCAL,
				&goWorldTransform[0][0], nullptr,
				shouldSnap ? &snapVals[0] : nullptr 
			);

			ImGuizmo::ViewManipulate( &camView[0][0], 8.0f, { viewportBoundsMax.X - 75, viewportBoundsMin.X }, { 75, 75 }, 0x10101010 );

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
				&identity[0][0], 8.0f, ImVec2( viewportBoundsMax.X - 75, viewportBoundsMin.X ), ImVec2( 75, 75 ), 0x10101010 );
		}

		// Update the Editor Camera's View Matrix
		if ( oldCamView != camView )
			m_EditorCamera->SetViewMatrix( camView );
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

				SharedPtr<OldStaticMesh> mesh = AssetManager::GetAsset<OldStaticMesh>( meshComponent.Mesh );
				if ( !mesh )
					return;

				SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
				if ( !meshSource )
					return;

				m_GameObjectIDShader->SetInt( "uID", Cast<uint32_t>( go ) );
				for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
				{
					const OldSubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
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

	void EditorViewportPanel::RenderSelectionOutline()
	{
	#if 0
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

	#endif
	}
}

#endif // IS_EDITOR
