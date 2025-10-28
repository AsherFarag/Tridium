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

				if ( ImGui::BeginPopupContextWindow( nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems ) )
				{
					UI_DrawAddGameObjectMenu( {} );
					ImGui::EndPopup();
				}

				ImGui::EndChild();
			}
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

		if ( ImGui::BeginPopup( "AddGameObjectMenu" ) )
		{
			UI_DrawAddGameObjectMenu( {} );
			ImGui::EndPopup();
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
			GameObject gameObject( activeScene.get(), entity );
			auto* hierarchy = gameObject.TryGet<HierarchyComponent>();

			// Only draw root nodes here; children will be drawn recursively
			if ( hierarchy == nullptr || hierarchy->Parent == NullEntity )
			{
				UI_DrawHierarchyNode( gameObject );
			}
		}
	}

	void SceneHierarchyPanel::UI_DrawHierarchyNode( GameObject a_GameObject )
	{
		StringView name = "No Name";
		if ( NameComponent* nameComponent = a_GameObject.TryGet<NameComponent>() )
		{
			name = nameComponent->Name;
		}

		if ( !m_SearchFilter.PassFilter( name.data() ) )
		{
			return; // Skip this GameObject if it doesn't match the search filter
		}

		StringView icon = EditorIcons::Cube;
		if ( IconComponent* iconComp = a_GameObject.TryGet<IconComponent>() )
		{
			icon = iconComp->Icon;
		}

		auto* hierarchy = a_GameObject.TryGet<HierarchyComponent>();

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_FramePadding;
		nodeFlags |= hierarchy && hierarchy->FirstChild != NullEntity ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf;
		nodeFlags |= SelectionManager::Get().IsSelected( a_GameObject ) ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

		if ( ImGui::TreeNodeEx( (void*)(uintptr_t)(uint32_t)a_GameObject.ID(), nodeFlags, name.data() ) )
		{
			ImGui::TreePop();
		}

		if ( ImGui::IsItemClicked()  )
		{
			if ( !Input::IsKeyPressed( EInputKey::LeftControl ) )
				SelectionManager::Get().DeselectAll( ESelectionContext::Scene );

			SelectionManager::Get().Select( ESelectionContext::Scene, a_GameObject );
		}
	}

	void SceneHierarchyPanel::UI_DrawAddGameObjectMenu( GameObject a_Parent )
	{
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

		// Folder
		{
			if ( ImGui::MenuItem( "Folder" ) )
			{
				GameObject folder = activeScene->CreateEmptyGameObject();
				folder.Add<HierarchyComponent>();
				folder.Add<NameComponent>().Name = "Folder";
				folder.Add<IconComponent>().Icon = EditorIcons::Folder;

				ImGui::CloseCurrentPopup();
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Creates a new GameObject intended to be used as a folder in the hierarchy. Equivalent to Scene::CreateGameObject()." );
				ImGui::EndTooltip();
			}
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR