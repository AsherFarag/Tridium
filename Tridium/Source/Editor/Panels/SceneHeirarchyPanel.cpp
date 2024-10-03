#include "tripch.h"

#ifdef IS_EDITOR

#include "SceneHeirarchyPanel.h"
#include "imgui.h"

#include <Editor/Editor.h>
#include <Editor/EditorCamera.h>

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor {

	SceneHeirarchyPanel::SceneHeirarchyPanel()
		: Panel( "Scene Heirarchy" )
	{
	}

	void SceneHeirarchyPanel::OnImGuiDraw()
	{
		DrawSceneHeirarchy();
	}

	void SceneHeirarchyPanel::SetSelectedGameObject( GameObject gameObject )
	{
		m_SelectedGameObject = gameObject;

		if ( !m_Inspector )
			m_Inspector = m_Owner->PushPanel<InspectorPanel>();

		if ( m_Inspector->InspectedGameObject == gameObject )
			return;

		m_Inspector->InspectedGameObject = m_SelectedGameObject;

		// Since a game object was selected, bring the Inspector into focus.
		//m_Inspector->Focus();
	}


	bool SceneHeirarchyPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool shift = Input::IsKeyPressed( Input::KEY_LEFT_SHIFT );
		bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );
		bool alt = Input::IsKeyPressed( Input::KEY_LEFT_ALT );

		switch ( e.GetKeyCode() )
		{
		case Input::KEY_DELETE:
		{
			// Delete
			// Destroys the selected game object
			if ( m_SelectedGameObject.IsValid() )
			{
				m_SelectedGameObject.Destroy();
				return true;
			}
			break;
		}
		case Input::KEY_F:
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
						auto editorCam = GetEditorLayer()->GetEditorCamera();
						editorCam->LerpTo( goTransform.Position - ( editorCam->GetForwardDirection() * 5.f ) );
						return true;
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
		m_Context = Application::GetScene();
		if ( m_Context == nullptr )
			return;

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 1, 1 ) );

		ImGui::FunctionScope endWindow( +[]() { ImGui::End(); } );
		if ( !ImGui::Begin( "Scene Heirarchy" ) )
		{
			m_IsHovered = false;
			m_IsFocused = false;
			return;
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused() || ImGui::IsItemFocused();

		auto gameObjects = m_Context->GetRegistry().view<TagComponent>();
		ImGui::Text( "Game Objects: %i", gameObjects.size() );

		ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("+").x - 10);

		if ( ImGui::SmallButton( "+" ) )
			OpenAddPopUp();

		DrawAddPopUp();

		ImGui::Separator();

		// Draw Heirarchy List
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed;
		flags |= gameObjects.size() == 0 ? ImGuiTreeNodeFlags_Leaf : 0;
		ImGui::PushFont( ImGui::GetBoldFont() );
		if ( ImGui::TreeNodeEx( m_Context->GetName().c_str(), flags ) )
		{
			ImGui::PopFont();
			for ( int i = 0; i < gameObjects.size(); ++i )
			{
				GameObject go = gameObjects[i];
				// We are only drawing root objects in this loop
				if ( go.GetParent().IsValid() )
					continue;

				DrawSceneNode( go );
			}

			ImGui::TreePop();
		}
		else
		{
			ImGui::PopFont();
		}
	}

	void SceneHeirarchyPanel::OpenAddPopUp()
	{
		ImGui::OpenPopup( "Add##SceneHierachy" );
	}

	void SceneHeirarchyPanel::DrawAddPopUp( GameObject gameObject )
	{
		GameObject newGO = {};

		if ( !ImGui::BeginPopup( "Add##SceneHierachy" ) )
			return;

		if ( ( gameObject.IsValid() ? ImGui::BeginMenu("Add Child") : ImGui::BeginMenu( "Add GameObject" ) ) )
		{
			if ( ImGui::MenuItem( "Empty" ) )
			{
				newGO = m_Context->InstantiateGameObject();
				SetSelectedGameObject( newGO );
			}

			ImGui::Separator();

			if ( ImGui::BeginMenu("Primatives") )
			{
				//if ( ImGui::MenuItem( "Cube" ) )
				//{
				//	newGO = m_Context->InstantiateGameObject( "Cube" );
				//	newGO.AddComponent<MeshComponent>().SetMesh( MeshLibrary::GetCube() );
				//	SetSelectedGameObject( newGO );
				//}

				//if ( ImGui::MenuItem( "Sphere" ) )
				//{
				//	newGO = m_Context->InstantiateGameObject( "Sphere" );
				//	newGO.AddComponent<MeshComponent>().SetMesh( MeshLibrary::GetSphere() );
				//	SetSelectedGameObject( newGO );
				//}

				ImGui::EndMenu();
			}

			if ( ImGui::MenuItem( "Sprite" ) )
			{
				newGO = m_Context->InstantiateGameObject( "Sprite" );
				newGO.AddComponent<SpriteComponent>();
				SetSelectedGameObject( newGO );
			}

			ImGui::EndMenu();
		}

		if ( !gameObject.IsValid() )
		{
			ImGui::Separator();

			ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Text, { 0.8, 0.1, 0.1, 0.8 } );
			if ( ImGui::MenuItem( " - Remove All - " ) ) m_Context->Clear();
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
		if ( !go.IsValid() )
		{
			TE_CORE_WARN( "Attempting to draw an invalid Game Object node!" );
			return;
		}

		std::string label = go.GetTag();
		bool selected = go == m_SelectedGameObject;
		bool hasChildren = go.GetChildren().size() > 0;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if ( selected )
			flags |= ImGuiTreeNodeFlags_Selected;
		if ( !hasChildren ) 
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool drawChildren = ImGui::TreeNodeEx( (void*)(uint64_t)(uint32_t)go, flags, label.c_str() );

		if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			OpenAddPopUp();

		DrawAddPopUp( go );

		// Drag-Drop Payload handling
		if ( ImGui::BeginDragDropSource() )
		{
			ImGui::SetDragDropPayload( "GameObject", (void*)&go, sizeof( go ), ImGuiCond_Once );
			ImGui::Text( label.c_str() );

			ImGui::EndDragDropSource();
		}
		else if ( ImGui::BeginDragDropTarget() )
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "GameObject" );
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

		if ( ImGui::IsItemClicked() )
		{
			SetSelectedGameObject( go );
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