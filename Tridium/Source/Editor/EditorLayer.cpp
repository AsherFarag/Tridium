#include "tripch.h"

#ifdef IS_EDITOR
#include "EditorLayer.h"
#include "imgui.h"

#include "Editor.h"
#include "EditorCamera.h"

#include <Tridium/Scripting/ScriptEngine.h>
#include <Tridium/ECS/Components/Types.h>

#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ScriptEditorPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GameViewportPanel.h"
#include "Panels/MaterialEditorPanel.h"

#include <Tridium/IO/SceneSerializer.h>

namespace Tridium::Editor {

	// TEMP
	class Stats : public Panel
	{
	public:
		Stats() : Panel( "Stats" ) {}

		virtual void OnImGuiDraw()
		{
			if ( ImGuiBegin() )
			{
				ImGui::Text( "FPS: %i", Application::Get().GetFPS() );
			}

			ImGuiEnd();
		}
	};

	EditorLayer::EditorLayer()
		: Layer( "EditorLayer")
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = MakeRef<EditorCamera>();

		m_PanelStack.PushPanel<InspectorPanel>();
		m_SceneHeirarchy = m_PanelStack.PushPanel<SceneHeirarchyPanel>();
		m_ContentBrowser = m_PanelStack.PushPanel<ContentBrowserPanel>();
		m_GameViewportPanel = m_PanelStack.PushPanel<GameViewportPanel>();
		m_EditorViewportPanel = m_PanelStack.PushPanel<EditorViewportPanel>( m_EditorCamera );
		m_PanelStack.PushPanel<Stats>();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate()
	{
		if ( m_ActiveScene )
		{
			switch ( CurrentSceneState )
			{
			case SceneState::Edit:
			{
				m_EditorCamera->OnUpdate();
				break;
			}
			case SceneState::Play:
			{
				m_EditorCamera->OnUpdate();

				if ( !m_ActiveScene->IsPaused() )
				{
					m_ActiveScene->OnUpdate();
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
		ImGui::Begin( Application::GetActiveProject()->GetName().c_str(), nullptr, window_flags );
		ImGui::PopStyleVar();

		if ( opt_Fullscreen )
			ImGui::PopStyleVar( 2 );

		// Init Dock Space
		static const ImGuiID dockspace_id = ImGui::GetID( "EditorDockSpace" );
		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );

		DrawMenuBar();
		m_UIToolBar.OnImGuiDraw();

		for ( auto& it = m_PanelStack.rbegin(); it != m_PanelStack.rend(); it++ )
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
		m_ActiveScene->OnBegin();
		m_ActiveScene->SetPaused( false );


		m_GameViewportPanel->Focus();
		CurrentSceneState = SceneState::Play;
	}

	void EditorLayer::OnEndScene()
	{
		m_ActiveScene->OnEnd();

		CurrentSceneState = SceneState::Edit;
		m_EditorViewportPanel->Focus();
		LoadScene( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );
	}

	bool EditorLayer::LoadScene( const std::string& filepath )
	{
		m_ActiveScene->Clear();

		SceneSerializer serializer( m_ActiveScene );
		serializer.DeserializeText( filepath );
		return true;
	}

	bool EditorLayer::SaveScene( const std::string& filepath )
	{
		SceneSerializer serializer( m_ActiveScene );
		serializer.SerializeText( filepath );
		return true;
	}

	bool EditorLayer::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );
		bool alt = Input::IsKeyPressed( Input::KEY_LEFT_ALT );

		switch ( e.GetKeyCode() )
		{
		case Input::KEY_S:
		{
			if ( control )
			{
				if ( m_ActiveScene )
					SaveScene( m_ActiveScene->GetPath() );

				Util::SaveAll();
				return true;
			}
			break;
		}
		case Input::KEY_R:
		{
			if ( control )
			{
				ScriptEngine::Recompile();
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
			if ( ImGui::MenuItem( "New Project", nullptr, nullptr, false ) )
				TE_CORE_INFO( "New Project" );

			if ( ImGui::MenuItem( "Open Project", nullptr, nullptr, false ) )
				TE_CORE_INFO( "Open Project" );

			if ( ImGui::MenuItem( "Save Project", nullptr, nullptr, false ) )
				TE_CORE_INFO( "Save Project" );

			ImGui::Separator();

			// Scene
			if ( ImGui::MenuItem( "New Scene", nullptr, nullptr, false ) )
				TE_CORE_INFO( "New Scene" );

			if ( ImGui::MenuItem( "Open Scene", nullptr, nullptr, false ) )
				TE_CORE_INFO( "Open Scene" );

			if ( ImGui::MenuItem( "Save Scene", "Ctrl + S" ) )
				SaveScene( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Editor Preferences" ) )
				m_PanelStack.PushPanel<EditorPreferencesPanel>();

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::BeginMenu( "Panels" ) )
			{
				if ( ImGui::MenuItem( "Content Browser" ) ) m_PanelStack.PushPanel<ContentBrowserPanel>();
				if ( ImGui::MenuItem( "Stats" ) ) m_PanelStack.PushPanel<Stats>();

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Open Script Editor" ) )
				m_PanelStack.PushPanel<ScriptEditorPanel>();

			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
				ScriptEngine::Recompile();

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Other" ) )
		{
			if ( ImGui::MenuItem( "Recompile Shaders" ) )
				ShaderLibrary::RecompileAll();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

#pragma region - UIToolBar -

	UIToolBar::UIToolBar()
	{
		fs::path iconFolder( "Content/Engine/Editor/Icons" );

		PlayButtonIcon = TextureLoader::Import( ( iconFolder / "PlayButton.png" ).string() );
		StopButtonIcon = TextureLoader::Import( ( iconFolder / "StopButton.png" ).string() );
		PauseButtonIcon = TextureLoader::Import( ( iconFolder / "PauseButton.png" ).string() );
	}

	void UIToolBar::OnImGuiDraw()
	{
		float winPaddingY = 5.0f;
		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_WindowPadding, { 0, winPaddingY } );

		ImGui::Begin( "##UIToolBar", nullptr,
			ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoTitleBar );

		ImVec2 buttonPadding( 4, 4 );
		float regionAvailY = MAX( 0.0f, ImGui::GetContentRegionAvail().y - 5 - buttonPadding.y );
		ImVec2 buttonSize( regionAvailY, regionAvailY );

		EditorLayer* editor = GetEditorLayer();
		SceneState sceneState = editor->CurrentSceneState;
		bool hasPlayButton = ( sceneState == SceneState::Edit ) || ( sceneState == SceneState::Play && editor->GetActiveScene()->IsPaused() );
		bool hasPauseButton = ( sceneState == SceneState::Play ) && ( !editor->GetActiveScene()->IsPaused() );
		bool hasStopButton = sceneState == SceneState::Play;

		float totalButtonSizeX = buttonSize.x + ( buttonPadding.x * 2.f ) + ImGui::GetStyle().ItemSpacing.x;
		float groupSizeX = ( totalButtonSizeX * hasPlayButton ) + ( totalButtonSizeX * hasPauseButton ) + ( totalButtonSizeX * hasStopButton );
		ImGui::SetCursorPosX( ( ImGui::GetWindowWidth() * 0.5f ) - groupSizeX * 0.5f );

		ImGui::BeginGroup();
		{
			ImGui::ScopedStyleVar padding( ImGuiStyleVar_FramePadding, buttonPadding );

			if ( hasPlayButton )
			{
				if ( ImGui::ImageButton( "PlayButton", (ImTextureID)PlayButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 }, { 0.5f, 1.0f, 0.5f, 1.0f } ) )
				{
					editor->OnBeginScene();
				}
			}

			if ( hasPauseButton )
			{
				if ( ImGui::ImageButton( "PauseButton", (ImTextureID)PauseButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 } ) )
				{
					editor->GetActiveScene()->SetPaused( true );
				}
			}

			ImGui::SameLine();

			if ( hasStopButton )
			{
				if ( ImGui::ImageButton( "StopButton", (ImTextureID)StopButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 }, { 1.0f, 0.5f, 0.5f, 1.0f } ) )
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