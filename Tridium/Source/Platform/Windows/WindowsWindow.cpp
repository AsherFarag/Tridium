#include "tripch.h"
#include "WindowsWindow.h"

#include <Tridium/Events/Event.h>

#include <Platform/OpenGL/OpenGLContext.h>

#include <Tridium/Graphics/Rendering/RenderCommand.h>

// TEMP?
#include "stb_image.h"
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	static bool s_GLFWInitialized = false;

	constexpr EInputMouseButton TranslateMouseButton( int button )
	{
		switch ( button )
		{
		case GLFW_MOUSE_BUTTON_LEFT:   return EInputMouseButton::Left;
		case GLFW_MOUSE_BUTTON_RIGHT:  return EInputMouseButton::Right;
		case GLFW_MOUSE_BUTTON_MIDDLE: return EInputMouseButton::Middle;
		case GLFW_MOUSE_BUTTON_4:      return EInputMouseButton::Button4;
		case GLFW_MOUSE_BUTTON_5:      return EInputMouseButton::Button5;
		case GLFW_MOUSE_BUTTON_6:      return EInputMouseButton::Button6;
		case GLFW_MOUSE_BUTTON_7:      return EInputMouseButton::Button7;
		case GLFW_MOUSE_BUTTON_8:      return EInputMouseButton::Button8;
		}

		ASSERT_LOG( false, "Unknown mouse button!" );
		return EInputMouseButton::None;
	}

	static void GLFWErrorCallback( int error, const char* description )
	{
		LOG( LogCategory::Application, Error, "GLFW Error ({0}): {1}", error, description );
	}

	UniquePtr<Window> Window::Create( const WindowProps& props )
	{
		return MakeUnique<WindowsWindow>( props );
	}

	WindowsWindow::WindowsWindow( const WindowProps& props )
	{
		Init( props );
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();

		TODO( "Remove this once we are using the RHI" );
		if ( false )
		{
			glfwSwapBuffers( m_Window );
		}
	}

	bool WindowsWindow::IsMinimized() const
	{
		return glfwGetWindowAttrib( m_Window, GLFW_ICONIFIED );
	}

	void WindowsWindow::SetVSync( bool enabled )
	{
		if ( enabled )
			glfwSwapInterval( 1 );
		else
			glfwSwapInterval( 0 );

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::SetTitle( const std::string& title )
	{
		m_Data.Title = title;
		glfwSetWindowTitle( m_Window, m_Data.Title.c_str() );
	}

	void WindowsWindow::SetIcon( const std::string& path )
	{
		GLFWimage images[1];
		images[0].pixels = stbi_load( path.c_str(), &images[0].width, &images[0].height, 0, 4 ); //rgba channels 
		glfwSetWindowIcon( m_Window, 1, images );
		stbi_image_free( images[0].pixels );
	}

	void WindowsWindow::MakeContextCurrent( Window* window )
	{
		if ( window )
			glfwMakeContextCurrent( (GLFWwindow*)window->GetNativeWindow() );
		else
			glfwMakeContextCurrent( nullptr );
	}

	void WindowsWindow::Init( const WindowProps& props )
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG( LogCategory::Application, Info, "Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height );

		// Only initializes GLFW once
		if ( !s_GLFWInitialized )
		{
			int success = glfwInit();
			ASSERT_LOG( success, "Could not initialize GLFW!" );
			glfwSetErrorCallback( GLFWErrorCallback );
			s_GLFWInitialized = true;
		}

		TODO( "Make anti-aliasing a setting!" );
		glfwWindowHint( GLFW_SAMPLES, 8 );

		m_Window = glfwCreateWindow( (int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr );
		MakeContextCurrent( this );

		// - Rendering Context Initialisation -
		//m_RenderingContext = RenderingContext::Create( m_Window );
		//m_RenderingContext->Init();

		glfwSetWindowUserPointer( m_Window, &m_Data );
		SetVSync( true );

		// - Set GLFW callbacks -

		glfwSetWindowSizeCallback( m_Window, []( GLFWwindow* window, int width, int height )
			{
				//RenderCommand::SetViewport( 0, 0, width, height );

				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
				data.Width = width;
				data.Height = height;

				data.EventCallback( WindowResizeEvent( width, height ) );
			} );

		glfwSetWindowCloseCallback( m_Window, []( GLFWwindow* window )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
				WindowCloseEvent event;
				data.EventCallback( event );
			} );

		glfwSetKeyCallback( m_Window, []( GLFWwindow* window, int key, int scancode, int action, int mods )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );

				switch ( action )
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event( static_cast<EInputKey>( key ), 0 );
					data.EventCallback( event );
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event( static_cast<EInputKey>( key ) );
					data.EventCallback( event );
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event( static_cast<EInputKey>( key ), 1 );
					data.EventCallback( event );
					break;
				}
				}
			} );

		glfwSetCharCallback( m_Window, []( GLFWwindow* window, unsigned int character )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
				KeyTypedEvent event( static_cast<EInputKey>( character ) );
				data.EventCallback( event );
			} );

		glfwSetMouseButtonCallback( m_Window, []( GLFWwindow* window, int button, int action, int mods )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );

				switch ( action )
				{
				case GLFW_PRESS:
				{
					EInputMouseButton key = TranslateMouseButton( button );
					data.EventCallback( MouseButtonPressedEvent( key ) );
					break;
				}
				case GLFW_RELEASE:
				{
					EInputMouseButton key = TranslateMouseButton( button );
					MouseButtonReleasedEvent event( key );
					data.EventCallback( event );
					break;
				}
				}
			} );

		glfwSetScrollCallback( m_Window, []( GLFWwindow* window, double xOffset, double yOffset )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
				MouseScrolledEvent event( (float)xOffset, (float)yOffset );
				data.EventCallback( event );
				Input::_SetMouseScrollOffset( (int)xOffset, (int)yOffset );
			} );

		glfwSetCursorPosCallback( m_Window, []( GLFWwindow* window, double xPos, double yPos )
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer( window );
				MouseMovedEvent event( (float)xPos, (float)yPos );
				data.EventCallback( event );
			} );
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow( m_Window );
	}

}