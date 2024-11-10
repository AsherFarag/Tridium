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

	static const std::unordered_map<entt::id_type, const char*> s_ComponentIcons =
	{
		{ entt::hashed_string( "CameraComponent" ).value(), TE_ICON_CAMERA },
		{ entt::hashed_string( "SpriteComponent" ).value(), TE_ICON_IMAGE },
		{ entt::hashed_string( "StaticMeshComponent" ).value(), TE_ICON_SHAPES },
		{ entt::hashed_string( "LuaScriptComponent" ).value(), TE_ICON_FILE_CODE },
		{ entt::hashed_string( "DirectionalLightComponent" ).value(), TE_ICON_LIGHTBULB },
		{ entt::hashed_string( "PointLightComponent" ).value(), TE_ICON_LIGHTBULB },
		{ entt::hashed_string( "SpotLightComponent" ).value(), TE_ICON_LIGHTBULB },
		{ entt::hashed_string( "BoxColliderComponent" ).value(), TE_ICON_CUBE },
		{ entt::hashed_string( "SphereColliderComponent" ).value(), TE_ICON_CIRCLE },
		{ entt::hashed_string( "CapsuleColliderComponent" ).value(), TE_ICON_CAPSULES },
		{ entt::hashed_string( "MeshColliderComponent" ).value(), TE_ICON_SHAPES },
	};

	void InspectorPanel::OnImGuiDraw()
	{
		// Early out if there is no GameObject to inspect
		if ( !ImGui::Begin( TE_ICON_MAGNIFYING_GLASS " Inspector" ) || !InspectedGameObject.IsValid() )
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
				- ImGui::CalcTextSize( TE_ICON_GEAR ).x
				- ( 2.0f * ImGui::GetStyle().FramePadding.x )
			);
			std::string optionsName = fmt::format( TE_ICON_GEAR "##{0}", a_Name );
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
			if ( ImGui::InputText( TE_ICON_PENCIL "##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				tagComponent->Tag = std::move(tag);
			}
		}

		ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( TE_ICON_TRASH_CAN ).x );

		ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Button, Style::Colors::Red.Value );

		if ( ImGui::Button( TE_ICON_TRASH_CAN ) ) 
		{ 
			InspectedGameObject.Destroy();
		}

		ImGui::PopStyleColor();

		ImGui::Separator();

		// Draw all components attached to the GameObject

		// Draw TransformComponent first
		if ( TransformComponent* tc = InspectedGameObject.TryGetComponent<TransformComponent>() )
		{
			if ( DrawComponentTreeNode( TE_ICON_CUBES " Transform", false ) )
			{
				DrawProperty( "Position", tc->Position, EDrawPropertyFlags::Editable );
				DrawProperty( "Rotation", tc->Rotation, EDrawPropertyFlags::Editable );
				DrawProperty( "Scale   ", tc->Scale, EDrawPropertyFlags::Editable );

				ImGui::TreePop();
			}
		}

		auto components = InspectedGameObject.GetAllComponents();
		for ( auto& [metaType, component] : components )
		{
			if ( s_BlacklistedComponents.contains( metaType.id() ) )
				continue;

			const char* icon = s_ComponentIcons.contains( metaType.id() ) ? s_ComponentIcons.at( metaType.id() ) : TE_ICON_GEARS;
			std::string className = fmt::format( "{0} {1}", icon, Refl::MetaRegistry::GetCleanTypeName( metaType ) );

			// Component options popup
			if ( ImGui::BeginPopup( className.c_str() ) )
			{
				if ( ImGui::MenuItem( TE_ICON_X " Remove Component" ) )
				{
					Tridium::Refl::Internal::RemoveFromGameObjectFunc removeFunc;
					if ( Tridium::Refl::MetaRegistry::TryGetMetaPropertyFromClass( metaType, removeFunc, Tridium::Refl::Internal::RemoveFromGameObjectPropID ) )
						removeFunc( *Application::GetScene(),  InspectedGameObject );
					else
						TE_CORE_ERROR( "Component [{0}] does not have a RemoveFromGameObject function!", Refl::MetaRegistry::GetCleanTypeName( metaType ) );
				}

				ImGui::EndPopup();
			}

			if ( !DrawComponentTreeNode( className.c_str(), true) )
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

		if ( ImGui::Button( TE_ICON_PLUS " Add Component" ) )
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
					addToGameObjectFunc.value().cast<Tridium::Refl::Internal::AddToGameObjectFunc>()( *Application::GetScene(), InspectedGameObject );
				}
				break;
			}

			ImGui::EndMenu();
		}
	}

}

#endif