#include "tripch.h"

#if IS_EDITOR

#include "SceneHeirarchyPanel.h"
#include "imgui.h"

#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Editor/EditorStyle.h>

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Graphics/Rendering/Texture.h>

namespace Tridium {

	SceneHeirarchyPanel::SceneHeirarchyPanel()
		: Panel( "Scene Heirarchy" )
	{
		m_OnGameObjectSelectedHandle = Editor::Events::OnGameObjectSelected.Add<&SceneHeirarchyPanel::SetSelectedGameObject>( this );
	}

	SceneHeirarchyPanel::~SceneHeirarchyPanel()
	{
	}

	void SceneHeirarchyPanel::OnImGuiDraw()
	{
		DrawSceneHeirarchy();
	}

	void SceneHeirarchyPanel::SetSelectedGameObject( GameObject gameObject )
	{
		m_SelectedGameObject = gameObject;
	}


	bool SceneHeirarchyPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool shift = Input::IsKeyPressed( EInputKey::LeftShift );
		bool control = Input::IsKeyPressed( EInputKey::LeftControl );
		bool alt = Input::IsKeyPressed( EInputKey::LeftAlt );

		switch ( e.GetKeyCode() )
		{
		case EInputKey::Delete:
		{
			// Delete
			// Destroys the selected game object
			if ( m_SelectedGameObject.IsValid() )
			{
				m_SelectedGameObject.Destroy();
				Editor::Events::OnGameObjectSelected.Broadcast( GameObject{} );
				return true;
			}
			break;
		}
		case EInputKey::F:
		{
			// Shift + F
			// Make Editor Camera find the selected game object
			if ( shift )
			{
				if ( m_SelectedGameObject.IsValid() )
				{
					if ( m_SelectedGameObject.HasComponent<TransformComponent>() )
					{
						auto& goTransform = m_SelectedGameObject.GetComponent<TransformComponent>();
						auto editorCam = Editor::GetEditorLayer()->GetEditorCamera();
						editorCam->LerpTo( goTransform.Position - ( editorCam->GetForwardDirection() * 5.f ) );
						return true;
					}
				}
			}
			break;
		}
		case EInputKey::C:
		{
			// Control + C
			// Copy the selected game object
			if ( control )
			{
				if ( m_SelectedGameObject.IsValid() )
				{
					Editor::GetPayloadManager().SetPayload( "GameObject", m_SelectedGameObject );
					return true;
				}
			}
			break;
		}
		case EInputKey::V:
		{
			// Control + V
			// Paste the copied game object
			if ( control )
			{
				if ( Editor::GetPayloadManager().HasPayload() )
				{
					EditorPayload* payload = Editor::GetPayloadManager().GetPayload( "GameObject" );
					if ( payload && !payload->IsEmpty() )
					{
						GameObject copiedGO = payload->As<GameObject>();
						if ( copiedGO.IsValid() && SceneManager::GetActiveScene() )
						{
							GameObject newGO = SceneManager::GetActiveScene()->InstantiateGameObjectFrom(copiedGO);
							Editor::Events::OnGameObjectSelected.Broadcast( newGO );
							return true;
						}
					}
				}
			}
			break;
		}
		}

		return false;
	}

	void SceneHeirarchyPanel::DrawSceneHeirarchy()
	{
		Scene* scene = SceneManager::GetActiveScene();
		if ( !scene )
			return;

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 1, 1 ) );
		//ImGui::ScopedStyleCol winBg( ImGuiCol_::ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg) );
		ImGui::FunctionScope endWindow( +[]() { ImGui::End(); } );

		if ( !ImGui::Begin( ( TE_ICON_MOUNTAIN_SUN " " + scene->GetName() + "###Scene Heirarchy" ).c_str() ) )
		{
			m_IsHovered = false;
			m_IsFocused = false;
			return;
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused() || ImGui::IsItemFocused();


		// Draw list header (Search Bar and Add Game Object button).
		{
			// Add some padding to the top of the window
			ImGui::Dummy( ImVec2( 0.0f, 0.0f ) );

			float addGameObjectButtonWidth = ImGui::CalcTextSize( TE_ICON_PLUS ).x + ImGui::GetStyle().FramePadding.x * 2.0f;

			// Search Bar
			{
				ImGui::Text( " " TE_ICON_MAGNIFYING_GLASS);
				ImGui::SameLine();
				ImGui::ScopedStyleCol border( ImGuiCol_Border, ImVec4( 1.0f, 1.0f, 1.0f, 0.15f ) );
				ImGui::ScopedStyleVar borderSize( ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 1.0f );
				ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - addGameObjectButtonWidth - 5.0f );
				ImGui::InputText( "##Search", &m_SearchBuffer );
			}

			// Add GameObject Button
			{
				ImGui::SameLine( ImGui::GetContentRegionAvail().x - addGameObjectButtonWidth );
				ImGui::ScopedStyleCol buttonBg( ImGuiCol_Button, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f ) );
				if ( ImGui::Button( TE_ICON_PLUS ) )
					OpenAddPopUp();
			}

			DrawAddPopUp();
		}

		ImGui::Separator();

		if ( ImGui::BeginChild( "SceneHeirarchy", { 0, 0 }, ImGuiChildFlags_FrameStyle ) )
		{
			auto gameObjects = scene->GetECS().View<TagComponent>();
			ImGui::ScopedStyleVar itemSpacing( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 0, 1 ) );
			ImGuiTextFilter filter( m_SearchBuffer.c_str() );
			for ( int i = 0; i < gameObjects.size(); ++i )
			{
				GameObject go = gameObjects.begin()[i];
				// We are only drawing root objects in this loop
				if ( go.GetParent().IsValid() )
					continue;

				if ( !m_SearchBuffer.empty() )
				{
					TODO( "Make this search filter include children objects!" );
					if ( !filter.PassFilter( go.GetTag().c_str() ) )
						continue;
				}

				DrawSceneNode( go );
			}
		}
		ImGui::EndChild();
	}

	void SceneHeirarchyPanel::OpenAddPopUp()
	{
		ImGui::OpenPopup( "Add##SceneHierachy" );
	}

	void SceneHeirarchyPanel::DrawAddPopUp( GameObject gameObject )
	{
		GameObject newGO = {};

		Scene* scene = SceneManager::GetActiveScene();
		if ( !scene )
			return;

		if ( !ImGui::BeginPopup( "Add##SceneHierachy" ) )
			return;

		if ( ( gameObject.IsValid() ? ImGui::BeginMenu("Add Child") : ImGui::BeginMenu( "Add GameObject" ) ) )
		{
			if ( ImGui::MenuItem( TE_ICON_CUBE " Empty" ) )
			{
				newGO = scene->InstantiateGameObject();
				SetSelectedGameObject( newGO );
			}

			ImGui::Separator();

			if ( ImGui::MenuItem( TE_ICON_CUBE " Static Mesh" ) )
			{
				newGO = scene->InstantiateGameObject( "Static Mesh" );
				newGO.AddComponent<StaticMeshComponent>();
				SetSelectedGameObject( newGO );
			}

			if ( ImGui::BeginMenu( TE_ICON_SHAPES " Primatives") )
			{
				if ( ImGui::MenuItem( "Cube" ) )
				{
					newGO = scene->InstantiateGameObject( "Cube" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultCube();
					newGO.AddComponent<BoxColliderComponent>();
					newGO.AddComponent<RigidBodyComponent>();
					SetSelectedGameObject( newGO );
				}

				if ( ImGui::MenuItem( "Sphere" ) )
				{
					newGO = scene->InstantiateGameObject( "Sphere" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultSphere();
					newGO.AddComponent<SphereColliderComponent>();
					newGO.AddComponent<RigidBodyComponent>();
					SetSelectedGameObject( newGO );
				}

				if ( ImGui::MenuItem( "Cylinder" ) )
				{
					newGO = scene->InstantiateGameObject( "Cylinder" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultCylinder();
					newGO.AddComponent<CylinderColliderComponent>();
					newGO.AddComponent<RigidBodyComponent>();
					SetSelectedGameObject( newGO );
				}

				if ( ImGui::MenuItem( "Capsule" ) )
				{
					newGO = scene->InstantiateGameObject( "Capsule" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultCapsule();
					newGO.AddComponent<CapsuleColliderComponent>();
					newGO.AddComponent<RigidBodyComponent>();
					SetSelectedGameObject( newGO );
				}

				if ( ImGui::MenuItem( "Cone" ) )
				{
					newGO = scene->InstantiateGameObject( "Cone" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultCone();
					SetSelectedGameObject( newGO );
				}

				if ( ImGui::MenuItem( "Torus" ) )
				{
					newGO = scene->InstantiateGameObject( "Torus" );
					newGO.AddComponent<StaticMeshComponent>().Mesh = MeshFactory::GetDefaultTorus();
					SetSelectedGameObject( newGO );
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if ( ImGui::MenuItem( TE_ICON_LIGHTBULB " Point Light" ) )
			{
				newGO = scene->InstantiateGameObject( "Point Light" );
				newGO.AddComponent<PointLightComponent>();
				SetSelectedGameObject( newGO );
			}

			if ( ImGui::MenuItem( TE_ICON_LIGHTBULB " Spot Light" ) )
			{
				newGO = scene->InstantiateGameObject( "Spot Light" );
				newGO.AddComponent<SpotLightComponent>();
				SetSelectedGameObject( newGO );
			}

			if ( ImGui::MenuItem( TE_ICON_LIGHTBULB " Directional Light" ) )
			{
				newGO = scene->InstantiateGameObject( "Directional Light" );
				newGO.AddComponent<DirectionalLightComponent>();
				SetSelectedGameObject( newGO );
			}

			ImGui::Separator();

			if ( ImGui::MenuItem( TE_ICON_IMAGE " Sprite" ) )
			{
				newGO = scene->InstantiateGameObject( "Sprite" );
				newGO.AddComponent<SpriteComponent>();
				SetSelectedGameObject( newGO );
			}

			ImGui::EndMenu();
		}

		if ( !gameObject.IsValid() )
		{
			ImGui::Separator();

			ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Text, ImVec4( Editor::GetPallete().Red ) );
			if ( ImGui::MenuItem( " - Remove All - " ) ) scene->Clear();
			ImGui::PopStyleColor();
		}

		ImGui::EndPopup();

		if ( newGO.IsValid() && gameObject.IsValid() )
		{
			gameObject.AttachChild( newGO );
		}
	}

	void SceneHeirarchyPanel::DrawSceneNode( GameObject go )
	{
		if ( !go.IsValid() || !go.HasComponent<TagComponent>() )
		{
			LOG( LogCategory::Editor, Warn, "Attempting to draw an invalid Game Object node!" );
			return;
		}

		const String& label = go.GetTag();
		bool selected = go == m_SelectedGameObject;
		bool hasChildren = go.GetChildren().size() > 0;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if ( selected )
			flags |= ImGuiTreeNodeFlags_Selected;
		if ( !hasChildren ) 
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool drawChildren = ImGui::TreeNodeEx( (void*)(uint64_t)(uint32_t)go, flags, ( TE_ICON_CUBE " " + label ).c_str());

		if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			OpenAddPopUp();

		DrawAddPopUp( go );

		// Drag-Drop Payload handling
		bool isDragging = false;
		if ( ImGui::BeginDragDropSource() )
		{
			isDragging = true;
			ImGui::SetDragDropPayload( TE_PAYLOAD_GAME_OBJECT, (void*)&go, sizeof( go ), ImGuiCond_Once );
			ImGui::Text( label.c_str() );

			ImGui::EndDragDropSource();
		}
		else if ( ImGui::BeginDragDropTarget() )
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_GAME_OBJECT );
			if ( payload )
			{
				GameObject payloadGO = *(GameObject*)payload->Data;
				if ( payloadGO != go )
				{
					if ( payloadGO.HasParent() && payloadGO.GetParent() == go )
						go.DetachChild( payloadGO );
					else
						go.AttachChild( payloadGO );
				}
			}

			ImGui::EndDragDropTarget();
		}

		// Handle mouse release for selection, ensuring it's not part of a drag-and-drop
		if ( !isDragging && ImGui::IsItemHovered() && ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )
		{
			Editor::Events::OnGameObjectSelected.Broadcast( go );
		}

		if ( drawChildren )
		{
			std::vector<GameObject>& children = go.GetChildren();
			for ( auto& child : children )
			{
				DrawSceneNode( child );
			}

			ImGui::TreePop();
		}
	}

}

#endif // IS_EDITOR