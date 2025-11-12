#include "tripch.h"

#if CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS

#include <Tridium/Application/PlatformInterface.h>

#include <glfw/glfw3.h>

#if CONFIG_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#undef CreateWindow
#endif

namespace Tridium {

#if CONFIG_PLATFORM_WINDOWS
	// Unholy windows schenanigans to keep window resizing working correctly for non decorated windows.
	static WNDPROC OriginalProc;
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch ( uMsg )
		{
			case WM_NCCALCSIZE:
			{
				if ( wParam == TRUE && lParam != NULL )
				{
					NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>( lParam );

					// Detect if maximized
					WINDOWPLACEMENT WindowPlacement;
					WindowPlacement.length = sizeof( WindowPlacement );
					GetWindowPlacement( hWnd, &WindowPlacement );

					if ( WindowPlacement.showCmd != SW_MAXIMIZE )
					{
						// Only trim borders when not maximized
						pParams->rgrc[0].top += 1;
						pParams->rgrc[0].right -= 1;
						pParams->rgrc[0].bottom -= 1;
						pParams->rgrc[0].left += 1;
					}
				}

				return 0;
			}
			case WM_NCPAINT:
			{
				// Prevent the non-client area from being painted
				return 0;
			}
			case WM_NCHITTEST:
			{
				// Expand the hit test area for resizing
				const int borderWidth = 8; // Adjust this value to control the hit test area size

				POINTS mousePos = MAKEPOINTS( lParam );
				POINT clientMousePos = { mousePos.x, mousePos.y };
				ScreenToClient( hWnd, &clientMousePos );

				RECT windowRect;
				GetClientRect( hWnd, &windowRect );

				if ( clientMousePos.y >= windowRect.bottom - borderWidth )
				{
					if ( clientMousePos.x <= borderWidth )
						return HTBOTTOMLEFT;
					else if ( clientMousePos.x >= windowRect.right - borderWidth )
						return HTBOTTOMRIGHT;
					else
						return HTBOTTOM;
				}
				else if ( clientMousePos.y <= borderWidth )
				{
					if ( clientMousePos.x <= borderWidth )
						return HTTOPLEFT;
					else if ( clientMousePos.x >= windowRect.right - borderWidth )
						return HTTOPRIGHT;
					else
						return HTTOP;
				}
				else if ( clientMousePos.x <= borderWidth )
				{
					return HTLEFT;
				}
				else if ( clientMousePos.x >= windowRect.right - borderWidth )
				{
					return HTRIGHT;
				}

				const int titleHeight = 32;                     // your TitleBarHeight
				const int buttonsWidth = 3 * 46;                 // close/max/min width

				break;
			}
			case WM_NCACTIVATE:
			{
				// Prevent non-client area from being redrawn during window activation
				return TRUE;
			}
		}

		return CallWindowProc( OriginalProc, hWnd, uMsg, wParam, lParam );
	}

#endif // CONFIG_PLATFORM_WINDOWS

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

		ASSERT( false, "Unknown mouse button!" );
		return EInputMouseButton::None;
	}

	static EInputKey GLFWKeyToTridiumKey( int a_Key )
	{
		return static_cast<EInputKey>( a_Key );
	}

	static void SetCurrentGLFWContext( GLFWwindow* a_Window )
	{
		static GLFWwindow* s_CurrentContext = nullptr;

		if ( s_CurrentContext != a_Window )
		{
			glfwMakeContextCurrent( a_Window );
			s_CurrentContext = a_Window;
		}
	}

	class PlatformDisplay_GLFW : public IPlatformDisplay
	{
	public:

		GLFWmonitor* Monitor = nullptr;

		Pair< float, float > GetContentScale() const override
		{
			float XScale = 1.0f, YScale = 1.0f;
			glfwGetMonitorContentScale( Monitor, &XScale, &YScale );
			return { XScale, YScale };
		}

		Pair< uint32_t, uint32_t > GetSize() const override
		{
			int Width = 0, Height = 0;
			glfwGetMonitorWorkarea( Monitor, nullptr, nullptr, &Width, &Height );
			return { static_cast<uint32_t>( Width ), static_cast<uint32_t>( Height ) };
		}

		Pair< int32_t, int32_t > GetPosition() const override
		{
			int X = 0, Y = 0;
			glfwGetMonitorPos( Monitor, &X, &Y );
			return { static_cast<int32_t>( X ), static_cast<int32_t>( Y ) };
		}

		StringView GetName() const override
		{
			const char* Name = glfwGetMonitorName( Monitor );
			return Name ? StringView( Name ) : StringView();
		}

		bool IsPrimary() const override
		{
			return glfwGetPrimaryMonitor() == Monitor;
		}

		void* GetNativeDisplay() const override
		{
			return Monitor;
		}

		//======================================================================
		PlatformDisplay_GLFW( GLFWmonitor* a_Monitor )
			: Monitor( a_Monitor )
		{
		}

		//======================================================================
		~PlatformDisplay_GLFW() override = default;

	};

	class PlatformWindow_GLFW : public IPlatformWindow
	{
	public:

		//======================================================================
		GLFWwindow* NativeWindow;
		String Title;
		bool VSyncEnabled;
		Delegate<void( const Event& )> EventCallback;

		//======================================================================
		void* GetNativeWindow() const override
		{
			return NativeWindow;
		}

		//======================================================================
		Pair< int32_t, int32_t > GetSize() const override
		{
			int32_t Width = 0, Height = 0;
			glfwGetWindowSize( NativeWindow, &Width, &Height );
			return { Width, Height };
		}

		//======================================================================
		Pair< int32_t, int32_t > GetPosition() const override
		{
			int X = 0, Y = 0;
			glfwGetWindowPos( NativeWindow, &X, &Y );
			return { static_cast<int32_t>( X ), static_cast<int32_t>( Y ) };
		}

		//======================================================================
		bool IsVSyncEnabled() const override
		{
			return VSyncEnabled;
		}

		//======================================================================
		bool IsMinimised() const override
		{
			return glfwGetWindowAttrib( NativeWindow, GLFW_ICONIFIED ) == GLFW_TRUE;
		}

		//======================================================================
		bool IsMaximised() const override
		{
			return glfwGetWindowAttrib( NativeWindow, GLFW_MAXIMIZED ) == GLFW_TRUE;
		}

		//======================================================================
		bool IsFullscreen() const override
		{
			return glfwGetWindowMonitor( NativeWindow ) != nullptr;
		}

		//======================================================================
		void SetSize( uint32_t a_Width, uint32_t a_Height ) override
		{
			glfwSetWindowSize( NativeWindow, static_cast<int>( a_Width ), static_cast<int>( a_Height ) );
		}

		//======================================================================
		void SetPosition( int32_t a_X, int32_t a_Y ) override
		{
			glfwSetWindowPos( NativeWindow, a_X, a_Y );
		}

		//======================================================================
		void SetVSync( bool a_Enabled ) override
		{
			SetCurrentGLFWContext( NativeWindow );
			glfwSwapInterval( a_Enabled ? 1 : 0 );
			VSyncEnabled = a_Enabled;
		}

		//======================================================================
		void SetTitle( const String& a_Title ) override
		{
			Title = a_Title;
			glfwSetWindowTitle( NativeWindow, Title.c_str() );
		}

		//======================================================================
		void SetIcon( PlatformWindowIcon a_Icon ) override
		{
			SetCurrentGLFWContext( NativeWindow );

			if ( a_Icon.Pixels != nullptr )
			{
				GLFWimage Image{};
				Image.width = static_cast<int>( a_Icon.Width );
				Image.height = static_cast<int>( a_Icon.Height );
				Image.pixels = a_Icon.Pixels;
				glfwSetWindowIcon( NativeWindow, 1, &Image );
			}
			else
			{
				glfwSetWindowIcon( NativeWindow, 0, nullptr );
			}
		}

		//======================================================================
		void SetFullscreen( bool a_FullscreenEnabled, IPlatformDisplay* a_Display = nullptr ) override
		{
			//glfwGetMonitor
			GLFWmonitor* Monitor = a_Display
				? static_cast<PlatformDisplay_GLFW*>( a_Display )->Monitor
				: glfwGetPrimaryMonitor();

			if ( a_FullscreenEnabled )
			{
				glfwSetWindowMonitor(
					NativeWindow,
					Monitor,
					0, 0,  // x, y position
					glfwGetVideoMode( Monitor )->width,
					glfwGetVideoMode( Monitor )->height,
					glfwGetVideoMode( Monitor )->refreshRate
				);
			}
			else
			{
				// Set the window to windowed mode
				int MonitorWidth = 800, MonitorHeight = 600; // Default size
				glfwGetMonitorWorkarea( Monitor, nullptr, nullptr, &MonitorWidth, &MonitorHeight );
				int WindowWidth = int( MonitorWidth * 0.8 ); // 80% of monitor width
				int WindowHeight = int( MonitorHeight * 0.8 ); // 80% of monitor height
				int WindowX = ( MonitorWidth - WindowWidth ) / 2; // Center the window
				int WindowY = ( MonitorHeight - WindowHeight ) / 2; // Center the window
				glfwSetWindowMonitor( NativeWindow, nullptr, WindowX, WindowY, WindowWidth, WindowHeight, GLFW_DONT_CARE );
			}
		}

		//======================================================================
		void Restore() override
		{
			glfwRestoreWindow( NativeWindow );
		}

		//======================================================================
		void Minimise() override
		{
			glfwIconifyWindow( NativeWindow );
		}

		//======================================================================
		void Maximise() override
		{
			glfwMaximizeWindow( NativeWindow );
		}

		//======================================================================
		void Close() override
		{
			glfwSetWindowShouldClose( NativeWindow, GLFW_TRUE );
		}

		//======================================================================
		PlatformWindow_GLFW( const PlatformWindowProps& a_Props, IPlatformDisplay* a_Display = nullptr )
		{
			EventCallback = a_Props.EventCallback;

			PlatformDisplay_GLFW* Display = static_cast<PlatformDisplay_GLFW*>( a_Display );

			glfwDefaultWindowHints();

			glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
			glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
			glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );

			// TODO: Make these dependant on RHI backend somehow?
			glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
			glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, CONFIG_DEBUG );

			glfwWindowHint( GLFW_MAXIMIZED, a_Props.Maximised ? GLFW_TRUE : GLFW_FALSE );
			glfwWindowHint( GLFW_DECORATED, a_Props.Decorated ? GLFW_TRUE : GLFW_FALSE );
			glfwWindowHint( GLFW_VISIBLE, a_Props.Visible ? GLFW_TRUE : GLFW_FALSE );
			glfwWindowHint( GLFW_SAMPLES, a_Props.Samples );

			// Set pixel format hints
			glfwWindowHint( GLFW_FLOATING, a_Props.PixelFormat.FloatingPoint ? GLFW_TRUE : GLFW_FALSE );
			glfwWindowHint( GLFW_RED_BITS, a_Props.PixelFormat.RedBits );
			glfwWindowHint( GLFW_GREEN_BITS, a_Props.PixelFormat.GreenBits );
			glfwWindowHint( GLFW_BLUE_BITS, a_Props.PixelFormat.BlueBits );
			glfwWindowHint( GLFW_ALPHA_BITS, a_Props.PixelFormat.AlphaBits );

			NativeWindow = glfwCreateWindow(
				static_cast<int>( a_Props.Width ),
				static_cast<int>( a_Props.Height ),
				a_Props.Title.c_str(),
				Display ? Display->Monitor : nullptr,
				nullptr  // No sharing
			);

			glfwSetWindowUserPointer( NativeWindow, this );

			SetIcon( a_Props.Icon );
			SetTitle( a_Props.Title );
			SetVSync( a_Props.VSync );

			// Set GLFW Callbacks
			{
				glfwSetWindowSizeCallback( NativeWindow, []( GLFWwindow* a_Window, int a_Width, int a_Height )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					window->EventCallback( WindowResizeEvent( a_Width, a_Height ) );
				} );

				glfwSetWindowCloseCallback( NativeWindow, []( GLFWwindow* a_Window )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					window->EventCallback( WindowCloseEvent{} );
				} );

				glfwSetKeyCallback( NativeWindow, []( GLFWwindow* a_Window, int a_Key, int a_Scancode, int a_Action, int a_Mods )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					switch ( a_Action )
					{
						case GLFW_PRESS:
						{
							window->EventCallback( KeyPressedEvent{ Cast<EInputKey>( a_Key ), 0 } );
							break;
						}
						case GLFW_RELEASE:
						{
							window->EventCallback( KeyReleasedEvent{ Cast<EInputKey>( a_Key ) } );
							break;
						}
						case GLFW_REPEAT:
						{
							window->EventCallback( KeyPressedEvent{ Cast<EInputKey>( a_Key ), 1 } );
							break;
						}
					}
				} );

				glfwSetCharCallback( NativeWindow, []( GLFWwindow* a_Window, unsigned int a_Character )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					KeyTypedEvent event( static_cast<EInputKey>( a_Character ) );
					window->EventCallback( event );
				} );

				glfwSetMouseButtonCallback( NativeWindow, []( GLFWwindow* a_Window, int a_Button, int a_Action, int a_Mods )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					switch ( a_Action )
					{
						case GLFW_PRESS:
						{
							EInputMouseButton key = TranslateMouseButton( a_Button );
							window->EventCallback( MouseButtonPressedEvent( key ) );
							break;
						}
						case GLFW_RELEASE:
						{
							EInputMouseButton key = TranslateMouseButton( a_Button );
							MouseButtonReleasedEvent event( key );
							window->EventCallback( event );
							break;
						}
					}
				} );

				glfwSetScrollCallback( NativeWindow, []( GLFWwindow* a_Window, double a_XOffset, double a_YOffset )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					MouseScrolledEvent event( (float)a_XOffset, (float)a_YOffset );
					window->EventCallback( event );
					Input::_SetMouseScrollOffset( (int)a_XOffset, (int)a_YOffset );
				} );

				glfwSetCursorPosCallback( NativeWindow, []( GLFWwindow* a_Window, double a_XPos, double a_YPos )
				{
					auto* window = (PlatformWindow_GLFW*)glfwGetWindowUserPointer( a_Window );
					if ( !window->EventCallback )
						return;

					MouseMovedEvent event( (float)a_XPos, (float)a_YPos );
					window->EventCallback( event );
				} );
			}

		#if CONFIG_PLATFORM_WINDOWS
			// Windows hack to allow resizing of non-decorated windows.
			if ( !a_Props.Decorated )
			{
				HWND hWnd = glfwGetWin32Window( NativeWindow );

				LONG_PTR lStyle = GetWindowLongPtr( hWnd, GWL_STYLE );
				lStyle |= WS_THICKFRAME | /*WS_CAPTION | */WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
				SetWindowLongPtr( hWnd, GWL_STYLE, lStyle );

				RECT windowRect;
				GetWindowRect( hWnd, &windowRect );
				const int Width = windowRect.right - windowRect.left;
				const int Height = windowRect.bottom - windowRect.top;

				OriginalProc = (WNDPROC)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
				SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( WindowProc ) );

				SetWindowPos( hWnd, NULL, 0, 0, Width, Height, SWP_FRAMECHANGED | SWP_NOMOVE );
			}
		#endif // CONFIG_PLATFORM_WINDOWS
		}

		~PlatformWindow_GLFW() override
		{
			glfwDestroyWindow( NativeWindow );
		}
	};

	class PlatformInterface_GLFWImpl : public IPlatformInterface
	{
	public:

		UniquePtr< PlatformDisplay_GLFW > PrimaryDisplay = nullptr;

		EPlatformType GetPlatformType() const override
		{
		#if CONFIG_PLATFORM_WINDOWS
			return EPlatformType::Windows;
		#elif CONFIG_PLATFORM_LINUX
			return EPlatformType::Linux;
		#elif CONFIG_PLATFORM_MACOS
			return EPlatformType::MacOS;
		#else
			#error "Unsupported platform for GLFW implementation."
		#endif
		}

		UniquePtr< IPlatformWindow > CreateWindow( const PlatformWindowProps& a_Props, IPlatformDisplay* a_Display ) override
		{
			return MakeUnique< PlatformWindow_GLFW >( a_Props, a_Display );
		}

		IPlatformDisplay* GetPrimaryDisplay() override
		{
			GLFWmonitor* PrimaryMonitor = glfwGetPrimaryMonitor();
			if ( PrimaryMonitor == nullptr )
			{
				LOG( LogCategory::Application, Warn, "Failed to get primary monitor." );
				return nullptr;
			}

			if ( PrimaryDisplay == nullptr )
			{
				PrimaryDisplay = MakeUnique< PlatformDisplay_GLFW >( PrimaryMonitor );
				return PrimaryDisplay.get();
			}

			if ( PrimaryDisplay->Monitor != PrimaryMonitor )
			{
				PrimaryDisplay = MakeUnique< PlatformDisplay_GLFW >( PrimaryMonitor );
			}

			return PrimaryDisplay.get();
		}

		void PollEvents() override
		{
			glfwPollEvents();
		}

		bool Setup( const PlatformSettings& a_Settings ) override
		{
			// Initialize GLFW for Windows platform
			if ( int Success = glfwInit(); Success == GLFW_FALSE )
			{
				return false;
			}

			// Set the error callback for GLFW
			{
				const auto ErrorCallback = +[]( int a_Error, const char* a_Description )
				{
					LOG( LogCategory::Application, Error, "GLFW Error ({0}): {1}", a_Error, a_Description );
				};

				glfwSetErrorCallback( ErrorCallback );
			}

			return true;
		}

		void Teardown() override
		{
			glfwTerminate();
		}
	};

	UniquePtr<IPlatformInterface> CreatePlatformInterface_GLFW()
	{
		return MakeUnique< PlatformInterface_GLFWImpl >();
	}

} // namespace Tridium

#endif // CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS