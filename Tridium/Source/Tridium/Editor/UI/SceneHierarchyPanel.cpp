#include "SceneHierarchyPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Engine/Engine.h>

namespace Tridium {

	void SceneHierarchyPanel::OnEvent( Event& a_Event )
	{
	}

	void SceneHierarchyPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		if ( !SceneManager::ActiveScene() )
			return; // No active scene to draw

		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			ImGui::BeginGroup();

			UI_DrawAddButton();

			ImGui::SameLine();

			UI_DrawSearchBar();

			ImGui::EndGroup();

			if ( ImGui::BeginChild( "Hierarchy", { 0,0 }, ImGuiChildFlags_FrameStyle ) )
			{
				UI_DrawHierarchy();

				ImGui::EndChild();
			}

			UI_DrawAddGameObjectMenu();
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::UI_DrawAddButton()
	{
		ImGui::ScopedStyleCol buttonCol( ImGuiCol_Button, 0x00000000 );
		if ( ImGui::Button( TE_ICON_PLUS "##AddButton" ) )
		{
			ImGui::OpenPopup( "AddGameObjectMenu" );
		}
	}

	void SceneHierarchyPanel::UI_DrawSearchBar()
	{
		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );

		if ( ImGui::InputTextWithHint( "##SceneHierarchySearchBar", TE_ICON_MAGNIFYING_GLASS " Filter...", m_SearchFilter.InputBuf, IM_ARRAYSIZE( m_SearchFilter.InputBuf ) ) )
			m_SearchFilter.Build();
	}

	void SceneHierarchyPanel::UI_DrawHierarchy()
	{
		const auto& activeScene = SceneManager::ActiveScene();

		auto& registry = activeScene->Registry();

		for ( EntityID entity : registry.View<EntityID>() )
		{
			StringView name = "No Tag";
			if ( TagComponent* tag = registry.TryGet<TagComponent>( entity ) )
			{
				name = tag->Tag;
			}

			if ( !m_SearchFilter.PassFilter( name.data() ) )
			{
				continue; // Skip this GameObject if it doesn't match the search filter
			}

			auto* hierarchy = registry.TryGet<HierarchyComponent>( entity );

			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_FramePadding;
			nodeFlags |= hierarchy && hierarchy->FirstChild != NullEntity ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf;
			nodeFlags |= Editor::GetSelectionContext().SelectedObject == GameObject( *activeScene, entity ) ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

			if ( ImGui::TreeNodeEx( (void*)(uintptr_t)(uint32_t)entity, nodeFlags, TE_ICON_CUBE " %s", name.data() ) )
			{
				ImGui::TreePop();
			}

			if ( ImGui::IsItemClicked() )
			{
				Editor::GetSelectionContext().SelectedObject = GameObject( *activeScene, entity );
			}
		}
	}

	void SceneHierarchyPanel::UI_DrawAddGameObjectMenu()
	{
		if ( !ImGui::BeginPopup( "AddGameObjectMenu" ) )
			return; // Early out if the popup is not open or there is no active scene

		const auto& activeScene = SceneManager::ActiveScene();

		// Empty GameObject
		{
			if ( ImGui::MenuItem( "Empty GameObject" ) )
			{
				activeScene->CreateEmptyGameObject();
				ImGui::CloseCurrentPopup();
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Creates an empty GameObject with no components. Equivalent to Scene::CreateEmptyGameObject()." );
				ImGui::EndTooltip();
			}
		}

		// GameObject
		{
			if ( ImGui::MenuItem( "GameObject" ) )
			{
				activeScene->CreateGameObject( "GameObject" );
				ImGui::CloseCurrentPopup();
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Creates a new GameObject with some core components. Equivalent to Scene::CreateGameObject()." );
				ImGui::EndTooltip();
			}
		}

		ImGui::EndPopup();
	}

} // namespace Tridium

#endif // WITH_EDITOR