#include "tripch.h"

#ifdef IS_EDITOR

#include "SceneHeirarchy.h"
#include "imgui.h"

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>

namespace Tridium::Editor {

	namespace Utils {
		static void DrawVec3Control( const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f )
		{
			float itemWidth = ImGui::GetContentRegionAvail().x / 3.f - 30 ;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( label.c_str() );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.9f, 0.5f, 0.5f, 0.9f ) );
				ImGui::PushID( 0 );
				ImGui::DragFloat( "", &values.x );
				ImGui::PopID();
				ImGui::PopStyleColor();

				ImGui::SameLine();

				// y
				//ImGui::PushItemWidth( itemWidth );
				ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.5f, 0.9f, 0.5f, 0.9f ) );
				ImGui::PushID( 1 );
				ImGui::DragFloat( "", &values.y );
				ImGui::PopID();
				ImGui::PopStyleColor();

				ImGui::SameLine();

				// z
				ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.5f, 0.5f, 0.9f, 0.9f ) );
				ImGui::PushID( 2 );
				ImGui::DragFloat( "", &values.z );
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

	SceneHeirarchy::SceneHeirarchy()
	{
		m_Context = Application::GetScene();
	}

	void SceneHeirarchy::OnImGuiDraw()
	{
		DrawSceneHeirarchy();
		DrawInspector();
	}

	void SceneHeirarchy::DrawSceneHeirarchy()
	{
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 1, 1 ) );

		if ( ImGui::Begin( "Scene Heirarchy" ) )
		{
			ImGui::PopStyleVar();

			auto gameObjects = Application::GetScene()->GetRegistry().view<TagComponent>();
			ImGui::Text( "Game Objects: %i", gameObjects.size() );

			ImGui::SameLine();

			#pragma region Add-GameObject Button

			// Align the button to the right
			float addGameObjectButtonWidth = ImGui::CalcTextSize( "+" ).x + ImGui::GetStyle().FramePadding.x * 2.f;
			ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - addGameObjectButtonWidth - 5 );
			if ( ImGui::Button( "+" ) )
				Application::GetScene()->InstantiateGameObject();

			#pragma endregion

			#pragma region GamesObjects List

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
						SetSelectedGameObject( go );
				}
				ImGui::EndListBox();
			}

			#pragma endregion

			ImGui::End();
		}
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
		DrawComponents( m_SelectedGameObject );

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

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			if ( ImGui::MenuItem( "Transform" ) ) AddComponentToSelectedGameObject<TransformComponent>();
			if ( ImGui::MenuItem( "Camera" ) ) AddComponentToSelectedGameObject<CameraComponent>();
			if ( ImGui::MenuItem( "Mesh" ) ) AddComponentToSelectedGameObject<MeshComponent>();

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
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if ( gameObject.HasComponent<T>() )
		{
			auto& component = gameObject.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 1, 1 } );
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx( ( void* )typeid( T ).hash_code(), treeNodeFlags, name.c_str() );
			ImGui::PopStyleVar();
			ImGui::SameLine();

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

	void SceneHeirarchy::DrawComponents( GameObject gameObject )
	{
		auto& tag = m_SelectedGameObject.GetComponent<TagComponent>();
		ImGui::InputText( "Tag", (char*)tag.Tag.c_str(), tag.MaxSize() );

		DrawComponent<TransformComponent>( "Transform", gameObject, []( auto& component )
			{
				Utils::DrawVec3Control( "Position", component.Position );
				Vector3 rotation = glm::degrees( component.Rotation );
				Utils::DrawVec3Control( "Rotation", rotation );
				component.Rotation = glm::radians( rotation );
				Utils::DrawVec3Control( "Scale", component.Scale, 1.0f );
			} );
	}

#pragma endregion

}

#endif // IS_EDITOR