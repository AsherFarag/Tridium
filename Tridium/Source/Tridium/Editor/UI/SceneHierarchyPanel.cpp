#include "SceneHierarchyPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/UserActions/SceneActions.h>
#include <Tridium/Engine/Engine.h>

// Components
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Physics/PhysicsComponents.h>

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
			// Draw children
			if ( hierarchy )
			{
				EntityID childEntity = hierarchy->FirstChild;
				while ( childEntity != NullEntity )
				{
					GameObject childGameObject( a_GameObject.Scene(), childEntity );
					UI_DrawHierarchyNode( childGameObject );
					auto* childHierarchy = childGameObject.TryGet<HierarchyComponent>();
					if ( childHierarchy )
					{
						childEntity = childHierarchy->NextSibling;
					}
					else
					{
						break; // No more siblings
					}
				}
			}

			ImGui::TreePop();
		}

		// Handle payloads for drag and drop game objects
		bool isDragDropSource = false;
		if ( ImGui::BeginDragDropSource() )
		{
			isDragDropSource = true;

			ImGui::SetDragDropPayload( "SceneHierarchyGameObject", &a_GameObject, sizeof( GameObject ) );
			ImGui::TextUnformatted( name.data() );
			ImGui::EndDragDropSource();
		}

		if ( !isDragDropSource && ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "SceneHierarchyGameObject" ) )
			{
				if ( payload->IsDelivery() )
				{
					GameObject draggedGameObject = *(GameObject*)payload->Data;

					// Reparent the dragged GameObject to be a child of this GameObject
					auto& parentHierarchy = a_GameObject.GetOrAdd<HierarchyComponent>();
					auto& childHierarchy = draggedGameObject.GetOrAdd<HierarchyComponent>();
					// Set parent
					childHierarchy.Parent = a_GameObject.ID();
					// Insert as first child
					childHierarchy.NextSibling = parentHierarchy.FirstChild;
					if ( parentHierarchy.FirstChild != NullEntity )
					{
						auto& firstChildHierarchy = GameObject( a_GameObject.Scene(), parentHierarchy.FirstChild ).Get<HierarchyComponent>();
						firstChildHierarchy.PrevSibling = draggedGameObject.ID();
					}
					parentHierarchy.FirstChild = draggedGameObject.ID();
				}
			}

			ImGui::EndDragDropTarget();
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

		GameObject createdGameObject;

		// GameObject
		{
			if ( ImGui::MenuItem( TE_ICON_CUBE " GameObject" ) )
			{
				createdGameObject = activeScene->CreateGameObject( "GameObject" );
				ImGui::CloseCurrentPopup();
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Creates a new GameObject with some core components." );
				ImGui::EndTooltip();
			}
		}

		#pragma region Lighting

		ImGui::SeparatorText( "Lighting" );

		// Sky Box
		if ( ImGui::MenuItem( TE_ICON_CLOUD_SUN " Sky Box" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Sky Box" );
			createdGameObject.Add<SkyboxComponent>();
			ImGui::CloseCurrentPopup();
		}

		// Directional Light
		if ( ImGui::MenuItem( TE_ICON_SUN " Directional Light" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Directional Light" );
			createdGameObject.Add<DirectionalLightComponent>();
			ImGui::CloseCurrentPopup();
		}

		// Point Light
		if ( ImGui::MenuItem( TE_ICON_LIGHTBULB " Point Light" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Point Light" );
			createdGameObject.Add<PointLightComponent>();
			ImGui::CloseCurrentPopup();
		}

		// Spot Light
		if ( ImGui::MenuItem( TE_ICON_LIGHTBULB " Spot Light" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Spot Light" );
			createdGameObject.Add<SpotLightComponent>();
			ImGui::CloseCurrentPopup();
		}

		#pragma endregion

		#pragma region Physics

		ImGui::SeparatorText( "Physics" );

		if ( ImGui::MenuItem( TE_ICON_CIRCLE " Physics Sphere" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Physics Sphere" );
			createdGameObject.Add<RigidBodyComponent>();
			createdGameObject.Add<SphereColliderComponent>();
			ImGui::CloseCurrentPopup();
		}

		if ( ImGui::MenuItem( TE_ICON_CUBE " Physics Cube" ) )
		{
			createdGameObject = activeScene->CreateGameObject( "Physics Cube" );
			createdGameObject.Add<RigidBodyComponent>();
			createdGameObject.Add<BoxColliderComponent>();
			ImGui::CloseCurrentPopup();
		}

		#pragma endregion

		#pragma region Other

		ImGui::SeparatorText( "Other" );

		if ( ImGui::MenuItem( TE_ICON_FOLDER_OPEN " Folder" ) )
		{
			createdGameObject = activeScene->CreateEmptyGameObject();
			createdGameObject.Add<HierarchyComponent>();
			createdGameObject.Add<NameComponent>().Name = "Folder";
			createdGameObject.Add<IconComponent>().Icon = TE_ICON_FOLDER_OPEN;

			ImGui::CloseCurrentPopup();
		}

		if ( ImGui::BeginItemTooltip() )
		{
			ImGui::TextUnformatted( "Creates a new GameObject intended to be used as a folder in the hierarchy." );
			ImGui::EndTooltip();
		}

		#pragma endregion


		// If a GameObject was created, set up its hierarchy and selection
		if ( createdGameObject )
		{
			// If a parent is specified, set up the hierarchy
			if ( a_Parent )
			{
				auto& parentHierarchy = a_Parent.Get<HierarchyComponent>();
				auto& childHierarchy = createdGameObject.GetOrAdd<HierarchyComponent>();
				// Set parent
				childHierarchy.Parent = a_Parent.ID();
				// Insert as first child
				childHierarchy.NextSibling = parentHierarchy.FirstChild;
				if ( parentHierarchy.FirstChild != NullEntity )
				{
					auto& firstChildHierarchy = GameObject( activeScene.get(), parentHierarchy.FirstChild ).Get<HierarchyComponent>();
					firstChildHierarchy.PrevSibling = createdGameObject.ID();
				}
				parentHierarchy.FirstChild = createdGameObject.ID();
			}

			// Select the newly created GameObject
			SelectionManager::Get().DeselectAll( ESelectionContext::Scene );
			SelectionManager::Get().Select( ESelectionContext::Scene, createdGameObject );

			Editor::GetUserActionManager().Push( MakeUnique<GameObjectUserAction>( createdGameObject, GameObjectUserAction::EActionType::Create ) );
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR