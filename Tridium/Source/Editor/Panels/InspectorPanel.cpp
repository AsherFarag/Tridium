#include "tripch.h"

#ifdef IS_EDITOR

#include "InspectorPanel.h"

#include <Editor/Editor.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>

#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>
#include <Editor/EditorUtil.h>
#include <Tridium/Asset/AssetManager.h>

using namespace entt::literals;

namespace ImGui {
	static void DrawVec3Control( const std::string& label, Vector3& values, float speed, bool uniform = false, const char* format = "%.4f" )
	{

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


		// Draw all components
		auto& registry = Application::Get().GetScene()->GetRegistry();
		for ( auto&& [id, storage] : registry.storage() )
		{
			if ( storage.size() == 0 )
				continue;

			// Resolve the meta type for the current component type
			Refl::MetaType metaType = entt::resolve( storage.type() );
			if ( !metaType )  // Ensure meta type exists
				continue;

			void* componentPtr = storage.value( InspectedGameObject );
			if ( !componentPtr )
				continue;

			Refl::MetaAny component = metaType.from_void( componentPtr );

			// Check if the type has a DrawPropFunc property associated with it
			if ( auto prop = metaType.prop( Internal::DrawPropFuncID ) )
			{
				// Try to retrieve the DrawPropFunc and call it
				if ( prop.value().try_cast<Internal::DrawPropFunc>( ) )
				{
					Internal::DrawPropFunc drawFunc = prop.value().cast<Internal::DrawPropFunc>();
					const char* className = metaType.prop( "CleanClassName"_hs ).value().cast<const char*>();
					drawFunc( className, component, 0, static_cast<::Tridium::Refl::PropertyFlags>( ::Tridium::Refl::EPropertyFlag::EditAnywhere ));
				}

				ImGui::Separator();
			}
		}
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
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_CONTENT_BROWSER_ITEM ) )
			{
				IO::FilePath filePath( static_cast<const char*>( payload->Data ) );
				auto ext = filePath.GetExtension();
				if ( ext == ".lua" ) {
					AddComponentToGameObject<LuaScriptComponent>( InspectedGameObject, Script::Create( filePath ) );
				}
				else if ( ext == ".png" ) {
					AddComponentToGameObject<SpriteComponent>( InspectedGameObject, AssetManager::GetAsset<Texture>( filePath.ToString() ) );
				}
				else if ( ext == ".obj" || ext == ".fbx" ) {
					AddComponentToGameObject<MeshComponent>( InspectedGameObject, AssetManager::GetAsset<Mesh>( filePath.ToString() ) );
				}
			}
			ImGui::EndDragDropTarget();
		}

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			for ( auto&& [id, metaType] : entt::resolve() )
			{
				if ( auto isComponentProp = metaType.prop( Refl::IsComponentID ); !isComponentProp )
					continue;

				const char* className = metaType.prop( "CleanClassName"_hs ).value().cast<const char*>();
				if ( !ImGui::MenuItem( className ) )
					continue;

				if ( auto addToGameObjectFunc = metaType.prop( Tridium::Refl::Internal::AddToGameObjectPropID ); addToGameObjectFunc )
				{
					addToGameObjectFunc.value().cast<Tridium::Refl::Internal::AddToGameObjectFunc>()( InspectedGameObject );
				}
				break;
			}

			ImGui::EndMenu();
		}
	}

}

#endif