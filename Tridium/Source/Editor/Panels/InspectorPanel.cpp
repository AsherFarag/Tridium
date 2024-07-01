#include "tripch.h"

#ifdef IS_EDITOR

#include "InspectorPanel.h"

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>

#include <Tridium/Rendering/Texture.h>

namespace ImGui {
	static void DrawVec3Control( const std::string& label, Vector3& values, float speed, const char* format = "%.4f" )
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

	void InspectorPanel::OnImGuiDraw()
	{
		// Early out if there is no GameObject to inspect
		if ( !ImGui::Begin( "Inspector" ) || !InspectedGameObject.IsValid() )
		{
			ImGui::End();
			return;
		}

		// - Draw Components -
		DrawInspectedGameObject();

		ImGui::Separator();

		DrawAddComponentButton();

		ImGui::End();
	}

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
				if ( !std::is_same<T, TransformComponent>() )
				{
					if ( ImGui::MenuItem( "Remove component" ) )
						removeComponent = true;
				}

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

	void InspectorPanel::DrawInspectedGameObject()
	{
		auto& tag = InspectedGameObject.GetComponent<TagComponent>();
		ImGui::InputText( "Tag", (char*)tag.Tag.c_str(), tag.MaxSize() );

		ImGui::SameLine();

		ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Button, ImVec4( 1, 0.2, 0.2, 1 ) );
		if ( ImGui::Button( "Destroy" ) ) { InspectedGameObject.Destroy(); }
		ImGui::PopStyleColor();

		DrawComponent<TransformComponent>( "Transform", InspectedGameObject, []( auto& component )
			{
				ImGui::DrawVec3Control( "Position", component.Position, 0.01f );
				Vector3 rotation = glm::degrees( glm::eulerAngles( component.Rotation ) );
				ImGui::DrawVec3Control( "Rotation", rotation, 1.f );
				component.Rotation = glm::radians( rotation );
				ImGui::DrawVec3Control( "Scale", component.Scale, 0.01f );
			} );

		DrawComponent<MeshComponent>( "Mesh", InspectedGameObject, []( auto& component )
			{
			} );

		DrawComponent<CameraComponent>( "Camera", InspectedGameObject, []( auto& component )
			{
				int currentItem = (int)component.SceneCamera.GetProjectionType();
				Camera::ProjectionType projType = component.SceneCamera.GetProjectionType();
				const char* const items[ 2 ] = { "Perspective", "Orthographic" };

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

		DrawComponent<CameraControllerComponent>( "Camera Controller", InspectedGameObject, []( auto& component )
			{
				ImGui::DragFloat( "Speed", &component.Speed, 0.1f );
				ImGui::DragFloat( "Look Sensitivity", &component.LookSensitivity, 0.1f );
			} );

		DrawComponent<LuaScriptComponent>( "Lua Script Component", InspectedGameObject, []( auto& component )
			{
				ImGui::Text( "Script: " );
				ImGui::SameLine();
				if ( component.GetScript() )
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, component.GetScript()->GetFilePath().string().c_str() );
				else
					ImGui::TextColored( { 0.85, 0.65, 0.1, 0.9 }, "Empty" );

				if ( ImGui::BeginDragDropTarget() )
				{
					if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "ContentBrowserItem" ) )
					{
						component.SetScript( Script::Create( static_cast<const char*>( payload->Data ) ) );
					}
					ImGui::EndDragDropTarget();
				}
			} );

		DrawComponent<SpriteComponent>( "Sprite Component", InspectedGameObject, []( auto& component )
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

					if ( textureSize.x >= textureSize.y )
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
					ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_FrameRounding, 5 );
					ImGui::Image( (ImTextureID)component.GetTexture()->GetRendererID(),
						previewSize,
						{ 0, 1 }, { 1, 0 } );
					ImGui::PopStyleVar();

					ImGui::TreePop();
				}
			} );
	}

	template <typename T, typename... Args>
	void AddComponentToGameObject( GameObject go, Args&&... args )
	{
		if ( go.TryAddComponent<T>( std::forward<Args>( args )... ) == nullptr )
			TE_CORE_ERROR( "{0} already has a [{1}]!", go.GetTag(), typeid( T ).name() );
	}

	void InspectorPanel::DrawAddComponentButton()
	{
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
				AddComponentToGameObject<LuaScriptComponent>( InspectedGameObject, Script::Create( static_cast<const char*>( payload->Data ) ) );
			}
			ImGui::EndDragDropTarget();
		}

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			if ( ImGui::MenuItem( "Transform" ) )		    { AddComponentToGameObject<TransformComponent>( InspectedGameObject );	}
			if ( ImGui::MenuItem( "Mesh" ) )			    { AddComponentToGameObject<MeshComponent>( InspectedGameObject ); }
			if ( ImGui::MenuItem( "Camera" ) )			    { AddComponentToGameObject<CameraComponent>( InspectedGameObject ); }
			if ( ImGui::MenuItem( "Camera Controller" ) ) { AddComponentToGameObject<CameraControllerComponent>( InspectedGameObject ); }
			if ( ImGui::MenuItem( "Lua Script" ) )		    { AddComponentToGameObject<LuaScriptComponent>( InspectedGameObject ); }
			if ( ImGui::MenuItem( "Sprite" ) )		        { AddComponentToGameObject<SpriteComponent>( InspectedGameObject ); }

			ImGui::EndMenu();
		}
	}

}

#endif