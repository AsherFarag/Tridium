#include "tripch.h"
#include "Editor.h"

#if WITH_EDITOR

#include <Tridium/Engine/Engine.h>

// Panels
#include <Tridium/Editor/UI/AssetBrowserPanel.h>
#include <Tridium/Editor/UI/EditorViewportPanel.h>
#include <Tridium/Editor/UI/FrameProfilerPanel.h>
#include <Tridium/Editor/UI/InspectorPanel.h>
#include <Tridium/Editor/UI/SceneHierarchyPanel.h>

#if CONFIG_PLATFORM_WINDOWS
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#endif // CONFIG_PLATFORM_WINDOWS

namespace Tridium {

	REGISTER_TICK_GROUP( EditorTick, "BeginAppUpdate"_H );
	REGISTER_TICK_GROUP( EditorRender, "Render"_H );

	Editor* Editor::s_Instance = nullptr;
	decltype( Editor::Events::OnGameObjectSelected ) Editor::Events::OnGameObjectSelected{};
	decltype( Editor::Events::OnSelectionChanged ) Editor::Events::OnSelectionChanged{};

	Editor::Editor() : IAppLayer( "Editor" )
	{
		ENSURE( !s_Instance, "Editor instance already exists!" );

		s_Instance = this;

		// Set Window title and icon
		Application::GetWindow().SetTitle("Tridium Editor");
		//Application::GetWindow().SetIcon( ( Engine::Get()->EngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" ).ToString() );

		Application::AddOnTick( TickGroups::EditorTick, []() { Editor::Get()->Tick(); } );
		Application::AddOnTick( TickGroups::BeginUIDraw, []() { Editor::Get()->DrawUI(); } );

	#if CONFIG_PLATFORM_WINDOWS

		// Set the window proc
		// To keep the functionality of dragging the window and it snapping to the screen, etc.
		// We need to override the window procedure so we can handle if the title bar is hovered or not.

		GLFWwindow* window = reinterpret_cast<GLFWwindow*>( Application::GetWindow().GetNativeWindow() );
		HWND hWnd = glfwGetWin32Window( window );

		static WNDPROC s_OriginalProc = (WNDPROC)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
		const auto WindowProc = +[]( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) -> LRESULT
		{
			if ( uMsg == WM_NCHITTEST )
			{
				if ( Editor::Get() && Editor::Get()->m_IsTitleBarHovered )
				{
					// If the title bar is hovered, we want to allow dragging the window
					return HTCAPTION;
				}
			}

			// Call the original window procedure for default handling
			return CallWindowProc( s_OriginalProc, hWnd, uMsg, wParam, lParam );
		};
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( WindowProc ) );

	#endif // CONFIG_PLATFORM_WINDOWS

		// TEMP!
		SceneManager::SetActiveScene( Scene::Create( MakeShared<AssetInfo>(
			AssetInfo{
				.ID = UUID::Generate(),
				.Name = "Untilted"
			}
		) ) );

		SceneManager::ActiveScene()->Init();
	}

	Editor::~Editor()
	{
		s_Instance = nullptr;
	}

	void Editor::Tick()
	{
		const float deltaTime = Application::GetDeltaTime();

		if ( const SharedPtr<Scene>& scene = SceneManager::ActiveScene() )
		{
			if ( m_ScenePlayMode != EScenePlayMode::None && ( !m_ScenePaused || m_SceneStepFrames > 0 ) )
			{
				// In play mode and either not paused or stepping
				scene->OnUpdate( deltaTime );

				if ( m_SceneStepFrames > 0 )
					m_SceneStepFrames--;
			}

			scene->OnRender( deltaTime );
		}

		m_UIManager.UpdateUI( Application::GetDeltaTime() );
	}

	void Editor::DrawUI()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* Viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos( Viewport->Pos );
		ImGui::SetNextWindowSize( Viewport->Size );
		ImGui::SetNextWindowViewport( Viewport->ID );

		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleColor( ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f } );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

		// Begin the main dockspace window
		ImGui::Begin( "Editor DockSpace", nullptr, windowFlags );
		{
			ImGui::PopStyleColor();
			ImGui::PopStyleVar( 3 );

			UI_DrawTitleBar();

			// Dockspace
			static const ImGuiID DockspaceID = ImGui::GetID( "Dockspace" );
			ImGui::DockSpace( DockspaceID );
		}
		ImGui::End();

		m_UIManager.DrawUI();
	}

	void Editor::OnAttach()
	{
		// Panels
		m_UIManager.CreatePanel<AssetBrowserPanel>( TE_ICON_FOLDER_OPEN " Assets", true );
		m_UIManager.CreatePanel<EditorViewportPanel>( TE_ICON_TV " Editor", true );
		m_UIManager.CreatePanel<SceneHierarchyPanel>( TE_ICON_MOUNTAIN_SUN " Hierarchy", true );
		m_UIManager.CreatePanel<InspectorPanel>( TE_ICON_MAGNIFYING_GLASS " Inspector", true );
		m_UIManager.CreatePanel<FrameProfilerPanel>( TE_ICON_CLOCK " Profiler", true );
	}

	void Editor::OnDetach()
	{
	}

	void Editor::OnEvent( Event& a_Event )
	{
		m_UIManager.OnEvent( a_Event );

		if ( a_Event.IsConsumed() )
			return;

		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<KeyPressedEvent>( [this]( const KeyPressedEvent& a_Event ) -> bool { return Event_KeyPressed( a_Event ); } );
	}

	void Editor::BeginPlay( EScenePlayMode a_PlayMode )
	{
		m_ScenePlayMode = a_PlayMode;
		m_ScenePaused = false;

		// Take a snapshot of the current scene state for restoration later
		m_SceneSnapshot = SceneManager::ActiveScene()->Registry().Clone();

		SceneManager::ActiveScene()->OnBeginPlay( a_PlayMode );
	}

	void Editor::Pause( bool a_Pause )
	{
		m_ScenePaused = a_Pause;
	}

	void Editor::Step()
	{
		m_SceneStepFrames++;
	}

	void Editor::EndPlay()
	{
		SceneManager::ActiveScene()->OnEndPlay();

		SceneManager::ActiveScene()->m_Registry = std::move( m_SceneSnapshot );

		m_ScenePlayMode = EScenePlayMode::None;
		m_ScenePaused = false;
		m_SceneStepFrames = 0;
	}

	void Editor::UI_DrawTitleBar()
	{
		const float TitleBarHeight = 57.0f;
		const ImVec2 WindowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos( ImVec2( WindowPadding.x, WindowPadding.y ) );
		const ImVec2 TitlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 TitlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - WindowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + TitleBarHeight };

		auto* DrawList = ImGui::GetWindowDrawList();
		DrawList->AddRectFilled( TitlebarMin, TitlebarMax, UI::GetTheme().Titlebar );

		// Draw animated gradient
		{
			static float s_GradientTime = 0.0f;
			s_GradientTime += (float)Application::GetDeltaTime() * 1.0f; // Adjust speed as needed
			// Oscillates between 0.25 and 1
			float GradientMultipler = glm::sin( s_GradientTime ) * 0.375f + 0.625f; // This will give a value between 0.25 and 1

			ImVec4 GradientColours[4];
			GradientColours[0] = UI::GetTheme().TitlebarGradientTopLeft;
			GradientColours[1] = UI::GetTheme().TitlebarGradientTopRight;
			GradientColours[2] = UI::GetTheme().TitlebarGradientBottomRight;
			GradientColours[3] = UI::GetTheme().TitlebarGradientBottomLeft;

			for ( auto& Colour : GradientColours ) Colour.w *= 0;

			if ( m_ScenePlayMode == EScenePlayMode::Play )
			{
				GradientColours[1] = UI::GetTheme().Green;
				GradientColours[1].w = GradientMultipler;
			}
			else if ( m_ScenePlayMode == EScenePlayMode::Simulate )
			{
				GradientColours[1] = UI::GetTheme().Blue;
				GradientColours[1].w = GradientMultipler;
			}

			if ( m_ScenePaused && m_ScenePlayMode != EScenePlayMode::None )
			{
				GradientColours[0] = UI::GetTheme().Orange;
				GradientColours[0].w = 1.0f - GradientMultipler;
			}

			DrawList->AddRectFilledMultiColor(
				TitlebarMin, ImVec2( TitlebarMax.x, TitlebarMax.y - TitleBarHeight + ( TitleBarHeight * 0.75f ) ),
				ImGui::ColorConvertFloat4ToU32( GradientColours[0] ),
				ImGui::ColorConvertFloat4ToU32( GradientColours[1] ),
				ImGui::ColorConvertFloat4ToU32( GradientColours[2] ),
				ImGui::ColorConvertFloat4ToU32( GradientColours[3] )
			);
		}

		float MenubarOffsetX = TitlebarMin.x + WindowPadding.x;
		float MenubarOffsetY = 0.0f;

		// Draw the Apple logo
		{
			const float logoSize = ( TitlebarMax.y - TitlebarMin.y ) * 0.6f;
			const ImVec2 logoRectStart = TitlebarMin + ImVec2( 5.0f, 5.0f );
			const ImVec2 logoRectMax = ImVec2( logoRectStart.x + logoSize, logoRectStart.y + logoSize );

			// Center the menubar with respect to the logo
			MenubarOffsetY = ( logoRectMax.y - logoRectStart.y ) * 0.5f;

			DrawList->AddRect( logoRectStart, logoRectMax, IM_COL32( 255, 255, 255, 255 ), 4.0f );

			MenubarOffsetX += logoSize + 10.0f; // Add some space after the logo
		}

		const float ButtonsAreaWidth = 120.0f;

		// Create the drag zone for the title bar
		{
			ImGui::SetNextItemAllowOverlap();
			ImGui::InvisibleButton(
				"##TitleBarDragZone",
				ImVec2( ImGui::GetContentRegionAvail().x - ButtonsAreaWidth, TitleBarHeight ) );

			// Check if the title bar is hovered and is not blocked by overlapping items
			m_IsTitleBarHovered = ImGui::IsItemHovered();
		}

		// Draw Menubar
		ImGui::BeginGroup();
		{
			const float MenubarHeight = ImGui::GetTextLineHeightWithSpacing() + WindowPadding.y * 2.0f;
			MenubarOffsetY -= 10.0f; TODO( "Make the menu bar, play bar and window buttons aligned without this magic value" );
			UI_DrawMenuBar( Vector2( MenubarOffsetX, TitlebarMin.y + MenubarOffsetY ),
						    Vector2( MenubarOffsetX + ImGui::GetWindowWidth() - ButtonsAreaWidth, TitlebarMin.y + MenubarHeight + MenubarOffsetY ) );
		}
		ImGui::EndGroup();

		//=====================================================
		// Draw Scene Buttons
		//=====================================================

		UI_DrawPlayBar( TitleBarHeight );

		//=====================================================
		// Window Buttons
		//=====================================================

		ImGui::PushStyleColor( ImGuiCol_Button, 0u );
		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.0f );

		const float WindowButtonSize = TitleBarHeight / 2.0f;
		ImVec2 CursorPos( ImGui::GetWindowWidth() - ButtonsAreaWidth, WindowPadding.y + 6.0f );
		ImGui::SetCursorPos( CursorPos );

		// Minimise Button
		{
			if ( ImGui::Button( EditorIcons::WindowMinimize.Data, ImVec2( WindowButtonSize, WindowButtonSize ) ) )
			{
				Application::GetWindow().Minimise();
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Minimise" );
				ImGui::EndTooltip();
			}
		}

		// Maximise/Restore Button
		ImGui::SameLine();
		{
			const bool isWindowMaximised = Application::GetWindow().IsMaximised();
			const char* MaximizeIcon = isWindowMaximised
				? TE_ICON_WINDOW_RESTORE "###Maximize"
				: TE_ICON_WINDOW_MAXIMIZE "###Maximize";

			if ( ImGui::Button( MaximizeIcon, ImVec2( WindowButtonSize, WindowButtonSize ) ) )
			{
				if ( isWindowMaximised )
				{
					Application::GetWindow().Restore();
				}
				else
				{
					Application::GetWindow().Maximise();
				}
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( isWindowMaximised ? "Restore" : "Maximise" );
				ImGui::EndTooltip();
			}
		}

		// Close Button
		ImGui::SameLine();
		{
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, (ImVec4)UI::GetTheme().Red );

			if ( ImGui::Button( EditorIcons::Xmark.Data, ImVec2( WindowButtonSize, WindowButtonSize ) ) )
			{
				Application::GetWindow().Close();
			}

			ImGui::PopStyleColor();

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::TextUnformatted( "Close" );
				ImGui::EndTooltip();
			}
		}

		ImGui::PopStyleVar(); // Reset FrameBorderSize
		ImGui::PopStyleColor(); // Reset Button color
	}

	void Editor::UI_DrawMenuBar( Vector2 a_Min, Vector2 a_Max )
	{
		ImRect MenubarRect( ImVec2( a_Min.X, a_Min.Y ), ImVec2( a_Max.X, a_Max.Y ) );
		if ( !ImGui::BeginMenuBarEx( MenubarRect ) )
			return;

		if ( ImGui::BeginMenu( "File" ) )
		{
			// Project
			if ( ImGui::MenuItem( "New Scene", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "New Scene" );

			if ( ImGui::MenuItem( "Open Scene", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "Open Scene" );

			if ( ImGui::MenuItem( "Save Scene", nullptr, nullptr, false ) )
				LOG( LogCategory::Editor, Info, "Save Scene" );

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			//if ( ImGui::MenuItem( "Project Settings" ) )
			//	//ToolUIModule::CreatePanel< ProjectSettingsPanel >();

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::BeginMenu( "Panels" ) )
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Other" ) )
		{
			if ( ImGui::MenuItem( "About" ) )
				LOG( LogCategory::Editor, Info, "About clicked" );

			if ( ImGui::MenuItem( "Help " TE_ICON_CIRCLE_QUESTION ) )
				LOG( LogCategory::Editor, Info, "Help clicked" );

			ImGui::EndMenu();
		}

		ImGui::EndMenuBarEx();
	}

	void Editor::UI_DrawPlayBar( float a_Height )
	{
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		const bool hasPlayButton = m_ScenePlayMode == EScenePlayMode::None || ( m_ScenePlayMode == EScenePlayMode::Play && m_ScenePaused );
		const bool hasSimulateButton = m_ScenePlayMode == EScenePlayMode::None || ( m_ScenePlayMode == EScenePlayMode::Simulate && m_ScenePaused );
		const bool hasPauseButton = m_ScenePlayMode != EScenePlayMode::None && !m_ScenePaused;
		const bool hasStepButton = m_ScenePaused && m_ScenePlayMode != EScenePlayMode::None;
		const bool hasStopButton = m_ScenePlayMode != EScenePlayMode::None;
		const bool hasSettingsButton = m_ScenePlayMode == EScenePlayMode::None;

		const int numButtons = int( hasPlayButton ) + int( hasSimulateButton ) + int( hasPauseButton ) + int( hasStepButton ) + int( hasStopButton ) + int( hasSettingsButton );

		const ImVec2 currentCursorPos = ImGui::GetCursorPos();

		const ImVec2 buttonSize( a_Height / 2.0f, a_Height / 2.0f );
		const ImVec2 groupSize( numButtons * buttonSize.x + ( numButtons - 1 ) * ImGui::GetStyle().ItemSpacing.x,
								buttonSize.y + windowPadding.y * 2.0f );

		ImGui::SetCursorPos( ImVec2( ImGui::GetWindowWidth() * 0.5f - groupSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f ) );

		ImGui::PushStyleColor( ImGuiCol_ChildBg, (ImVec4)UI::GetTheme().Background );
		ImGui::BeginChild( "##SceneButtons", groupSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
		{
			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.0f );

			bool needsSameLine = false;

			// Play Button
			if ( hasPlayButton )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Green );

				if ( ImGui::Button( TE_ICON_PLAY, buttonSize ) )
				{
					if ( !m_ScenePaused )
					{
						BeginPlay( EScenePlayMode::Play );
					}
					else
					{
						Pause( false );
					}
				}

				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Play" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			if ( hasSimulateButton )
			{
				if ( needsSameLine )
				{
					ImGui::SameLine();
				}

				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Blue );

				if ( ImGui::Button( TE_ICON_GEARS, buttonSize ) )
				{
					if ( !m_ScenePaused )
					{
						BeginPlay( EScenePlayMode::Simulate );
					}
					else
					{
						Pause( false );
					}
				}

				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Simulate" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			// Pause Button
			if ( hasPauseButton )
			{
				if ( needsSameLine )
				{
					ImGui::SameLine();
				}

				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Text );

				if ( ImGui::Button( TE_ICON_PAUSE, buttonSize ) )
				{
					Pause( true );
				}

				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Pause" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			// Step Button
			if ( hasStepButton )
			{
				if ( needsSameLine )
				{
					ImGui::SameLine();
				}

				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Text );
				if ( ImGui::Button( TE_ICON_FORWARD_STEP, buttonSize ) )
				{
					Step();
				}

				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Step" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			// Stop Button
			if ( hasStopButton )
			{
				if ( needsSameLine )
				{
					ImGui::SameLine();
				}

				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Red );

				if ( ImGui::Button( TE_ICON_STOP, buttonSize ) )
				{
					EndPlay();
				}

				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Stop" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			// Settings Button
			if ( hasSettingsButton )
			{
				if ( needsSameLine )
				{
					ImGui::SameLine();
				}

				ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)UI::GetTheme().Text );
				if ( ImGui::Button( TE_ICON_ELLIPSIS_VERTICAL, buttonSize ) )
				{
					ImGui::OpenPopup( "GamePlaySettingsPopup" );
				}
				ImGui::PopStyleColor();

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::TextUnformatted( "Game Settings" );
					ImGui::EndTooltip();
				}

				needsSameLine = true;
			}

			if ( ImGui::BeginPopup( "GamePlaySettingsPopup" ) )
			{
				{
					static int numClients = 1;
					ImGui::SliderInt( "Number of Clients", &numClients, 1, 4, "%d Clients" );
					if ( ImGui::BeginItemTooltip() )
					{
						ImGui::TextUnformatted( "Set the number of clients to spawn when simulating the game." );
						ImGui::EndTooltip();
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar(); // Reset FrameBorderSize
		}
		ImGui::EndChild();
		ImGui::PopStyleColor(); // Reset Child background color


		ImGui::SetCursorPos( currentCursorPos );
	}

	bool Editor::Event_KeyPressed( const KeyPressedEvent& a_Event )
	{
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
					m_UserActionManager.Redo();
					return true;
				}
				break;
			}
			// Undo
			case EInputKey::Z:
			{
				if ( control )
				{
					m_UserActionManager.Undo();
					return true;
				}
				break;
			}
		}

		return false;
	}

}

#endif // WITH_EDITOR
