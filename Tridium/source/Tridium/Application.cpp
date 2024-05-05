#include "tripch.h"
#include "Application.h"

#include <glad/glad.h>

namespace Tridium {

#define BIND_EVENT_FN(x) std::bind( &x, this, std::placeholders::_1 ) 

Tridium::Application::Application()
{
	m_Window = std::unique_ptr<Window>( Window::Create() );
	m_Window->SetEventCallback( BIND_EVENT_FN( Application::OnEvent ) );
} 

Tridium::Application::~Application()
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

		m_Window->OnUpdate();
	}
}

void Application::OnEvent( Event& e )
{
	EventDispatcher dispatcher( e );
	dispatcher.Dispatch<WindowCloseEvent>( BIND_EVENT_FN( Application::OnWindowClosed ) );

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
