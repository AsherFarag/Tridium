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
		{
			m_Project = MakeShared<Project>();
			if ( !a_ProjectPath.empty() )
			{
				ProjectSerializer s( m_Project );
				s.DeserializeText( a_ProjectPath );
				m_Project->GetConfiguration().ProjectDirectory = IO::FilePath( a_ProjectPath ).GetParentPath();
			}
		}

		// Initialise Window
		{
			m_Window = Window::Create();
			m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, 1 ) );
		}

		// Initialise Physics Engine
		{
			m_PhysicsEngine = PhysicsEngine::Create();
			m_PhysicsEngine->Init();
		}

		// Initialise Asset Manager
		{
			InitializeAssetManager();
		}

		// Initialise Scene
		{
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( m_Project->GetConfiguration().StartScene ) )
			{
				m_ActiveScene = scene;
			}
			else
			{
				TE_CORE_WARN( "Failed to load start scene! - Creating new scene" );
				m_ActiveScene = MakeShared<Scene>();
			}
		}

		// Initialise ImGui
		{
			m_ImGuiLayer = new ImGuiLayer();
			PushOverlay( m_ImGuiLayer );
		}

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

		m_GameViewport.Init( m_Window->GetWidth(), m_Window->GetHeight() );

		uint32_t frameCounter = 0;
		double fpsInterval = 0.0;
		uint32_t minFPS = 0xFFFFFFFF;
		uint32_t maxFPS = 0;

	#ifndef IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnBegin();
		}
	#endif // IS_EDITOR


		while ( m_Running )
		{
			// Update Time
			const double lastFrameTime = Time::Now();
			Time::Update();
			Time::s_DeltaTime = Time::Now() - lastFrameTime;

			// Update FPS
			++frameCounter;
			fpsInterval += Time::DeltaTime();
			uint32_t curFrameRate = 1.0 / Time::DeltaTime();
			minFPS = MIN( minFPS, curFrameRate );
			maxFPS = MAX( maxFPS, curFrameRate );
			if ( fpsInterval >= 1.0 )
			{
				m_PrevFrameInfo.FPS = frameCounter;
				m_PrevFrameInfo.MinFPS = minFPS;
				m_PrevFrameInfo.MaxFPS = maxFPS;
				frameCounter = 0;
				fpsInterval -= 1.0;
				minFPS = 0xFFFFFFFF;
				maxFPS = 0;
			}

			// Update Loop
			if ( !m_Window->IsMinimized() )
			{
				Update();
			}

			m_Window->OnUpdate();
		}

#ifndef IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnEnd();
		}
#endif // IS_EDITOR

		// Shutdown Sequence
		{
			m_GameInstance->Shutdown();
			m_PhysicsEngine->Shutdown();
			m_AssetManager->Shutdown();

			#ifdef IS_EDITOR
			Editor::EditorApplication::Shutdown();
			#endif // IS_EDITOR
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Update()
	{
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
			// Render the scene with the main camera
			Vector3 cameraPos = m_ActiveScene->GetMainCamera()->GetGameObject().GetTransform().Position;
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				m_ActiveScene->GetMainCamera()->SceneCamera,
				m_ActiveScene->GetMainCamera()->GetView(),
				cameraPos );
		}
		else
		{
			// If no camera is found, render the scene with a default camera
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				Camera(),
				Matrix4( 1.0f ),
				Vector3( 0.0 ) );
		}

		m_GameViewport.Resize( m_Window->GetWidth(), m_Window->GetHeight() );
		m_GameViewport.RenderToWindow();

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
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Quit()
	{
	#ifdef IS_EDITOR
		Editor::GetEditorLayer()->OnEndScene();
	#else
		Get().Shutdown();
	#endif // IS_EDITOR
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
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
	
	void Application::SetScene( SharedPtr<Scene> a_Scene )
	{
		s_Instance->m_ActiveScene = a_Scene;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
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

	///////////////////////////////////////////////////////////////////////////////////////////
	bool Application::OnWindowClosed( WindowCloseEvent& e )
	{
		Shutdown();
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Shutdown()
	{
		m_Running = false;
	}

} // namespace Tridium