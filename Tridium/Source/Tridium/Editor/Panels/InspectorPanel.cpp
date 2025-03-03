#include "tripch.h"

#if IS_EDITOR

#include "InspectorPanel.h"
#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/EditorUtil.h>
#include <Tridium/Editor/EditorDocumentation.h>

#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Application.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Reflection/Reflection.h>

#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Graphics/Rendering/Material.h>
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Editor/PropertyDrawers.h>
#include <Tridium/Asset/AssetManager.h>

using namespace entt::literals;

namespace Tridium {

	struct ComponentTreeNode
	{
		const char* Icon{ nullptr };
		const char* Name{ nullptr };
		bool HasOptionsButton{ false };
		bool* OptionsPressed{ nullptr };
		bool IsEnabledTogglable{ false };
		bool* IsEnabled{ nullptr };
		bool DocumentationButton{ true };
		const char* DocumentationKey{ nullptr };
	};

	namespace Helpers {

		bool DrawSmallButton( const char* a_Label )
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if ( window->SkipItems )
				return false;

			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID( a_Label );
			const ImVec2 label_size = ImGui::CalcTextSize( a_Label, NULL, true );
			const ImGuiButtonFlags flags = 0;

			ImVec2 pos = window->DC.CursorPos;
			ImVec2 size = ImGui::CalcItemSize( {0,0}, label_size.x, label_size.y );

			const ImRect bb( pos, pos + size );
			ImGui::ItemSize( size, style.FramePadding.y );
			if ( !ImGui::ItemAdd( bb, id ) )
				return false;

			bool hovered, held;
			bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );

			// Render
			const ImU32 col = ImGui::GetColorU32( ( held && hovered ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );
			const ImRect frameBB = { 
				{ bb.GetCenter().x - bb.GetHeight() * 0.5f, bb.Min.y },
				{ bb.GetCenter().x + bb.GetHeight() * 0.5f, bb.Max.y }
			};
			ImGui::RenderNavHighlight( frameBB, id );
			ImGui::RenderFrame( frameBB.Min, frameBB.Max, col, true, style.FrameRounding );

			// Center label
			const ImVec2 label_pos = ImVec2( bb.Min.x + ( bb.GetWidth() - label_size.x ) * 0.5f, bb.Min.y + ( bb.GetHeight() - label_size.y ) * 0.5f );
			ImGui::RenderText( label_pos, a_Label );

			IMGUI_TEST_ENGINE_ITEM_INFO( id, label, g.LastItemData.StatusFlags );
			return pressed;
		}

		void DrawDocumentation( TypeDocumentation* a_Documentation )
		{

			if ( !a_Documentation )
			{
				return;
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImGui::TextUnformatted( a_Documentation->Description.c_str() );

			if ( !a_Documentation->Members.IsEmpty() &&
				ImGui::TreeNode( "Members" ) )
			{
				for ( const VariableDocumentation& member : a_Documentation->Members )
				{
					if ( ImGui::TreeNode( member.Name.c_str() ) )
					{
						ImGui::TextUnformatted( member.Type.c_str() );
						ImGui::TextUnformatted( member.Description.c_str() );

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if ( !a_Documentation->Functions.IsEmpty() &&
				ImGui::TreeNode( "Functions" ) )
			{
				for ( const FunctionDocumentation& function : a_Documentation->Functions )
				{
					if ( ImGui::TreeNode( function.Name.c_str() ) )
					{
						ImGui::TextUnformatted( function.ReturnType.c_str() );
						ImGui::TextUnformatted( function.Description.c_str() );
						if ( !function.Parameters.IsEmpty() &&
							ImGui::TreeNode( "Parameters" ) )
						{
							for ( const VariableDocumentation& param : function.Parameters )
							{
								if ( ImGui::TreeNode( param.Name.c_str() ) )
								{
									ImGui::TextUnformatted( param.Type.c_str() );
									ImGui::TextUnformatted( param.Description.c_str() );

									ImGui::TreePop();
								}
							}

							ImGui::TreePop();
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}

		// Add spaces between words in a class name
		// Example: "EnemyAIComponent" -> "Enemy AI Component"
		String ScrubClassName( const char* a_ClassName )
		{
			if ( !a_ClassName )
				return "";

			String scrubbedName;
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

		const char* GetCleanClassName( const char* a_ClassName )
		{
			static UnorderedMap<String, String> s_CleanClassNames;

			if ( s_CleanClassNames.contains( a_ClassName ) )
				return s_CleanClassNames.at( a_ClassName ).c_str();

			String cleanName = ScrubClassName( a_ClassName );
			s_CleanClassNames[a_ClassName] = std::move( cleanName );
			return s_CleanClassNames.at( a_ClassName ).c_str();
		}

		bool DrawComponentTreeNode( ComponentTreeNode a_Node )
		{
			const ImGuiStyle& style = ImGui::GetStyle();

			bool isOpen = false;

			ImGui::SetNextItemAllowOverlap();

			// Draw TreeNode
			{
				ImVec4 arrowColor = ImGui::GetStyleColorVec4( ImGuiCol_Text );
				arrowColor.w = 0.5f;
				ImGui::PushStyleColor( ImGuiCol_Text, arrowColor );
				ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
				ImGui::PushID( a_Node.Name );
				isOpen = ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_Framed);
				ImGui::PopID();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}

			ImGui::PushID( ImGui::GetID( static_cast<const void*>( a_Node.Name ) ) );
			ImGui::BeginGroup();
			{
				// Draw Icon
				{
					ImGui::SameLine();
					ImGui::Text( a_Node.Icon );
				}

				// Draw Enabled Checkbox or Dummy
				{
					ImGui::SameLine();
					const ImGuiID id = ImGui::GetID( "##CheckBox" );

					float checkboxSize = ImGui::GetTextLineHeight() * 0.8f;
					ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2( 0.0f, ( 0.5f * ( ImGui::GetFrameHeight() ) ) - ( 0.5f * checkboxSize ) );
					const ImRect bb( pos, ImVec2( pos.x + checkboxSize, pos.y + checkboxSize ) );

					ImGui::ItemSize( ImVec2( checkboxSize, checkboxSize ), style.FramePadding.y );
					if ( ImGui::ItemAdd( bb, id )
						&& a_Node.IsEnabledTogglable
						&& a_Node.IsEnabled )
					{
						bool isHovered, isHeld;
						bool pressed = ImGui::ButtonBehavior( bb, id, &isHovered, &isHeld );

						if ( pressed )
						{
							*a_Node.IsEnabled = !*a_Node.IsEnabled;
							ImGui::MarkItemEdited( id );
						}

						ImGui::RenderNavHighlight( bb, id );

						// Render
						ImGui::RenderFrame( bb.Min, bb.Max,
							ImGui::GetColorU32( ( isHeld && isHovered ) ? ImGuiCol_FrameBgActive : isHovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg ),
							true, style.FrameRounding );

						if ( *a_Node.IsEnabled )
						{
							const float pad = 4.0f;
							const float checkSz = checkboxSize - pad * 2.0f;
							const ImVec2 checkPos = bb.Min + ImVec2( pad, pad );
							ImGui::RenderCheckMark( ImGui::GetCurrentWindow()->DrawList, checkPos, ImGui::GetColorU32( ImGuiCol_Text ), checkSz );
						}
					}
				}

				// Draw Name
				{
					ImGui::SameLine();
					ImGui::TextUnformatted( a_Node.Name );
				}

				// Draw Options button
				const float smallButtonSize = ImGui::GetTextLineHeight() + 2 * 1.0f;
				if ( a_Node.HasOptionsButton )
				{
					ImGui::SameLine(
						ImGui::GetContentRegionAvail().x
						- smallButtonSize - style.ItemInnerSpacing.x );

					ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2( 0.0f, ( 0.5f * ( ImGui::GetFrameHeight() ) ) - ( 0.5f * smallButtonSize ) );
					ImGui::SetCursorScreenPos( pos );
					if ( Helpers::DrawSmallButton( TE_ICON_ELLIPSIS_VERTICAL )
						&& a_Node.OptionsPressed )
						*a_Node.OptionsPressed = true;
				}

				// Draw Documentation button
				if ( a_Node.DocumentationButton )
				{
					if ( TypeDocumentation* documentation = EditorDocumentation::Get()->GetType( a_Node.DocumentationKey ) )
					{
						ImGui::SameLine(
							ImGui::GetContentRegionAvail().x
							- ( 4 * (smallButtonSize - style.ItemInnerSpacing.x ) ) );

						ImVec2 pos = ImGui::GetCursorScreenPos() + ImVec2( 0.0f, ( 0.5f * ( ImGui::GetFrameHeight() ) ) - ( 0.5f * smallButtonSize ) );
						ImGui::SetCursorScreenPos( pos );
						if ( Helpers::DrawSmallButton( TE_ICON_CIRCLE_QUESTION ) )
						{
							ImGui::OpenPopup( "DocumentationPopup" );
						}

						if ( ImGui::BeginPopup( "DocumentationPopup" ) )
						{
							ImGui::PushFont( ImGui::GetBoldFont() );
							ImGui::Text( a_Node.Name );
							ImGui::PopFont();

							ImGui::Separator();

							DrawDocumentation( documentation );

							ImGui::EndPopup();
						}
					}

				}
			}
			ImGui::EndGroup();
			ImGui::PopID();

			return isOpen;
		}

	}

	// Create a set of blacklisted components that should not be drawn in the inspector
	static const std::unordered_set<entt::id_type> s_BlacklistedComponents =
	{
		entt::hashed_string("Component").value(),
		entt::hashed_string( "TagComponent" ).value(),
		entt::hashed_string( "TransformComponent" ).value(),
		entt::hashed_string( "GUIDComponent" ).value(),
		entt::hashed_string( "NativeScriptComponent" ).value(),
		entt::hashed_string( "GameObjectFlagsComponent" ).value(),
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
		m_OnGameObjectSelectedHandle = Editor::Events::OnGameObjectSelected.Add<&InspectorPanel::SetInspectedGameObject>( this );
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



	void InspectorPanel::DrawInspectedGameObject()
	{
		const ImGuiStyle& style = ImGui::GetStyle();

		// Draw enabled checkbox
		{
			bool enabled = InspectedGameObject.IsEnabled();
			if ( ImGui::Checkbox( "##_Enabled", &enabled ) )
			{
				InspectedGameObject.SetEnabled( enabled );
			}
		}

		ImGui::SameLine( 0.0f, style.ItemInnerSpacing.x );

		// Draw gameobject name as editable text input field
		if ( TagComponent* tagComponent = InspectedGameObject.TryGetComponent<TagComponent>() )
		{
			String tag = InspectedGameObject.GetComponent<TagComponent>().Tag;
			if ( ImGui::InputText( TE_ICON_PENCIL "##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				tagComponent->Tag = std::move(tag);
			}
		}

		const float optionsButtonSize = ImGui::GetTextLineHeight();
		ImGui::SameLine( ImGui::GetContentRegionAvail().x - optionsButtonSize );
		if ( ImGui::Button( TE_ICON_ELLIPSIS_VERTICAL ) )
		{
			ImGui::OpenPopup( "GameObjectOptions" );
		}

		// Draw GameObject documentation button
		ImGui::SameLine( ImGui::GetContentRegionAvail().x - optionsButtonSize - style.ItemInnerSpacing.x - optionsButtonSize - style.ItemInnerSpacing.x );
		if ( ImGui::Button( TE_ICON_CIRCLE_QUESTION ) )
		{
			ImGui::OpenPopup( "GameObjectInfo" );
		}

		if ( ImGui::BeginPopup( "GameObjectInfo" ) )
		{
			ImGui::TextUnformatted( "Game Object" );
			if ( TypeDocumentation* documentation = EditorDocumentation::Get()->GetType( "GameObject" ) )
			{
				Helpers::DrawDocumentation( documentation );
			}

			ImGui::EndPopup();
		}

		if ( ImGui::BeginPopup( "GameObjectOptions" ) )
		{
			{
				bool active = InspectedGameObject.IsActive();
				if ( ImGui::Checkbox( TE_ICON_LIGHTBULB " Active", &active ) )
				{
					InspectedGameObject.SetActive( active );
				}
			}

			{
				bool visible = InspectedGameObject.IsVisible();
				if ( ImGui::Checkbox( TE_ICON_EYE " Visible", &visible ) )
				{
					InspectedGameObject.SetVisible( visible );
				}
			}

			ImGui::Separator();

			{
				ImGui::ScopedStyleCol redText( ImGuiCol_Text, ImVec4( Editor::GetPallete().Red ) );
				if ( ImGui::MenuItem( TE_ICON_TRASH_CAN " Delete" ) )
				{
					Editor::GetCommandManager().Execute( Commands::GameObjectDestroyed{ SceneManager::GetActiveSceneWeak(), InspectedGameObject } );

					InspectedGameObject.Destroy();
					InspectedGameObject = GameObject();
					Editor::Events::OnGameObjectSelected.Broadcast( GameObject() );
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		// Draw all components attached to the GameObject
		DrawComponents( InspectedGameObject );
	}



	void InspectorPanel::DrawComponents( GameObject a_GO )
	{
		ImGuiStyle& style = ImGui::GetStyle();
		const ImVec2 rowSpacing = ImVec2( style.ItemSpacing.x, 1.0f );

		// Draw TransformComponent first
		if ( TransformComponent* tc = a_GO.TryGetComponent<TransformComponent>() )
		{
			if ( Helpers::DrawComponentTreeNode( {
				.Icon = TE_ICON_CUBES, .Name = "Transform",
				.HasOptionsButton = false, .OptionsPressed = nullptr,
				.IsEnabledTogglable = false, .IsEnabled = nullptr, 
				.DocumentationButton = true, .DocumentationKey = "TransformComponent"
				} ) )
			{
				ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, rowSpacing );
				if ( ImGui::BeginTable( "", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable ) )
				{
					const float firstColumnWidth = ImGui::CalcTextSize( "Position   " ).x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
					ImGui::TableSetupColumn( "##Property", ImGuiTableColumnFlags_WidthStretch, firstColumnWidth );

					// Draw Position
					{
						ImGui::TableNextRow();
						ImGui::PopStyleVar();

						ImGui::TableNextColumn();
						ImGui::TextUnformatted( "Position" );
						ImGui::TableNextColumn();

						ImGui::PushID( "p" );
						Editor::DrawProperty( "", tc->Position, EDrawPropertyFlags::Editable );
						ImGui::PopID();
					}

					// Draw Rotation
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, rowSpacing );
						ImGui::TableNextRow();
						ImGui::PopStyleVar();

						ImGui::TableNextColumn();
						ImGui::TextUnformatted( "Rotation" );
						ImGui::TableNextColumn();

						ImGui::PushID( "r" );
						Editor::DrawProperty( "", tc->Rotation, EDrawPropertyFlags::Editable );
						ImGui::PopID();
					}

					// Draw Scale
					{
						ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, rowSpacing );
						ImGui::TableNextRow();
						ImGui::PopStyleVar();

						ImGui::TableNextColumn();
						ImGui::TextUnformatted( "Scale" );

						// Draw Scale lock button
						// Store the lock state in ImGui State storage so it can be saved per GameObject
						bool useScaleLock = false;
						{
							ImGuiStorage* storage = ImGui::GetStateStorage();
							ImGuiID id = ImGui::GetID( reinterpret_cast<const void*>( a_GO.ID() ) );
							useScaleLock = storage->GetBool( id, false );
							ImGui::SameLine();
							ImGui::PushID( "Lock" );
							// Set the colors for the button to the same as a checkbox
							ImGui::PushStyleColor( ImGuiCol_Button, style.Colors[ImGuiCol_FrameBg] );
							ImGui::PushStyleColor( ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_FrameBgHovered] );
							ImGui::PushStyleColor( ImGuiCol_ButtonActive, style.Colors[ImGuiCol_FrameBgActive] );
							if ( ImGui::Button( ( useScaleLock ? TE_ICON_LOCK : "" ), ImVec2( ImGui::GetFrameHeight(), ImGui::GetFrameHeight() ) ) )
							{
								useScaleLock = !useScaleLock;
								storage->SetBool( id, useScaleLock );
							}
							ImGui::PopStyleColor( 3 );
							ImGui::PopID();

							if ( ImGui::BeginItemTooltip() )
							{
								ImGui::TextUnformatted( "Lock Proportions" );
								ImGui::EndTooltip();
							}
						}

						ImGui::TableNextColumn();
						{
							Vector3 scale = tc->Scale;
							const Vector3 oldScale = scale;

							ImGui::PushID( "s" );
							if ( Editor::DrawProperty( "", scale, EDrawPropertyFlags::Editable ) )
							{
								if ( useScaleLock )
								{
									// Find the value that was changed
									float changedValue = 0.0f;
									if ( scale.x != oldScale.x )
										changedValue = scale.x;
									else if ( scale.y != oldScale.y )
										changedValue = scale.y;
									else if ( scale.z != oldScale.z )
										changedValue = scale.z;

									// Calculate the proportional change based on the changed axis
									if ( scale.x != oldScale.x )
									{
										// Lock the proportions by scaling the other axes
										float proportion = oldScale.x == 0.0f ? 1.0f : scale.x / oldScale.x;
										scale.y = oldScale.y * proportion;
										scale.z = oldScale.z * proportion;
									}
									else if ( scale.y != oldScale.y )
									{
										// Lock the proportions by scaling the other axes
										float proportion = oldScale.y == 0.0f ? 1.0f : scale.y / oldScale.y;
										scale.x = oldScale.x * proportion;
										scale.z = oldScale.z * proportion;
									}
									else if ( scale.z != oldScale.z )
									{
										// Lock the proportions by scaling the other axes
										float proportion = oldScale.z == 0.0f ? 1.0f : scale.z / oldScale.z;
										scale.x = oldScale.x * proportion;
										scale.y = oldScale.y * proportion;
									}
								}
							}
							ImGui::PopID();

							tc->Scale = scale;
						}
					}


					ImGui::EndTable();
				}
				else
				{
					ImGui::PopStyleVar();
				}

				ImGui::TreePop();
			}
		}

		auto components = a_GO.GetAllComponents();
		for ( auto& [metaType, component] : components )
		{
			if ( s_BlacklistedComponents.contains( metaType.ID() ) )
				continue;

			NativeScriptComponent* asNative = metaType.IsBaseOf( Refl::ResolveMetaType<NativeScriptComponent>() ) ? static_cast<NativeScriptComponent*>( component ) : nullptr;

			const char* icon = s_ComponentIcons.contains( metaType.ID() ) ? s_ComponentIcons.at( metaType.ID() ) : TE_ICON_GEARS;
			const char* className = Helpers::GetCleanClassName( metaType.GetCleanTypeName() );

			{
				constexpr EComponentFlags enabledFlag = EComponentFlags::Active | EComponentFlags::Visible;

				bool hasOptionsButton = true;
				bool isOptionsPressed = false;
				bool isActiveToggleable = asNative != nullptr;
				const bool isComponentActive = asNative ? asNative->Flags().HasFlag( enabledFlag ) : true;
				bool componentActive = isComponentActive;

				ComponentTreeNode node = {
					.Icon = icon, .Name = className,
						.HasOptionsButton = hasOptionsButton, .OptionsPressed = &isOptionsPressed,
						.IsEnabledTogglable = isActiveToggleable, .IsEnabled = &componentActive,
						.DocumentationButton = true, .DocumentationKey = metaType.GetCleanTypeName()
				};
				bool isOpen = Helpers::DrawComponentTreeNode( node );
				if ( asNative && componentActive != isComponentActive )
				{
					asNative->SetVisible( componentActive );
					asNative->SetActive( componentActive );
				}

				if ( isOptionsPressed )
					ImGui::OpenPopup( className );

				// Component options popup
				if ( ImGui::BeginPopup( className ) )
				{
					ImGui::PushStyleColor( ImGuiCol_Separator, ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ) );

					if ( asNative )
					{
						ImGui::PushFont( ImGui::GetBoldFont() );
						ImGui::SeparatorText( "Native Script Component" );
						ImGui::PopFont();

						// Draw active checkbox
						{
							bool active = asNative->Flags().HasFlag( EComponentFlags::Active );
							if ( ImGui::Checkbox( TE_ICON_LIGHTBULB " Active", &active ) )
							{
								asNative->SetActive( active );
							}
						}

						// Draw visible checkbox
						{
							bool visible = asNative->HasFlag( EComponentFlags::Visible );
							if ( ImGui::Checkbox( TE_ICON_EYE " Visible", &visible ) )
							{
								asNative->SetVisible( visible );
							}
						}

						ImGui::Separator();
					}

					// Draw remove component button
					{
						ImGui::ScopedStyleCol redText( ImGuiCol_Text, ImVec4( Editor::GetPallete().Red ) );
						if ( ImGui::MenuItem( TE_ICON_TRASH_CAN " Remove Component" ) )
						{
							auto removeFromGameObjectFunc = metaType.GetMetaAttribute<Refl::Props::RemoveFromGameObjectProp::Type>( Refl::Props::RemoveFromGameObjectProp::ID );
							if ( ASSERT( removeFromGameObjectFunc.has_value() ) )
								removeFromGameObjectFunc.value()( *SceneManager::GetActiveScene(), a_GO );
						}
					}

					ImGui::PopStyleColor();

					ImGui::EndPopup();
				}

				if ( !isOpen )
					continue;
			}

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

				const char* className = Helpers::GetCleanClassName( metaType.GetCleanTypeName() );
				if ( !ImGui::MenuItem( className ) )
					continue;

				auto addToGameObjectFunc = metaType.GetMetaAttribute<Refl::Props::AddToGameObjectProp::Type>( Refl::Props::AddToGameObjectProp::ID );
				if ( ASSERT( addToGameObjectFunc.has_value() ) )
					addToGameObjectFunc.value()( *SceneManager::GetActiveScene(), InspectedGameObject );

				break;
			}

			ImGui::EndMenu();
		}
	}

}

#endif