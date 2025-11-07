#include "InspectorPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/UserActions/SceneActions.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Scene/SceneManager.h>
#include <Tridium/Reflection/RuntimeMeta.h>
#include <Tridium/UI/PropertyDrawers.h>

namespace Tridium {

	void InspectorPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		m_ComponentFilter.Build();

		if ( !m_LockInspector )
		{
			m_InspectedObjects.Clear();

			for ( const Selectable& selected : SelectionManager::Get().Selections( ESelectionContext::Scene ) )
			{
				if ( !std::holds_alternative<GameObject>( selected ) )
					continue;

				GameObject gameObject = std::get<GameObject>( selected );

				if ( !gameObject.Valid() )
					continue;

				m_InspectedObjects.PushBack( gameObject );
			}
		}

		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			if ( m_InspectedObjects.Size() == 1 )
			{
				UI_DrawHeader();

				ImGui::Separator();

				// Calculate space to leave for bottom UI
				float bottomReserve =
					ImGui::GetFrameHeightWithSpacing() + // height of one button row
					ImGui::GetStyle().ItemSpacing.y * 2 + // extra breathing room
					4.0f; // small safety margin

				if ( ImGui::BeginChild( "##ComponentList", ImVec2( 0, -bottomReserve ), 0, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground ) )
				{
					UI_DrawComponents();
				}

				ImGui::EndChild();

				ImGui::Separator();

				UI_DrawAddComponent();
			}
			else
			{
				const char* message = m_InspectedObjects.Size() > 1 ? "Only one game object can be inspected at a time." : "No game object selected.";
				float textWidth = ImGui::CalcTextSize( message ).x;
				textWidth = Math::Min( textWidth, ImGui::GetContentRegionMax().x );

				ImGui::SetCursorPosX( ( ImGui::GetContentRegionMax().x - textWidth ) * 0.5f );
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ImGui::GetContentRegionMax().y * 0.5f - ImGui::GetTextLineHeightWithSpacing() * 0.5f );

				ImGui::BeginDisabled();
				ImGui::TextWrapped( message );
				ImGui::EndDisabled();
			}
		}

		ImGui::End();
	}

	void InspectorPanel::UI_DrawHeader()
	{
		const float lockButtonWidth = UI::CalcButtonSize( m_LockInspector ? EditorIcons::Lock : EditorIcons::LockOpen ).X;

		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - lockButtonWidth - ImGui::GetStyle().ItemSpacing.x );
		if ( ImGui::InputTextWithHint( "##ComponentSearchBar", TE_ICON_MAGNIFYING_GLASS " Search for Component...", m_ComponentFilter.InputBuf, IM_ARRAYSIZE( m_ComponentFilter.InputBuf ) ) )
			m_ComponentFilter.Build();

		ImGui::SameLine();
		if ( ImGui::Button( m_LockInspector ? EditorIcons::Lock.Data : EditorIcons::LockOpen.Data ) )
		{
			m_LockInspector = !m_LockInspector;
		}

		UI::BeginPropertyGrid();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex( 0 );
		ImGui::AlignTextToFramePadding();

		Entity entity = m_InspectedObjects[0];

		ImGui::Text( "ID: %u", GetEntityID( entity ) );

		ImGui::TableSetColumnIndex( 1 );
		ImGui::PushItemWidth( -FLT_MIN );

		ImGui::Text( "Version: %u", (uint32_t)GetEntityVersion( entity ) );

		ImGui::PopItemWidth();

		UI::EndPropertyGrid();
	}

	void InspectorPanel::UI_DrawComponents()
	{
		TODO( "Support multi-object editing in the inspector." );
		GameObject inspectedObject = m_InspectedObjects[0];

		const auto BeginComponentTree = []( StringView a_Name, bool& o_Delete )
		{
			const float contentWidth = ImGui::GetContentRegionAvail().x;
			const float buttonWidth = UI::CalcButtonSize( TE_ICON_TRASH_CAN ).X;

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX( ImGui::GetContentRegionMax().x - buttonWidth );

			ImGui::PushID( UI::GenerateID() );
			o_Delete = ImGui::Button( TE_ICON_TRASH );
			ImGui::PopID();

			ImGui::SetCursorPos( cursorPos );
			const float treeWidth = contentWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x;
			bool opened = UI::BeginTree( a_Name, UI::ETreeFlags::DefaultOpen | UI::ETreeFlags::Framed, treeWidth );

			return opened;
		};

		const auto DrawComponent = [&]( const Meta::RuntimeMetaInfo* metaData, void* componentData, bool( *drawFunc )() = nullptr )
		{
			StringView displayName = metaData->Editor.DisplayName;

			if ( !m_ComponentFilter.PassFilter( displayName.data(),
											    displayName.data() + displayName.size() ) )
			{
				return;
			}

			bool remove = false;

			if ( BeginComponentTree( displayName, remove ) )
			{
				ImGui::BeginGroup();
				bool modified = drawFunc ? drawFunc() : metaData->Editor.Draw( {}, componentData );
				ImGui::EndGroup();

				if ( ImGui::IsItemActivated() )
				{
					Editor::GetUserActionManager().Push( 
						metaData->Editor.Component.CreateUserAction( inspectedObject, EComponentUserActionType::Modify )
					);
				}

				UI::EndTree();
			}

			if ( remove )
			{
				Editor::GetUserActionManager().Push(
					metaData->Editor.Component.CreateUserAction( inspectedObject, EComponentUserActionType::Remove )
				);

				metaData->Component.Remove( inspectedObject.Scene()->Registry(), inspectedObject );
			}
		};

		// We want to always draw the Name and Transform components first.
		if ( UUIDComponent* uuid = inspectedObject.TryGet<UUIDComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<UUIDComponent>(), uuid );
		}

		if ( NameComponent* name = inspectedObject.TryGet<NameComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<NameComponent>(), name );
		}

		if ( TransformComponent* transform = inspectedObject.TryGet<TransformComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<TransformComponent>(), transform );
		}

		if ( IconComponent* icon = inspectedObject.TryGet<IconComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<IconComponent>(), icon,
			+[]() -> bool
			{
				return false;
			} );
		}

		// Iterates over all component storages in the registry,
		// and draws the ones that the inspected object has.
		for ( auto [id, storage] : inspectedObject.Scene()->Registry().Storage() )
		{
			if ( !storage.contains( inspectedObject.Entity() ) )
			{
				continue;
			}

			const Meta::RuntimeMetaInfo* metaData = Meta::GetRuntimeMetaInfo( id );

			// Skip non-component types.
			if ( !metaData || !metaData->Component.IsComponent )
			{
				continue;
			}

			// Skip components that are hidden in the inspector.
			if ( metaData->Editor.Component.HideInInspector )
			{
				continue;
			}

			// If we can get the component data, draw it.
			if ( void* componentData = metaData->Component.TryGet( inspectedObject.Scene()->Registry(), inspectedObject ) )
			{
				DrawComponent( metaData, componentData );
			}
		}
	}

	void InspectorPanel::UI_DrawAddComponent()
	{
		TODO( "Support multi-object editing in the inspector." );
		GameObject inspectedObject = m_InspectedObjects[0];

		// Center the button horizontally.
		ImGui::SetCursorPosX( ( ImGui::GetContentRegionAvail().x - UI::CalcButtonSize( "Add Component" ).X ) * 0.5f );
		if ( ImGui::Button( "Add Component" ) )
		{
			ImGui::OpenPopup( "AddComponentPopup" );
		}

		if ( ImGui::BeginPopup( "AddComponentPopup" ) )
		{
			for ( auto [id, type] : entt::resolve() )
			{
				const Meta::RuntimeMetaInfo* metaType = Meta::GetRuntimeMetaInfo( id );

				if ( !metaType || !metaType->Component.IsComponent )
					continue; // Not a component, skip.

				if ( metaType->Component.Has( inspectedObject.Scene()->Registry(), inspectedObject ) )
					continue; // Already has component, skip.

				if ( ImGui::MenuItem( metaType->Editor.DisplayName.data() ) )
				{
					metaType->Component.EmplaceOrReplace( inspectedObject.Scene()->Registry(), inspectedObject );
					Editor::GetUserActionManager().Push( metaType->Editor.Component.CreateUserAction( inspectedObject, EComponentUserActionType::Add ) );
					break;
				}
			}

			ImGui::EndMenu();
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR