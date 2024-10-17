#include "tripch.h"
#include "Application.h"

#ifdef IS_EDITOR
	#include <Editor/Editor.h>
	#include <Tridium/Asset/EditorAssetManager.h>
#else
	#include <Tridium/Asset/RuntimeAssetManager.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/Rendering/RenderCommand.h>
#include "Tridium/IO/ProjectSerializer.h"


namespace Tridium {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		// Set the singleton instance
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;

		// Initialise Window
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, 1 ) );

		// Initialise Project
		m_Project = MakeShared<Project>();
		ProjectSerializer s( m_Project );
		s.SerializeText( "test.tproject" );

		// Initialise Asset Manager
		InitializeAssetManager();

		// Initialise Scene
		TODO( "Setup a proper scene initialiser!" );
		m_ActiveScene = MakeShared<Scene>();

		// Initialise ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

		// Initialise the render pipeline
		RenderCommand::Init();

		// Initialise the editor
#ifdef IS_EDITOR
		Editor::EditorApplication::Init();
		Editor::GetEditorLayer()->SetActiveScene( m_ActiveScene );
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

		while ( m_Running )
		{
			Time::Update();

			m_FPS = (uint32_t)( 1.0 / Time::DeltaTime() );

			// Update Loop ========================================================================================

			#ifndef IS_EDITOR

			m_ActiveScene->OnUpdate();

			#endif // IS_EDITOR

			// ====================================================================================================



			// Render Loop ========================================================================================
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
		m_AssetManager = MakeShared<RuntimeAssetManager>();
#endif // IS_EDITOR

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