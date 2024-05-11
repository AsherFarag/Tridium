#include "tripch.h"
#include "Application.h"

#include <glad/glad.h>

namespace Tridium {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, std::placeholders::_1 ) );

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;
	
		while ( m_Running )
		{
			glClearColor( 0.2, 0.5, 1.0, 1.0 );
			glClear( GL_COLOR_BUFFER_BIT );

			for ( Layer* layer : m_LayerStack )
				layer->OnUpdate();

			m_ImGuiLayer->Begin();

			for ( Layer* layer : m_LayerStack )
				layer->OnImGuiDraw();

			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
	
	void Application::OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, std::placeholders::_1 ) );
	
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
		m_Running = false;
		return true;
	}

}
