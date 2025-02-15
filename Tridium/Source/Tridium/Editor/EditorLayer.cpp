#include "tripch.h"

#if IS_EDITOR
#include "EditorLayer.h"
#include "imgui.h"

#include "Editor.h"
#include "EditorCamera.h"
#include "EditorUtil.h"
#include "EditorStyle.h"

#include <Tridium/Scripting/ScriptEngine.h>
#include <Tridium/ECS/Components/Types.h>

#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ScriptEditorPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GameViewportPanel.h"
#include "Panels/Asset/MaterialEditorPanel.h"
#include "Panels/AssetRegistryPanel.h"
#include "Panels/SceneSettingsPanel.h"
#include "Panels/ProjectSettingsPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/StatsPanel.h"
#include "Panels/ProfilerPanel.h"

#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Asset/EditorAssetManager.h>

#include <fstream>
#include <Tridium/IO/Serializer.h>

namespace Tridium {

	EditorLayer::EditorLayer()
		: Layer( "EditorLayer")
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = MakeShared<EditorCamera>();

		m_PanelStack.PushPanel<InspectorPanel>();
		m_SceneHeirarchy = m_PanelStack.PushPanel<SceneHeirarchyPanel>();
		m_ContentBrowser = m_PanelStack.PushPanel<ContentBrowserPanel>();
		m_GameViewportPanel = m_PanelStack.PushPanel<GameViewportPanel>();
		m_EditorViewportPanel = m_PanelStack.PushPanel<EditorViewportPanel>( m_EditorCamera );
		m_PanelStack.PushPanel<StatsPanel>();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate()
	{
		if ( Scene* scene = SceneManager::GetActiveScene() )
		{
			switch ( CurrentSceneState )
			{
			case ESceneState::Edit:
			{
				m_EditorCamera->OnUpdate();
				break;
			}
			case ESceneState::Play:
			{
				m_EditorCamera->OnUpdate();

				if ( !scene->IsPaused() )
				{
					scene->OnUpdate();
				}
				break;
			}
			}
		}
	}

	void EditorLayer::OnImGuiDraw()
	{
		static bool opt_Fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration;
		if ( opt_Fullscreen )
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos( viewport->WorkPos );
			ImGui::SetNextWindowSize( viewport->WorkSize );
			ImGui::SetNextWindowViewport( viewport->ID );

			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

			window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		ImGui::Begin( Engine::Get()->GetActiveProject().Config.Name.c_str(), nullptr, window_flags);
		ImGui::PopStyleVar();

		if ( opt_Fullscreen )
			ImGui::PopStyleVar( 2 );

		// Init Dock Space
		static const ImGuiID dockspace_id = ImGui::GetID( "EditorDockSpace" );
		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );

		DrawMenuBar();
		m_UIToolBar.OnImGuiDraw();

		for ( auto it = m_PanelStack.rbegin(); it != m_PanelStack.rend(); it++ )
		{
			it->second->OnImGuiDraw();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent( Event& e )
	{
		for ( auto it = m_PanelStack.end(); it != m_PanelStack.begin(); )
		{
			( *--it ).second->OnEvent( e );
			if ( e.Handled )
				break;
		}

		if ( e.Handled )
			return;

		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<KeyPressedEvent>( TE_BIND_EVENT_FN( EditorLayer::OnKeyPressed, 1 ) );
	}

	void EditorLayer::OnBeginScene()
	{
		Scene* scene = SceneManager::GetActiveScene();
		// Store a copy of the current scene in storage
		m_SceneSnapshot = MakeShared<Scene>( *scene );

		scene->OnBeginPlay();
		scene->SetPaused( false );

		m_GameViewportPanel->Focus();
		CurrentSceneState = ESceneState::Play;

		Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );
	}

	void EditorLayer::OnEndScene()
	{
		SceneManager::GetActiveScene()->OnEndPlay();

		CurrentSceneState = ESceneState::Edit;
		m_EditorViewportPanel->Focus();

		// Restore the scene from storage
		if ( m_SceneSnapshot )
		{
			SceneManager::SetActiveScene( m_SceneSnapshot.get() );
			m_SceneSnapshot.reset();
		}

		TODO( "TEMP?" );
		Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Normal );
	}

	bool EditorLayer::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( EInputKey::LeftControl );
		bool alt = Input::IsKeyPressed( EInputKey::LeftAlt );

		switch ( e.GetKeyCode() )
		{
		case EInputKey::S:
		{
			if ( control )
			{
				if ( SceneManager::GetActiveScene() )
				{
					//if ( m_ActiveScene->GetPath().length() == 0 )
					//	Util::OpenSaveFileDialog( "Untitled.tscene", [this](const std::string& path) { SaveScene(path); });
					//else
					//	SaveScene( m_ActiveScene->GetPath() );
				}

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
			if ( CurrentSceneState == ESceneState::Play )
			{
				OnEndScene();
				return true;
			}
			break;
		}
		case EInputKey::Tab:
		{
			if ( CurrentSceneState == ESceneState::Play )
			{
				TODO( "Bruh" );
				static bool s_MouseIsCaptured = false;
				if ( s_MouseIsCaptured )
				{
					Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Normal );
					ImGui::SetMouseCursor( ImGuiMouseCursor_Arrow );
					s_MouseIsCaptured = false;
				}
				else
				{
					Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );
					ImGui::SetMouseCursor( ImGuiMouseCursor_None );
					s_MouseIsCaptured = true;
				}
				return true;
			}
		}
		// Redo
		case EInputKey::Y:
		{
			if ( control )
			{
				Editor::GetCommandManager().Redo();
				return true;
			}
			break;
		}
		// Undo
		case EInputKey::Z:
		{
			if ( control )
			{
				Editor::GetCommandManager().Undo();
				return true;
			}
			break;
		}
		}

		return false;
	}

	void EditorLayer::DrawMenuBar()
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
				Util::OpenNewFileDialog(
					"Scene",
					"Untitled.tscene",
					[this]( const std::string& path )
					{
						auto assetManager = AssetManager::Get<EditorAssetManager>();
						AssetMetaData metaData =
						{
								AssetHandle::Create(),
								EAssetType::Scene,
								path,
								FilePath( path ).GetFilenameWithoutExtension(),
								true
						};

						TODO( "REplace this with a scene manager function" );
						SharedPtr<Scene> scene = MakeShared<Scene>();
						scene->SetName( metaData.Name );
						if ( assetManager->CreateAsset( metaData, scene ) )
							SceneManager::SetActiveScene( scene.get() );
						else
							LOG( LogCategory::Editor, Error, "Failed to create scene '{0}'", path );
					} );
			}
			if ( ImGui::MenuItem( TE_ICON_FOLDER "Open Scene" ) )
			{
				Util::OpenLoadFileDialog( "",
					[this](const std::string& path) 
					{
						auto assetManager = AssetManager::Get<EditorAssetManager>();
						const AssetMetaData& sceneMetaData = assetManager->GetAssetMetaData( path );
						if ( auto scene = AssetManager::GetAsset<Scene>( sceneMetaData.Handle ) )
							SceneManager::SetActiveScene( scene.get() );
					});
			}
			if ( ImGui::MenuItem( TE_ICON_FLOPPY_DISK "Save Scene", "Ctrl + S" ) )
			{
				if ( Scene* scene = SceneManager::GetActiveScene() )
				{
					auto assetManager = AssetManager::Get<EditorAssetManager>();
					if ( !assetManager->SaveAsset( scene->GetHandle() ) )
					{
						Util::OpenSaveFileDialog( scene->GetName() + ".tscene",
							[this]( const std::string& path )
							{
								auto assetManager = AssetManager::Get<EditorAssetManager>();
								AssetMetaData metaData =
								{
									SceneManager::GetActiveScene()->GetHandle(),
									EAssetType::Scene,
									path,
									FilePath( path ).GetFilenameWithoutExtension(),
									true
								};
								TODO( "Gross, fix this!" );
								if ( assetManager->CreateAsset( metaData, SceneManager::GetActiveScene()->shared_from_this() ) )
									assetManager->SaveAsset( metaData.Handle );
								else
									LOG( LogCategory::Editor, Error, "Failed to save scene '{0}'", path );
							} );
					}
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Editor Preferences" ) )
				m_PanelStack.PushPanel<EditorPreferencesPanel>();

			if ( ImGui::MenuItem( "Project Settings" ) )
				m_PanelStack.PushPanel<ProjectSettingsPanel>();

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::BeginMenu( "Panels" ) )
			{
				if ( ImGui::MenuItem( "Content Browser" ) ) m_PanelStack.PushPanel<ContentBrowserPanel>();
				if ( ImGui::MenuItem( "Stats" ) ) m_PanelStack.PushPanel<StatsPanel>();
				if ( ImGui::MenuItem( "Asset Registry" ) ) m_PanelStack.PushPanel<AssetRegistryPanel>();
				if ( ImGui::MenuItem( "Scene Renderer" ) ) m_PanelStack.PushPanel<SceneRendererPanel>();
				if ( ImGui::MenuItem( "Script Editor" ) ) m_PanelStack.PushPanel<ScriptEditorPanel>();
				if ( ImGui::MenuItem( "Profiler" ) ) m_PanelStack.PushPanel<ProfilerPanel>();

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Open Script Editor" ) )
				m_PanelStack.PushPanel<ScriptEditorPanel>();

			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
			{
				ScriptEngine::Get()->RecompileAllScripts();
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
			const char* projectName = Engine::Get()->GetActiveProject().Config.Name.c_str();
			const float paddingFromRight = 10.0f;
			ImGui::SameLine( ImGui::GetContentRegionMax().x - ImGui::CalcTextSize( projectName ).x - paddingFromRight );

			ImGui::Separator();

			ImGui::PushFont( ImGui::GetExtraBoldFont() );
			ImGui::Text( projectName );
			ImGui::PopFont();
		}

		ImGui::EndMainMenuBar();
	}

#pragma region - UIToolBar -

	UIToolBar::UIToolBar()
	{
		FilePath iconFolder( Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons" );

		PlayButtonIcon = TextureLoader::LoadTexture( iconFolder / "PlayButton.png" );
		StopButtonIcon = TextureLoader::LoadTexture( iconFolder / "StopButton.png" );
		PauseButtonIcon = TextureLoader::LoadTexture( iconFolder / "PauseButton.png" );
	}

	void UIToolBar::OnImGuiDraw()
	{
		Scene* scene = SceneManager::GetActiveScene();
		if ( !scene )
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

		EditorLayer* editor = Editor::GetEditorLayer();
		ESceneState sceneState = editor->CurrentSceneState;
		bool hasPlayButton = ( sceneState == ESceneState::Edit ) || ( sceneState == ESceneState::Play && scene->IsPaused() );
		bool hasPauseButton = ( sceneState == ESceneState::Play ) && ( !scene->IsPaused() );
		bool hasStopButton = sceneState == ESceneState::Play;

		float totalButtonSizeX = buttonSize.x + ( buttonPadding.x * 2.f ) + ImGui::GetStyle().ItemSpacing.x;
		float groupSizeX = ( totalButtonSizeX * hasPlayButton ) + ( totalButtonSizeX * hasPauseButton ) + ( totalButtonSizeX * hasStopButton );
		ImGui::SetCursorPosX( ( ImGui::GetWindowWidth() * 0.5f ) - groupSizeX * 0.5f );
		ImGui::SetCursorPosY( ImGui::GetWindowHeight() * 0.5f - buttonSize.y * 0.5f );

		ImGui::BeginGroup();
		{
			ImGui::ScopedStyleVar padding( ImGuiStyleVar_FramePadding, buttonPadding );
			if ( hasPlayButton )
			{
				ImGui::ScopedStyleCol buttonCol( ImGuiCol_Text, ImVec4( Editor::GetPallete().Green ) );
				if ( ImGui::IconButton( TE_ICON_PLAY ) )
				{
					if ( scene->IsPaused() )
						scene->SetPaused( false );
					else
						editor->OnBeginScene();
				}
			}

			if ( hasPauseButton )
			{
				if ( ImGui::IconButton( TE_ICON_PAUSE ) )
				{
					SceneManager::GetActiveScene()->SetPaused( true );
				}
			}

			ImGui::SameLine();

			if ( hasStopButton )
			{
				ImGui::ScopedStyleCol buttonCol( ImGuiCol_Text, ImVec4( Editor::GetPallete().Red ) );
				if ( ImGui::IconButton( TE_ICON_STOP ) )
				{
					editor->OnEndScene();
				}
			}

		}
		ImGui::EndGroup();

		ImGui::End();
	}

#pragma endregion

}

#endif // IS_EDITOR