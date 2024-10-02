#include "tripch.h"
#include "Application.h"

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#include <Editor/EditorAssetManager.h>
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
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, 1 ) );

		// Initialise Project
		m_Project = MakeShared<Project>();
		ProjectSerializer s( m_Project );
		s.SerializeText( "test.tproject" );

		TODO( "Setup a proper scene initialiser!" );
		m_ActiveScene = MakeShared<Scene>();

		// Initialise ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

#ifdef IS_EDITOR

		Editor::EditorApplication::Init();
		Editor::GetEditorLayer()->SetActiveScene( m_ActiveScene );
		AssetManager::s_Instance = new Editor::EditorAssetManager();

#else

		AssetManager::s_Instance = new RuntimeAssetManager();

#endif // IS_EDITOR


		// Initialise the render pipeline
		RenderCommand::Init();
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

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

			for ( Layer* layer : m_LayerStack )
				layer->OnImGuiDraw();

			m_ImGuiLayer->End();
			// ---------
			
			// ====================================================================================================

			m_Window->OnUpdate();
		}

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
	
	void Application::PushLayer( Layer* layer )
	{
		m_LayerStack.PushLayer( layer );
	}
	
	void Application::PushOverlay( Layer* overlay )
	{
		m_LayerStack.PushOverlay( overlay );
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
}