#include "tripch.h"

#if IS_EDITOR

#include "InspectorPanel.h"

#include <Editor/Editor.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Reflection/Reflection.h>

#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Graphics/Rendering/Material.h>
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Editor/EditorUtil.h>
#include <Editor/PropertyDrawers.h>
#include <Tridium/Asset/AssetManager.h>

using namespace entt::literals;

namespace Tridium {

	// Add spaces between words in a class name
	// Example: "EnemyAIComponent" -> "Enemy AI Component"
	std::string ScrubClassName( const char* a_ClassName )
	{
		if ( !a_ClassName )
			return "";
		std::string scrubbedName;
		const size_t size = strlen( a_ClassName );
		for ( size_t i = 0; i < size; ++i )
		{
			if ( i > 0 && isupper( a_ClassName[i] ) )
			{
				if ( ( i + 1 < size && islower( a_ClassName[i + 1] ) )
					|| 
					 ( i - 1 > 0 && islower( a_ClassName[i - 1] ) ) )
					scrubbedName.push_back( ' ' );
			}
			scrubbedName.push_back( a_ClassName[i] );
		}

		return scrubbedName;
	}

	// Create a set of blacklisted components that should not be drawn in the inspector
	static const std::unordered_set<entt::id_type> s_BlacklistedComponents =
	{
		entt::hashed_string("Component").value(),
		entt::hashed_string( "TagComponent" ).value(),
		entt::hashed_string( "TransformComponent" ).value(),
		entt::hashed_string( "GUIDComponent" ).value(),
		entt::hashed_string( "NativeScriptComponent" ).value(),
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

	InspectorPanel::InspectorPanel()
		: Panel( "Inspector" )
	{
		m_OnGameObjectSelectedHandle = Events::OnGameObjectSelected.Add<&InspectorPanel::SetInspectedGameObject>( this );
	}

	InspectorPanel::~InspectorPanel()
	{
	}

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

	void InspectorPanel::SetInspectedGameObject( GameObject gameObject )
	{
		const bool isSameGameObject = InspectedGameObject == gameObject;
		InspectedGameObject = gameObject;
		
		// Bring the window to the front when a new GameObject is selected
		if ( !isSameGameObject )
		{
			ImGui::SetWindowFocus( m_Name.c_str() );
		}
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
				Editor::DrawProperty( "Position", tc->Position, EDrawPropertyFlags::Editable );
				Editor::DrawProperty( "Rotation", tc->Rotation, EDrawPropertyFlags::Editable );
				Editor::DrawProperty( "Scale   ", tc->Scale, EDrawPropertyFlags::Editable );

				ImGui::TreePop();
			}
		}

		auto components = InspectedGameObject.GetAllComponents();
		for ( auto& [metaType, component] : components )
		{
			if ( s_BlacklistedComponents.contains( metaType.ID() ) )
				continue;

			const char* icon = s_ComponentIcons.contains( metaType.ID() ) ? s_ComponentIcons.at( metaType.ID() ) : TE_ICON_GEARS;
			std::string className = fmt::format( "{0} {1}", icon, ScrubClassName( metaType.GetCleanTypeName() ) );

			// Component options popup
			if ( ImGui::BeginPopup( className.c_str() ) )
			{
				if ( ImGui::MenuItem( TE_ICON_X " Remove Component" ) )
				{
					auto removeFromGameObjectFunc = metaType.GetMetaAttribute<Refl::Props::RemoveFromGameObjectProp::Type>( Refl::Props::RemoveFromGameObjectProp::ID );
					if ( CORE_ASSERT( removeFromGameObjectFunc.has_value() ) )
						removeFromGameObjectFunc.value()( *SceneManager::GetActiveScene(), InspectedGameObject );
				}

				ImGui::EndPopup();
			}

			if ( !DrawComponentTreeNode( className.c_str(), true) )
				continue;

			Refl::MetaAny handle = metaType.FromVoid( component );
			Tridium::Refl::Internal::DrawAllMembersOfMetaClass( metaType, handle );

			ImGui::TreePop();
		}
	}

	template <typename T, typename... Args>
	void AddComponentToGameObject( GameObject go, Args&&... args )
	{
		if ( go.TryAddComponent<T>( std::forward<Args>( args )... ) == nullptr )
			LOG_ERROR( "{0} already has a [{1}]!", go.GetTag(), typeid( T ).name() );
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
				FilePath filePath( static_cast<const char*>( payload->Data ) );
				auto ext = filePath.GetExtension();

				TODO( "DragDrop Add Component" );
			}
			ImGui::EndDragDropTarget();
		}

		if ( ImGui::BeginPopup( "AddComponent" ) )
		{
			for ( auto&& [id, type] : entt::resolve() )
			{
				Refl::MetaType metaType = type;
				if ( s_BlacklistedComponents.contains( metaType.ID() ) )
					continue;

				if ( !metaType.IsComponent() )
					continue;

				std::string className = ScrubClassName( metaType.GetCleanTypeName() );
				if ( !ImGui::MenuItem( className.c_str() ) )
					continue;

				auto addToGameObjectFunc = metaType.GetMetaAttribute<Refl::Props::AddToGameObjectProp::Type>( Refl::Props::AddToGameObjectProp::ID );
				if ( CORE_ASSERT( addToGameObjectFunc.has_value() ) )
					addToGameObjectFunc.value()( *SceneManager::GetActiveScene(), InspectedGameObject );

				break;
			}

			ImGui::EndMenu();
		}
	}

}

#endif