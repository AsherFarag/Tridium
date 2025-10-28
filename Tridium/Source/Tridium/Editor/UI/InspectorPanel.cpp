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
			m_InspectedObject = Editor::Get()->GetSelectionContext().SelectedObject;
		}

		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			if ( m_InspectedObject.Valid() )
			{
				UI_DrawHeader();
				ImGui::Separator();
				UI_DrawComponents();
				ImGui::Separator();
				UI_DrawAddComponent();
			}
			else
			{
				const float textWidth = ImGui::CalcTextSize( "No game object selected." ).x;
				ImGui::SetCursorPosX( ( ImGui::GetContentRegionMax().x - textWidth ) * 0.5f );
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ImGui::GetContentRegionMax().y * 0.5f - ImGui::GetTextLineHeightWithSpacing() * 0.5f );
				ImGui::TextDisabled( "No game object selected." );
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
	}

	void InspectorPanel::UI_DrawComponents()
	{
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
						metaData->Editor.Component.CreateUserAction( m_InspectedObject, EComponentUserActionType::Modify ) 
					);
				}

				UI::EndTree();
			}

			if ( remove )
			{
				Editor::GetUserActionManager().Push(
					metaData->Editor.Component.CreateUserAction( m_InspectedObject, EComponentUserActionType::Remove )
				);

				metaData->Component.Remove( m_InspectedObject.Scene()->Registry(), m_InspectedObject );
			}
		};

		// We want to always draw the Name and Transform components first.
		if ( IconComponent* icon = m_InspectedObject.TryGet<IconComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<IconComponent>(), icon,
			+[]() -> bool
			{
				return false;
			} );
		}

		if ( UUIDComponent* uuid = m_InspectedObject.TryGet<UUIDComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<UUIDComponent>(), uuid );
		}

		if ( NameComponent* name = m_InspectedObject.TryGet<NameComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<NameComponent>(), name );
		}

		if ( TransformComponent* transform = m_InspectedObject.TryGet<TransformComponent>() )
		{
			DrawComponent( Meta::GetRuntimeMetaInfo<TransformComponent>(), transform );
		}

		// Iterates over all component storages in the registry,
		// and draws the ones that the inspected object has.
		for ( auto [id, storage] : m_InspectedObject.Scene()->Registry().Storage() )
		{
			if ( !storage.contains( m_InspectedObject ) )
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
			if ( void* componentData = metaData->Component.TryGet( m_InspectedObject.Scene()->Registry(), m_InspectedObject ) )
			{
				DrawComponent( metaData, componentData );
			}
		}
	}

	void InspectorPanel::UI_DrawAddComponent()
	{
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

				if ( !metaType->Component.IsComponent )
					continue; // Not a component, skip.

				if ( metaType->Component.Has( m_InspectedObject.Scene()->Registry(), m_InspectedObject ) )
					continue; // Already has component, skip.

				if ( ImGui::MenuItem( metaType->Editor.DisplayName.data() ) )
				{
					metaType->Component.EmplaceOrReplace( m_InspectedObject.Scene()->Registry(), m_InspectedObject );
					Editor::GetUserActionManager().Push( metaType->Editor.Component.CreateUserAction( m_InspectedObject, EComponentUserActionType::Add ) );
					break;
				}
			}

			ImGui::EndMenu();
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR