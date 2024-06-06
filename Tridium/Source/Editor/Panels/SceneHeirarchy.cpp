#include "tripch.h"

#ifdef IS_EDITOR

#include "SceneHeirarchy.h"
#include "imgui.h"

#include <Editor/EditorLayer.h>

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>

namespace ImGui {
	static void DrawVec3Control( const std::string& label, Vector3& values, float speed, const char* format = "%.2f" )
	{
		float itemWidth = ImGui::GetContentRegionAvail().x / 3.f - 30;

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

		ImGui::BeginGroup();
		ImGui::PushID( label.c_str() );
		{
			ImGui::PushItemWidth( itemWidth );

			// x
			ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.9f, 0.5f, 0.5f, 0.9f ) );
			ImGui::PushID( 0 );
			ImGui::DragFloat( "", &values.x, speed, 0, 0, format );
			ImGui::PopID();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			// y
			ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.5f, 0.9f, 0.5f, 0.9f ) );
			ImGui::PushID( 1 );
			ImGui::DragFloat( "", &values.y, speed, 0, 0, format );
			ImGui::PopID();
			ImGui::PopStyleColor();

			ImGui::SameLine();

			// z
			ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.5f, 0.5f, 0.9f, 0.9f ) );
			ImGui::PushID( 2 );
			ImGui::DragFloat( "", &values.z, speed, 0, 0, format );
			ImGui::PopID();
			ImGui::PopStyleColor();

			ImGui::PopItemWidth();
		}
		ImGui::PopID();
		ImGui::EndGroup();

		ImGui::PopStyleVar();

		ImGui::SameLine();
		ImGui::Text( label.c_str() );
	}
}

namespace Tridium::Editor {

	void SceneHeirarchy::OnEvent( Event& e )
	{
		if ( m_IsFocused )
		{
			EventDispatcher dispatcher( e );
			dispatcher.Dispatch<KeyPressedEvent>( TE_BIND_EVENT_FN( SceneHeirarchy::OnKeyPressed, std::placeholders::_1 ) );
		}
	}

	void SceneHeirarchy::OnImGuiDraw()
	{
		DrawSceneHeirarchy();
		DrawInspector();
	}

	void SceneHeirarchy::SetSelectedGameObject( GameObject gameObject )
	{
		m_SelectedGameObject = gameObject;
		// Since a game object was selected, bring the Inspector into focus.
		ImGui::SetWindowFocus( "Inspector" );
		ImGui::SetWindowFocus( "Scene Heirarchy" );
	}
	

	bool SceneHeirarchy::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool shift = Input::IsKeyPressed(Input::KEY_LEFT_SHIFT);
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
							auto& editorCam = EditorLayer::Get().GetEditorCamera();
							editorCam.Position = goTransform.Position - (editorCam.GetForwardDirection() * 5.f);
							return true;
						}
					}
				}
				break;
			}
		}

		return false;
	}

	void SceneHeirarchy::DrawSceneHeirarchy()
	{
		m_Context = Application::GetScene();
		if ( m_Context == nullptr )
			return;

		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 1, 1 ) );

		if ( !ImGui::Begin("Scene Heirarchy") )
		{
			ImGui::PopStyleVar();

			m_IsFocused = false;
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}

		ImGui::PopStyleVar();

		m_IsFocused = ImGui::IsWindowFocused() || ImGui::IsItemFocused();

		auto gameObjects = m_Context->GetRegistry().view<TagComponent>();
		ImGui::Text( "Game Objects: %i", gameObjects.size() );

		ImGui::SameLine();

		#pragma region Add-GameObject Button

		// Align the button to the right
		float addGameObjectButtonWidth = ImGui::CalcTextSize( "+" ).x + ImGui::GetStyle().FramePadding.x * 2.f;
		ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - addGameObjectButtonWidth - 5 );

		if ( ImGui::Button( "+" ) )
			ImGui::OpenPopup( "AddGameObject" );

		if ( ImGui::BeginPopup("AddGameObject" ) )
		{
			if ( ImGui::MenuItem( "Game Object" ) )
			{
				auto go = m_Context->InstantiateGameObject();
				SetSelectedGameObject(go);
			}

			ImGui::Separator();

			if ( ImGui::MenuItem( "Cube" ) )
			{
				auto go = m_Context->InstantiateGameObject( "Cube" );
				go.AddComponent<MeshComponent>();
				SetSelectedGameObject( go );
			}

			if ( ImGui::MenuItem( "Sprite" ) )
			{
				auto go = m_Context->InstantiateGameObject( "Sprite" );
				go.AddComponent<SpriteComponent>();
				SetSelectedGameObject( go );
			}

			ImGui::Separator();

			ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Text, { 0.8, 0.1, 0.1, 0.8 } );
			if ( ImGui::MenuItem( " - Remove All - " ) ) m_Context->Clear();
			ImGui::PopStyleColor();

			ImGui::EndMenu();
		}

		#pragma endregion

		#pragma region GamesObjects List

		ImGui::Separator();

		// Create a list box of all game objects in the scene
		if ( ImGui::BeginListBox( "Game Objects", ImGui::GetContentRegionAvail() ) )
		{
			for ( int i = 0; i < gameObjects.size(); ++i )
			{
				auto go = gameObjects[ i ];
				bool selected = go == m_SelectedGameObject;
				auto& goTag = gameObjects.get<TagComponent>( go ).Tag;

				// We must append the Tag with ##id so ImGui can have a unique identifier for this selectable.
				if ( ImGui::Selectable( ( goTag + "##" + std::to_string( (uint32_t)go ) ).c_str(), selected ) )
				{
					SetSelectedGameObject( go );
				}
			}
			ImGui::EndListBox();
		}

		#pragma endregion

		ImGui::End();
	}

	void SceneHeirarchy::DrawInspector()
	{
		// Early out if there is no GameObject to inspect
		if ( !ImGui::Begin( "Inspector" ) || !m_SelectedGameObject.IsValid() )
		{
			ImGui::End();
			return;
		}

		// - Draw Components -
		InspectGameObject( m_SelectedGameObject );

		ImGui::Separator();

		#pragma region - Add Component -

		ImGuiStyle& style = ImGui::GetStyle();
		// Center the button by calculating its size
		float size = ImGui::CalcTextSize( "Add Component" ).x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;
		float off = ( avail - size ) * 0.5f;
		if ( off > 0.0f )
			ImGui::SetCursorPosX( ImGui::GetCursorPosX() + off );

		if ( ImGui::Button( "Add Component" ) )
			ImGui::OpenPopup( "AddComponent" );

		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "ContentBrowserItem" ) )
			{
				AddComponentToSelectedGameObject<LuaScriptComponent>( Script::Create( static_cast<const char*>( payload->Data ) ) );
			}
			ImGui::EndDragDropTarget();
		}

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			if ( ImGui::MenuItem( "Transform" ) )		  AddComponentToSelectedGameObject<TransformComponent>();
			if ( ImGui::MenuItem( "Mesh" ) )			  AddComponentToSelectedGameObject<MeshComponent>();
			if ( ImGui::MenuItem( "Camera" ) )			  AddComponentToSelectedGameObject<CameraComponent>();
			if ( ImGui::MenuItem( "Camera Controller" ) ) AddComponentToSelectedGameObject<CameraControllerComponent>();
			if ( ImGui::MenuItem( "Lua Script" ) )		  AddComponentToSelectedGameObject<LuaScriptComponent>(ScriptLibrary::GetScript("Test"));
			if ( ImGui::MenuItem( "Sprite" ) )		      AddComponentToSelectedGameObject<SpriteComponent>();

			ImGui::EndMenu();
		}

		#pragma endregion

		ImGui::End();
	}

#pragma region Drawing Components

	TODO( "This is a temporary solution that requires a reflection system!" );

	template<typename T, typename UIFunction>
	static void DrawComponent( const std::string& name, GameObject gameObject, UIFunction uiFunction )
	{

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
		if ( gameObject.HasComponent<T>() )
		{
			auto& component = gameObject.GetComponent<T>();

			ImGui::Separator();
			bool open = ImGui::TreeNodeEx( ( void* )typeid( T ).hash_code(), treeNodeFlags, name.c_str() );

			ImGui::SameLine();
			
			// Align the button to the right
			float addGameObjectButtonWidth = ImGui::CalcTextSize( "+" ).x + ImGui::GetStyle().FramePadding.x * 2.f;
			ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - addGameObjectButtonWidth - 5 );
			if ( ImGui::Button( "+" ) )
			{
				ImGui::OpenPopup( "ComponentSettings" );
			}

			bool removeComponent = false;
			if ( ImGui::BeginPopup( "ComponentSettings" ) )
			{
				if ( ImGui::MenuItem( "Remove component" ) )
					removeComponent = true;

				ImGui::EndPopup();
			}

			if ( open )
			{
				uiFunction( component );
				ImGui::TreePop();
			}

			if ( removeComponent )
			{
				gameObject.RemoveComponent<T>();
			}
		}
	}

	void SceneHeirarchy::InspectGameObject( GameObject gameObject )
	{
		auto& tag = m_SelectedGameObject.GetComponent<TagComponent>();
		ImGui::InputText( "Tag", (char*)tag.Tag.c_str(), tag.MaxSize() );

		ImGui::SameLine();

		ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Button, ImVec4( 1, 0.2, 0.2, 1 ) );
		if ( ImGui::Button( "Destroy" ) ) { gameObject.Destroy(); }
		ImGui::PopStyleColor();

		DrawComponent<TransformComponent>( "Transform", gameObject, []( auto& component )
			{
				ImGui::DrawVec3Control( "Position", component.Position, 0.01f );
				Vector3 rotation = glm::degrees( component.Rotation );
				ImGui::DrawVec3Control( "Rotation", rotation, 1.f );
				component.Rotation = glm::radians( rotation );
				ImGui::DrawVec3Control( "Scale", component.Scale, 0.01f );
			} );

		DrawComponent<MeshComponent>( "Mesh", gameObject, []( auto& component )
			{
			} );

		DrawComponent<CameraComponent>( "Camera", gameObject, []( auto& component )
			{
				int currentItem = (int)component.SceneCamera.GetProjectionType();
				Camera::ProjectionType projType = component.SceneCamera.GetProjectionType();
				const char* const items[2] = {"Perspective", "Orthographic"};

				auto& camera = component.SceneCamera;

				ImGui::Combo( "Perspective", &currentItem, items, 2, -1 );
				camera.SetProjectionType( (Camera::ProjectionType)currentItem );

				ImGui::Text( "%f : Aspect Ratio", camera.GetAspectRatio() );

				if ( camera.GetProjectionType() == Camera::ProjectionType::Perspective )
				{
					float fov = camera.GetPerspectiveFOV();
					if ( ImGui::DragFloat( "FOV", &fov, 1.f, 0, 0, "%.0f" ) )
						camera.SetPerspectiveFOV( fov );

					float nearClip = camera.GetPerspectiveNearClip();
					if ( ImGui::DragFloat( "Near Clip", &nearClip, 1.f, 0, 0, "%.1f" ) )
						camera.SetPerspectiveNearClip( nearClip );

					float farClip = camera.GetPerspectiveFarClip();
					if ( ImGui::DragFloat( "Far Clip", &farClip, 1.f, 0, 0, "%.1f" ) )
						camera.SetPerspectiveFarClip( farClip );

				}
				// Orthographic
				else
				{
					float size = camera.GetOrthographicSize();
					if ( ImGui::DragFloat( "Size", &size, 1.f, 0, 0, "%.1f" ) )
						camera.SetOrthographicSize( size );

					float nearClip = camera.GetOrthographicNearClip();
					if ( ImGui::DragFloat( "Near Clip", &nearClip, 1.f, 0, 0, "%.1f" ) )
						camera.SetOrthographicNearClip( nearClip );

					float farClip = camera.GetOrthographicFarClip();
					if ( ImGui::DragFloat( "Far Clip", &farClip, 1.f, 0, 0, "%.1f" ) )
						camera.SetOrthographicFarClip( farClip );
				}


			} );

		DrawComponent<CameraControllerComponent>( "Camera Controller", gameObject, []( auto& component )
			{
				ImGui::DragFloat( "Speed", &component.Speed, 0.1f );
				ImGui::DragFloat( "Look Sensitivity", &component.LookSensitivity, 0.1f );
			} );

		DrawComponent<LuaScriptComponent>( "Lua Script Component", gameObject, []( auto& component )
			{
				ImGui::Text( "Script: " );
				ImGui::SameLine();
				if ( component.GetScript() )
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, component.GetScript()->GetFilePath().string().c_str() );
				else
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, "Empty");

				if ( ImGui::BeginDragDropTarget() )
				{
					if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "ContentBrowserItem" ) )
					{
						component.SetScript( Script::Create( static_cast<const char*>( payload->Data ) ) );
					}
					ImGui::EndDragDropTarget();
				}
			} );

		DrawComponent<SpriteComponent>( "Sprite Component", gameObject, []( auto& component )
			{
				ImGui::Text( "Sprite: " );
				ImGui::SameLine();
				if ( component.GetTexture() )
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, component.GetTexture()->GetPath().c_str() );
				else
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, "Empty" );

				if ( ImGui::BeginDragDropTarget() )
				{
					if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "ContentBrowserItem" ) )
					{
						component.SetTexture( Texture2D::Create( static_cast<const char*>( payload->Data ) ) );
					}
					ImGui::EndDragDropTarget();
				}

				if ( component.GetTexture() && ImGui::TreeNode( "Preview:" ) )
				{
					Vector2 textureSize( component.GetTexture()->GetWidth(), component.GetTexture()->GetHeight() );
					ImVec2 previewSize;
					ImVec2 regionAvail = ImGui::GetContentRegionAvail();
					// Centres the image so that there will never be less padding on the right side than the left.
					regionAvail.x -= ImGui::GetContentRegionMax().x - regionAvail.x;

					if ( textureSize.x > textureSize.y )
					{
						float yScale = textureSize.y / textureSize.x;
						previewSize.x = regionAvail.x;
						previewSize.y = previewSize.x * yScale;
					}
					else
					{
						float xScale = textureSize.x / textureSize.y;
						previewSize.y = regionAvail.y;
						previewSize.x = previewSize.y * xScale;
					}

					ImGui::Image( (ImTextureID)component.GetTexture()->GetRendererID(),
						previewSize,
						{ 0, 1 }, { 1, 0 } );

					ImGui::TreePop();
				}
			} );
	}

#pragma endregion

}

#endif // IS_EDITOR