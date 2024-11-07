#include "tripch.h"
#include "Application.h"

#ifdef IS_EDITOR
	#include <Editor/Editor.h>
	#include <Tridium/Asset/EditorAssetManager.h>
#else
	#include <Tridium/Asset/RuntimeAssetManager.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/Rendering/GameViewport.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Rendering/RenderCommand.h>
#include "Tridium/IO/ProjectSerializer.h"


namespace Tridium {

	Application* Application::s_Instance = nullptr;

	Application::Application( const std::string& a_ProjectPath )
	{
		// Set the singleton instance
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;

		// Initialise Project
		m_Project = MakeShared<Project>();
		if ( !a_ProjectPath.empty() )
		{
			ProjectSerializer s( m_Project );
			s.DeserializeText( a_ProjectPath );
			m_Project->GetConfiguration().ProjectDirectory = IO::FilePath( a_ProjectPath ).GetParentPath();
		}

		// Initialise Window
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, 1 ) );

		// Initialise Asset Manager
		InitializeAssetManager();

		// Initialise Scene
		m_ActiveScene = MakeShared<Scene>();
		if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( m_Project->GetConfiguration().StartScene ) )
		{
			m_ActiveScene = scene;
		}
		else
		{
			TE_CORE_WARN( "Failed to load start scene! - Creating new scene" );
		}

		// Initialise ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

		// Initialise the render pipeline
		RenderCommand::Init();

		// Initialise the editor
#ifdef IS_EDITOR
		Editor::EditorApplication::Init();
#endif // IS_EDITOR
	}

	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

#ifndef IS_EDITOR

		GameViewport gameViewport;

#endif // !IS_EDITOR

		while ( m_Running )
		{
			Time::Update();

			m_FPS = (uint32_t)( 1.0 / Time::DeltaTime() );

			// Update Loop ========================================================================================

			#ifndef IS_EDITOR

			Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );

			m_ActiveScene->OnUpdate();

			#endif // IS_EDITOR

			// ====================================================================================================



			// Render Loop ========================================================================================
			
			#ifndef IS_EDITOR

			if ( !m_ActiveScene->GetMainCamera() )
			{
				auto view = m_ActiveScene->GetRegistry().view<CameraComponent>();
				if ( !view.empty() )
					m_ActiveScene->SetMainCamera( view.begin()[0] );
			}

			if ( m_ActiveScene->GetMainCamera() )
			{
				Vector3 cameraPos = m_ActiveScene->GetMainCamera()->GetGameObject().GetTransform().Position;
				m_ActiveScene->GetSceneRenderer().Render(
					gameViewport.GetFramebuffer(),
					m_ActiveScene->GetMainCamera()->SceneCamera,
					m_ActiveScene->GetMainCamera()->GetView(),
					cameraPos );
			}

			gameViewport.Resize( m_Window->GetWidth(), m_Window->GetHeight() );
			gameViewport.RenderToWindow();

			#endif // !IS_EDITOR

			
			// ====================================================================================================



			// Layers =============================================================================================

			for ( Layer* layer : m_LayerStack )
				layer->OnUpdate();

			// - ImGui -
			m_ImGuiLayer->Begin();

			for ( int i = 0; i < m_LayerStack.NumLayers(); i++ )
				m_LayerStack[i]->OnImGuiDraw();

			m_ImGuiLayer->End();

			// ---------
			
			// ====================================================================================================

			m_Window->OnUpdate();
		}

		m_GameInstance->Shutdown();
		m_AssetManager->Shutdown();

		#ifdef IS_EDITOR

		Editor::EditorApplication::Shutdown();

		#endif // IS_EDITOR
	}

	void Application::Quit()
	{
	#ifdef IS_EDITOR
		Editor::GetEditorLayer()->OnEndScene();
	#else
		Get().Shutdown();
	#endif // IS_EDITOR
	}
	
	void Application::OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, 1 ) );
	
		for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			( *--it )->OnEvent( e );
			if ( e.Handled )
				break;
		}
	}
	
	void Application::InitializeAssetManager()
	{
#ifdef IS_EDITOR
		m_AssetManager = MakeShared<Editor::EditorAssetManager>();
#else
		//m_AssetManager = MakeShared<RuntimeAssetManager>();
#endif // IS_EDITOR
		m_AssetManager = MakeShared<Editor::EditorAssetManager>();
		m_AssetManager->Init();
	}

	bool Application::OnWindowClosed( WindowCloseEvent& e )
	{
		Shutdown();
		return true;
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

} // namespace Tridium