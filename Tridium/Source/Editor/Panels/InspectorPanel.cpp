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

		ImGui::Separator();

		DrawAddComponentButton();

		ImGui::End();
	}

	void DrawComponent( const Refl::MetaType& a_MetaType, Refl::MetaAny&& a_Component )
	{
		// Check if the type has a DrawPropFunc property associated with it
		if ( auto prop = a_MetaType.prop( Internal::DrawPropFuncID ) )
		{
			// Try to retrieve the DrawPropFunc and call it
			if ( prop.value().try_cast<Internal::DrawPropFunc>( ) )
			{
				Internal::DrawPropFunc drawFunc = prop.value().cast<Internal::DrawPropFunc>();
				const char* className = Refl::MetaRegistry::GetCleanTypeName( a_MetaType );
				drawFunc( className, a_Component, static_cast<::Tridium::Refl::PropertyFlags>( ::Tridium::Refl::EPropertyFlag::EditAnywhere ) );
			}
		}
	}

	void DrawComponent( const Refl::MetaType& a_MetaType, Refl::MetaAny& a_Component )
	{
		// Check if the type has a DrawPropFunc property associated with it
		if ( auto prop = a_MetaType.prop( Internal::DrawPropFuncID ) )
		{
			// Try to retrieve the DrawPropFunc and call it
			if ( prop.value().try_cast<Internal::DrawPropFunc>( ) )
			{
				Internal::DrawPropFunc drawFunc = prop.value().cast<Internal::DrawPropFunc>();
				const char* className = Refl::MetaRegistry::GetCleanTypeName( a_MetaType );
				drawFunc( className, a_Component, static_cast<::Tridium::Refl::PropertyFlags>( ::Tridium::Refl::EPropertyFlag::EditAnywhere ) );
			}
		}
	}

	void InspectorPanel::DrawInspectedGameObject()
	{
		if ( TagComponent* tagComponent = InspectedGameObject.TryGetComponent<TagComponent>() )
		{
			std::string tag = InspectedGameObject.GetComponent<TagComponent>().Tag;
			if ( ImGui::InputText( "Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				tagComponent->Tag = std::move(tag);
			}
		}

		ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Destroy").x);

		ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Button, Style::Colors::Red.Value );
		if ( ImGui::Button( "Destroy" ) ) { InspectedGameObject.Destroy(); }
		ImGui::PopStyleColor();

		ImGui::Separator();

		// Draw all components attached to the GameObject

		// Draw TransformComponent first
		if ( TransformComponent* tc = InspectedGameObject.TryGetComponent<TransformComponent>() )
		{
			DrawComponent( 
				Refl::MetaRegistry::ResolveMetaType<TransformComponent>(), 
				entt::forward_as_meta( *tc ) );
		}

		// Create a set of blacklisted components that should not be drawn in the inspector
		static const std::unordered_set<entt::id_type> BlacklistedComponents =
		{
			entt::type_hash<TransformComponent>::value(),
			entt::type_hash<TagComponent>::value(),
			entt::type_hash<GUIDComponent>::value(),
		};

		auto& registry = Application::Get().GetScene()->GetRegistry();
		for ( auto&& [id, storage] : registry.storage() )
		{
			if ( storage.size() == 0 )
				continue;

			if ( !storage.contains( InspectedGameObject ) )
				continue;

			// Resolve the meta type for the current component type
			Refl::MetaType metaType = Refl::MetaRegistry::ResolveMetaType( storage.type() );
			if ( !metaType || BlacklistedComponents.contains( metaType.id() ) )
				continue;

			void* componentPtr = storage.value( InspectedGameObject );
			if ( !componentPtr )
				continue;

			Refl::MetaAny component = metaType.from_void( componentPtr );

			DrawComponent( metaType, component );
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