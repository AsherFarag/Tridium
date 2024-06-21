#include "tripch.h"

#ifdef IS_EDITOR
#include "EditorLayer.h"
#include "imgui.h"
#include "Panels/EditorPreferences.h"
#include <Tridium/Scripting/ScriptEngine.h>
#include <Tridium/ECS/Components/Types.h>

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

	EditorLayer* EditorLayer::s_Instance = nullptr;

	EditorLayer::EditorLayer( const std::string& name )
		: Layer( name )
	{
		TE_CORE_ASSERT( s_Instance == nullptr, "An instance of the editor layer already exists!" );
		s_Instance = this;
	}

	EditorLayer::~EditorLayer()
	{
		s_Instance = nullptr;
	}

	void EditorLayer::OnAttach()
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_EditorCameraFBO = Framebuffer::Create( FBOspecification );

		m_SceneHeirarchy = m_PanelStack.PushPanel<SceneHeirarchy>();
		m_ContentBrowser = m_PanelStack.PushPanel<ContentBrowser>();
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
					m_EditorCamera.OnUpdate();

					m_EditorCameraFBO->Bind();
					m_ActiveScene->Render( m_EditorCamera, m_EditorCamera.GetViewMatrix() );
					m_EditorCameraFBO->Unbind();
					break;
				}
				case SceneState::Play:
				{
					if ( !m_ActiveScene->IsPaused() )
					{
						m_ActiveScene->OnUpdate();
					}

					CameraComponent* mainCam = m_ActiveScene->GetMainCamera();
					if ( mainCam )
					{
						m_EditorCameraFBO->Bind();
						mainCam->SceneCamera.SetViewportSize( m_ViewportSize.x, m_ViewportSize.y );
						m_ActiveScene->Render( *mainCam );
						m_EditorCameraFBO->Unbind();
					}
					else
					{
						m_EditorCameraFBO->Bind();
						m_ActiveScene->Render( m_EditorCamera, m_EditorCamera.GetViewMatrix() );
						m_EditorCameraFBO->Unbind();
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

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
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
		ImGui::Begin( Application::GetActiveProject().GetName().c_str(), nullptr, window_flags);
		ImGui::PopStyleVar();

			if ( opt_Fullscreen )
				ImGui::PopStyleVar( 2 );

			// Init Dock Space
			static const ImGuiID dockspace_id = ImGui::GetID( "EditorDockSpace" );
			ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );

			DrawMenuBar();
			m_UIToolBar.OnImGuiDraw();
			DrawEditorCameraViewPort();

			for ( auto& it = m_PanelStack.rbegin(); it != m_PanelStack.rend(); it++ )
			{
				it->second->OnImGuiDraw();
			}

		ImGui::End();
	}

	void EditorLayer::OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<KeyPressedEvent>( TE_BIND_EVENT_FN( EditorLayer::OnKeyPressed, std::placeholders::_1 ) );

		for ( auto it = m_PanelStack.end(); it != m_PanelStack.begin(); )
		{
			( *--it ).second->OnEvent( e );
			if ( e.Handled )
				break;
		}
	}

	void EditorLayer::OnBeginScene()
	{
		m_ActiveScene->OnBegin();
		m_ActiveScene->SetPaused( false );

		CurrentSceneState = SceneState::Play;

		//Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );
	}

	void EditorLayer::OnEndScene()
	{
		m_ActiveScene->OnEnd();

		CurrentSceneState = SceneState::Edit;

		//Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Normal );
	}

	bool EditorLayer::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );
		bool alt = Input::IsKeyPressed( Input::KEY_LEFT_ALT );

		switch ( e.GetKeyCode() )
		{
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

			if ( ImGui::MenuItem( "Save Scene", nullptr, nullptr, false ) )
				TE_CORE_INFO( "Save Scene" );

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Editor Preferences" ) )
				m_PanelStack.PushPanel<EditorPreferences>();

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::BeginMenu( "Panels" ) )
			{
				if ( ImGui::MenuItem( "Content Browser" ) ) m_PanelStack.PushPanel<ContentBrowser>();
				if ( ImGui::MenuItem( "Stats" ) ) m_PanelStack.PushPanel<Stats>();

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Open Script Editor" ) )
				m_PanelStack.PushPanel<ScriptEditor>();

			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
				ScriptEngine::Recompile();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void EditorLayer::DrawEditorCameraViewPort()
	{
		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f) );

		ImGui::Begin( "Viewport ##" );
		{
			m_EditorCamera.Focused = ImGui::IsWindowFocused();
			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			ImGui::Image( (ImTextureID)m_EditorCameraFBO->GetColorAttachmentID(), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

			if ( m_ViewportSize != regionAvail )
			{
				m_ViewportSize = regionAvail;
				m_EditorCamera.SetViewportSize( regionAvail.x, regionAvail.y );
				m_EditorCameraFBO->Resize( regionAvail.x, regionAvail.y );
			}
		}
		ImGui::End();
	}

#pragma region - UIToolBar -

	UIToolBar::UIToolBar()
	{
		fs::path iconFolder( "Content/Engine/Editor/Icons" );

		PlayButtonIcon = Texture2D::Create( ( iconFolder / "PlayButton.png" ).string() );
		StopButtonIcon = Texture2D::Create( ( iconFolder / "StopButton.png" ).string() );
		PauseButtonIcon = Texture2D::Create( ( iconFolder / "PauseButton.png" ).string() );
	}

	void UIToolBar::OnImGuiDraw()
	{
		static constexpr ImVec2 buttonSize( 22, 22 );
		static constexpr ImVec2 buttonPadding( 5, 5 );

		ImGui::BeginMenuBar();
		//ImGui::Begin( "##UIToolBar", nullptr, 
		//	ImGuiWindowFlags_NoDecoration 
		//	| ImGuiWindowFlags_NoScrollbar 
		//	| ImGuiWindowFlags_NoScrollWithMouse 
		//	| ImGuiWindowFlags_NoResize 
		//	| ImGuiWindowFlags_NoMove
		//	| ImGuiWindowFlags_NoTitleBar );

		EditorLayer& editor = EditorLayer::Get();
		SceneState sceneState = EditorLayer::Get().CurrentSceneState;

		bool hasPlayButton = ( sceneState == SceneState::Edit ) || ( sceneState == SceneState::Play && editor.GetActiveScene()->IsPaused() );
		bool hasPauseButton = ( sceneState == SceneState::Play ) && ( !editor.GetActiveScene()->IsPaused() );
		bool hasStopButton = sceneState == SceneState::Play;

		float totalButtonSizeX = buttonSize.x + ( buttonPadding.x * 2.f );
		float groupSizeX = ( totalButtonSizeX * hasPlayButton ) + ( totalButtonSizeX * hasPauseButton ) + ( totalButtonSizeX * hasStopButton );
		ImGui::SetCursorPosX( ( ImGui::GetWindowContentRegionMax().x * 0.5f ) - groupSizeX );

		ImGui::BeginGroup();
		{
			ImGui::ScopedStyleVar padding( ImGuiStyleVar_FramePadding, buttonPadding );

			if ( hasPlayButton )
			{
				if ( ImGui::ImageButton( "PlayButton", (ImTextureID)PlayButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 }, { 0.5f, 1.0f, 0.5f, 1.0f } ) )
				{
					editor.OnBeginScene();
				}
			}

			if ( hasPauseButton )
			{
				if ( ImGui::ImageButton( "PauseButton", (ImTextureID)PauseButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 } ) )
				{
					editor.GetActiveScene()->SetPaused( true );
				}
			}

			ImGui::SameLine();

			if ( hasStopButton )
			{
				if ( ImGui::ImageButton( "StopButton", (ImTextureID)StopButtonIcon->GetRendererID(),
					buttonSize, { 0,1 }, { 1,0 },
					{ 0,0,0,0 }, { 1.0f, 0.5f, 0.5f, 1.0f } ) )
				{
					editor.OnEndScene();
				}
			}
		}
		ImGui::EndGroup();

		ImGui::EndMenuBar();
	}

#pragma endregion

}

#endif // IS_EDITOR