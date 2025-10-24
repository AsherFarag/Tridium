#include "tripch.h"
#include "Editor.h"

#if WITH_EDITOR

#include <Tridium/Engine/Engine.h>

// Panels
#include <Tridium/Editor/UI/EditorViewportPanel.h>
#include <Tridium/Editor/UI/InspectorPanel.h>
#include <Tridium/Editor/UI/SceneHierarchyPanel.h>

namespace Tridium {

	REGISTER_TICK_GROUP( EditorTick, "BeginAppUpdate"_H );
	REGISTER_TICK_GROUP( EditorRender, "Render"_H );

	Editor* Editor::s_Instance = nullptr;
	decltype( Editor::Events::OnGameObjectSelected ) Editor::Events::OnGameObjectSelected{};

	Editor::Editor() : Layer( "Editor" )
	{
		ENSURE( !s_Instance, "Editor instance already exists!" );

		s_Instance = this;

		// Set Window title and icon
		Application::GetWindow().SetTitle("Tridium Editor");
		Application::GetWindow().SetIcon( ( Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" ).ToString() );

		m_Style.SetTheme( EditorStyle::ETheme::Midnight );

		Application::AddOnTick( TickGroups::EditorTick, []() { Editor::Get()->Tick(); } );
		Application::AddOnTick( TickGroups::DrawUI, []() { Editor::Get()->DrawUI(); } );

		// TEMP!
		SceneManager::SetActiveScene( MakeShared<Scene>() );
	}

	Editor::~Editor()
	{
		s_Instance = nullptr;
	}

	void Editor::Tick()
	{
		// TEMP!
		SceneManager::ActiveScene()->OnTick( 0.016f );

		TODO( "Delta Time" );
		m_UIManager.UpdateUI( 0.0f );
	}

	void Editor::DrawUI()
	{
		UI_DrawMenuBar();
		UI_DrawToolBar();

		m_UIManager.DrawUI();
	}

	void Editor::OnAttach()
	{
		// Panels
		m_UIManager.CreatePanel<EditorViewportPanel>( TE_ICON_TV " Editor", true );
		m_UIManager.CreatePanel<SceneHierarchyPanel>( TE_ICON_MOUNTAIN_SUN " Hierarchy", true );
		m_UIManager.CreatePanel<InspectorPanel>( TE_ICON_MAGNIFYING_GLASS " Inspector", true );
	}

	void Editor::OnDetach()
	{
	}

	void Editor::OnEvent( Event& a_Event )
	{
		m_UIManager.OnEvent( a_Event );

		if ( a_Event.Handled )
			return;

		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<KeyPressedEvent>( [this]( const KeyPressedEvent& a_Event ) -> bool { return Event_KeyPressed( a_Event ); } );
	}

	void Editor::UI_DrawMenuBar()
	{
		if ( !ImGui::BeginMainMenuBar() )
			return;

		if ( ImGui::BeginMenu( "File" ) )
		{
			// Project
			if ( ImGui::MenuItem( TE_ICON_FILE "New Project", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "New Project" );

			if ( ImGui::MenuItem( TE_ICON_FOLDER "Open Project", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "Open Project" );

			if ( ImGui::MenuItem( TE_ICON_FLOPPY_DISK "Save Project", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "Save Project" );

			ImGui::Separator();

			// Scene
			if ( ImGui::MenuItem( TE_ICON_FILE "New Scene" ) )
			{
			}

			if ( ImGui::MenuItem( TE_ICON_FOLDER "Open Scene" ) )
			{
			}

			if ( ImGui::MenuItem( TE_ICON_FLOPPY_DISK "Save Scene", "Ctrl + S" ) )
			{
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Editor Preferences" ) )
			{

			}

			if ( ImGui::MenuItem( "Project Settings" ) )
			{

			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::BeginMenu( "Panels" ) )
			{
				if ( ImGui::MenuItem( "Content Browser" ) ) {}
				if ( ImGui::MenuItem( "Stats" ) ) {}
				if ( ImGui::MenuItem( "Asset Registry" ) ) {}
				if ( ImGui::MenuItem( "Scene Renderer" ) ) {}
				if ( ImGui::MenuItem( "Script Editor" ) ) {}
				if ( ImGui::MenuItem( "Profiler" ) ) {};

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Open Script Editor" ) )
			{
			}

			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
			{
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Other" ) )
		{
			if ( ImGui::MenuItem( "Recompile Shaders" ) ) {}

			ImGui::EndMenu();
		}

		// Project Name
		{
			const char* projectName = Engine::ActiveProject().Config().General.Name.c_str();
			const float paddingFromRight = 10.0f;
			ImGui::SameLine( ImGui::GetContentRegionMax().x - ImGui::CalcTextSize( projectName ).x - paddingFromRight );

			ImGui::Separator();

			ImGui::PushFont( ImGui::GetExtraBoldFont() );
			ImGui::Text( projectName );
			ImGui::PopFont();
		}

		ImGui::EndMainMenuBar();
	}

	void Editor::UI_DrawToolBar()
	{
		if ( true )
			return;

		float winPaddingY = 5.0f;
		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_WindowPadding, { 0, winPaddingY } );

		ImGui::Begin( "##UIToolBar", nullptr,
			ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoTitleBar );

		const ImVec2 buttonPadding( 0, 0 );
		const float textSize = ImGui::GetTextLineHeight();
		const ImVec2 buttonSize( textSize, textSize );

		TODO( "This " );
		EScenePlayMode sceneState = EScenePlayMode::None;
		bool hasPlayButton = ( sceneState == EScenePlayMode::None ) || ( sceneState == EScenePlayMode::Play /*&& scene->IsPaused()*/ );
		//bool hasPauseButton = ( sceneState == EScenePlayMode::Play ) && (/* !scene->IsPaused()*/ );
		bool hasStopButton = sceneState == EScenePlayMode::Play;

		float totalButtonSizeX = buttonSize.x + ( buttonPadding.x * 2.f ) + ImGui::GetStyle().ItemSpacing.x;
		float groupSizeX = ( totalButtonSizeX * hasPlayButton ) + ( totalButtonSizeX /** hasPauseButton*/ ) + ( totalButtonSizeX * hasStopButton );
		ImGui::SetCursorPosX( ( ImGui::GetWindowWidth() * 0.5f ) - groupSizeX * 0.5f );
		ImGui::SetCursorPosY( ImGui::GetWindowHeight() * 0.5f - buttonSize.y * 0.5f );

		ImGui::BeginGroup();
		{
			ImGui::ScopedStyleVar padding( ImGuiStyleVar_FramePadding, buttonPadding );
			if ( hasPlayButton )
			{
				ImGui::ScopedStyleCol buttonCol( ImGuiCol_Text, ImVec4( Editor::GetStyle().Colors.Green ) );
				if ( ImGui::IconButton( TE_ICON_PLAY ) )
				{
				}
			}

			if ( /*hasPauseButton*/ false )
			{
				if ( ImGui::IconButton( TE_ICON_PAUSE ) )
				{
				}
			}

			ImGui::SameLine();

			if ( hasStopButton )
			{
				ImGui::ScopedStyleCol buttonCol( ImGuiCol_Text, ImVec4( Editor::GetStyle().Colors.Red ) );
				if ( ImGui::IconButton( TE_ICON_STOP ) )
				{
				}
			}

		}
		ImGui::EndGroup();

		ImGui::End();
	}

	bool Editor::Event_KeyPressed( const KeyPressedEvent& a_Event )
	{
		if ( a_Event.IsRepeat )
			return false;

		bool control = Input::IsKeyPressed( EInputKey::LeftControl );
		bool alt = Input::IsKeyPressed( EInputKey::LeftAlt );

		switch ( a_Event.KeyCode )
		{
			case EInputKey::S:
			{
				if ( control )
				{

					return true;
				}
				break;
			}
			case EInputKey::R:
			{
				if ( control )
				{
					return true;
				}
				break;
			}
			case EInputKey::Escape:
			{
				break;
			}
			case EInputKey::Tab:
			{
			}
			// Redo
			case EInputKey::Y:
			{
				if ( control )
				{
					m_CommandManager.Redo();
					return true;
				}
				break;
			}
			// Undo
			case EInputKey::Z:
			{
				if ( control )
				{
					m_CommandManager.Undo();
					return true;
				}
				break;
			}
		}

		return false;
	}

}

#endif // WITH_EDITOR
