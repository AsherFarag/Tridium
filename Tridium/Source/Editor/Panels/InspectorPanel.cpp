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

namespace Tridium::Editor {

	// Create a set of blacklisted components that should not be drawn in the inspector
	static const std::unordered_set<entt::id_type> s_BlacklistedComponents =
	{
		entt::hashed_string("Component").value(),
		entt::hashed_string( "TagComponent" ).value(),
		entt::hashed_string( "TransformComponent" ).value(),
		entt::hashed_string( "GUIDComponent" ).value(),
		entt::hashed_string( "ScriptableComponent" ).value(),
	};

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

	bool DrawComponentTreeNode(const char* a_Name, bool a_HasOptionsButton)
	{
		ImGui::SetNextItemAllowOverlap();
		const bool isOpen = ImGui::TreeNodeEx( a_Name, ImGuiTreeNodeFlags_Framed );

		if ( a_HasOptionsButton )
		{
			ImGui::SameLine( 
				ImGui::GetContentRegionMax().x 
				- ImGui::CalcTextSize( "*" ).x
				- ( 2.0f * ImGui::GetStyle().FramePadding.x )
			);
			std::string optionsName( TE_ICON_GEAR "##", a_Name );
			if ( ImGui::SmallButton( optionsName.c_str() ) )
				ImGui::OpenPopup( a_Name );
		}

		return isOpen;
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

		if ( ImGui::Button( "Destroy" ) ) 
		{ 
			InspectedGameObject.Destroy();
		}

		ImGui::PopStyleColor();

		ImGui::Separator();

		// Draw all components attached to the GameObject

		// Draw TransformComponent first
		if ( TransformComponent* tc = InspectedGameObject.TryGetComponent<TransformComponent>() )
		{
			if ( DrawComponentTreeNode( "Transform", false ) )
			{
				DrawProperty( "Pos", tc->Position, EDrawPropertyFlags::Editable );
				DrawProperty( "Rot", tc->Rotation, EDrawPropertyFlags::Editable );
				DrawProperty( "Sca", tc->Scale, EDrawPropertyFlags::Editable );

				ImGui::TreePop();
			}
		}

		auto components = InspectedGameObject.GetAllComponents();
		for ( auto& [metaType, component] : components )
		{
			if ( s_BlacklistedComponents.contains( metaType.id() ) )
				continue;

			const char* className = Refl::MetaRegistry::GetCleanTypeName( metaType );

			// Component options popup
			if ( ImGui::BeginPopup( className ) )
			{
				if ( ImGui::MenuItem( "Remove Component" ) )
				{
					Tridium::Refl::Internal::RemoveFromGameObjectFunc removeFunc;
					if ( Tridium::Refl::MetaRegistry::TryGetMetaPropertyFromClass( metaType, removeFunc, Tridium::Refl::Internal::RemoveFromGameObjectPropID ) )
						removeFunc( InspectedGameObject );
					else
						TE_CORE_ERROR( "Component [{0}] does not have a RemoveFromGameObject function!", Refl::MetaRegistry::GetCleanTypeName( metaType ) );
				}

				ImGui::EndPopup();
			}

			if ( !DrawComponentTreeNode( className, true ) )
				continue;

			Refl::MetaAny handle = metaType.from_void( component );
			Tridium::Refl::Internal::DrawAllMembersOfMetaClass( metaType, handle );

			ImGui::TreePop();
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

				TODO( "DragDrop Add Component" );
			}
			ImGui::EndDragDropTarget();
		}

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			for ( auto&& [id, metaType] : entt::resolve() )
			{
				if ( s_BlacklistedComponents.contains( metaType.id() ) )
					continue;

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