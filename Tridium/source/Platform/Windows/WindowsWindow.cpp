#include "tripch.h"
#include "WindowsWindow.h"

#include <Tridium/Events/KeyEvent.h>
#include <Tridium/Events/ApplicationEvent.h>
#include <Tridium/Events/MouseEvent.h>

#include <Platform/OpenGL/OpenGLContext.h>

namespace Tridium {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback( int error, const char* description )
	{
		TE_CORE_ERROR( "GLFW Error ({0}): {1}", error, description );
	}

	UniquePtr<Window> Window::Create( const WindowProps& props )
	{
		return MakeUnique<WindowsWindow>( props );
	}

	Tridium::WindowsWindow::WindowsWindow( const WindowProps& props )
	{
		Init( props );
	}

	Tridium::WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void Tridium::WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_RenderingContext->SwapBuffers();
	}

	void Tridium::WindowsWindow::SetVSync( bool enabled )
	{
		if ( enabled )
			glfwSwapInterval( 1 );
		else 
			glfwSwapInterval( 0 );

		m_Data.VSync = enabled;
	}

	bool Tridium::WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void Tridium::WindowsWindow::Init( const WindowProps& props )
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		TE_CORE_INFO( "Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height );

		// Only initializes GLFW once
		if ( !s_GLFWInitialized )
		{
			int success = glfwInit();
			TE_CORE_ASSERT( success, "Could not initialize GLFW!" );
			glfwSetErrorCallback( GLFWErrorCallback );
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow( (int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr );
		glfwMakeContextCurrent( m_Window );

		// - Rendering Context Initialisation -
		TODO( "Make this API dependent!" );
		m_RenderingContext = new OpenGLContext(m_Window);
		m_RenderingContext->Init();

		glfwSetWindowUserPointer( m_Window, &m_Data );
		SetVSync( true );

		// - Set GLFW callbacks -

		glfwSetWindowSizeCallback( m_Window, []( GLFWwindow* window, int width, int height )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event( width, height );
			data.EventCallback( event );
		});

		glfwSetWindowCloseCallback( m_Window, []( GLFWwindow* window )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			WindowCloseEvent event;
			data.EventCallback( event );
		});

		glfwSetKeyCallback( m_Window, []( GLFWwindow* window, int key, int scancode, int action, int mods )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			
			switch ( action )
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event( (Input::KeyCode)key, 0 );
					data.EventCallback( event );
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event( (Input::KeyCode)key );
					data.EventCallback( event );
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event( (Input::KeyCode)key, 1 );
					data.EventCallback( event );
					break;
				}
			}
		});

		glfwSetCharCallback( m_Window, []( GLFWwindow* window, unsigned int character )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			KeyTypedEvent event( (Input::KeyCode) character );
			data.EventCallback( event );
		} );

		glfwSetMouseButtonCallback( m_Window, []( GLFWwindow* window, int button, int action, int mods )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );

			switch ( action )
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event( button );
				data.EventCallback( event );
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event( button );
				data.EventCallback( event );
				break;
			}
			}
		});

		glfwSetScrollCallback( m_Window, []( GLFWwindow* window, double xOffset, double yOffset )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			MouseScrolledEvent event( (float)xOffset, (float)yOffset );
			data.EventCallback( event );
		});

		glfwSetCursorPosCallback( m_Window, []( GLFWwindow* window, double xPos, double yPos )
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
			MouseMovedEvent event( (float)xPos, (float)yPos );
			data.EventCallback( event );
		});
	}

	void Tridium::WindowsWindow::Shutdown()
	{
		glfwDestroyWindow( m_Window );
	}

}