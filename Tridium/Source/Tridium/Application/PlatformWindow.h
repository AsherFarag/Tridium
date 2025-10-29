#pragma once
#include <Tridium/Application/Event.h>
#include <Tridium/Common/Function.h>
#include <Tridium/Containers/Tuple.h>
#include <Tridium/Containers/String.h>

namespace Tridium {

	//=================================================================================================
	// Interface that represents a display (monitor) on the platform.
	//=================================================================================================
	class IPlatformDisplay
	{
	public:

		//=============================================================================================
		// Gets the content scale for the specified monitor. The content scale
		// is the ratio between the current DPI and the platform's default DPI.
		virtual Pair< float, float > GetContentScale() const = 0;

		//=============================================================================================
		// Gets the size of the display in pixels.
		virtual Pair< uint32_t, uint32_t > GetSize() const = 0;

		//=============================================================================================
		// Gets the position of display's viewport on the virtual screen.
		virtual Pair< int32_t, int32_t > GetPosition() const = 0;

		//=============================================================================================
		// Gets the name of the display.
		virtual StringView GetName() const = 0;

		//=============================================================================================
		// Is this display the primary display for the system?
		virtual bool IsPrimary() const = 0;

		//=============================================================================================
		// Gets a handle to the underlying native interface. (e.g. GLFWmonitor*)
		virtual void* GetNativeDisplay() const = 0;

	protected:

		//=============================================================================================
		IPlatformDisplay() = default;
		virtual ~IPlatformDisplay() = default;

	};

	//=================================================================================================
	// Data that can be used to set the icon for a platform window.
	// The icon is expected to be in RGBA 32-bit format.
	// The width and height should be a power of two,
	// typically 16x16, 32x32, or 64x64 pixels.
	//=================================================================================================
	struct PlatformWindowIcon
	{
		uint16_t Width = 0;        // Width of the icon in pixels.
		uint16_t Height = 0;       // Height of the icon in pixels.
		uint8_t* Pixels = nullptr; // Expected to be in RGBA 32-bit format.
	};

	//=================================================================================================
	// Creation parameters used for creating a platform window.
	//=================================================================================================
	struct PlatformWindowProps
	{
		String Title{};                         // The title of the window.
		PlatformWindowIcon Icon{};              // The window icon shown in the title bar and taskbar.
		uint32_t Width = 0; 		            // Width of the window in pixels.
		uint32_t Height = 0;                    // Heigh of the window in pixels.
		bool VSync = true; 	                    // Should the window be created with VSync enabled?
		bool Visible = true;                    // Should the window be visible after creation?
		bool Maximised = false;                 // Should the window be created in fullscreen?
		bool Decorated = true;                  // Should the window be created with a title bar and borders?
		uint8_t Samples = 1;                    // Samples used for MSAA. Valid values are 2x, 4x, 8x
		Delegate<void( const Event& )> EventCallback; // Callback function for window events.

		//=============================================================================================
		// Pixel format for the backbuffers of the window.
		struct
		{
			bool FloatingPoint = false; // Store each pixel channel as a floating point value.
			uint8_t RedBits = 8;        // Number of bits for the red channel.
			uint8_t GreenBits = 8;      // Number of bits for the green channel.
			uint8_t BlueBits = 8;       // Number of bits for the blue channel.
			uint8_t AlphaBits = 8;      // Number of bits for the alpha channel.
		} PixelFormat;
	};

	//=================================================================================================
	// Interface for a 'Window' on the platform.
	//=================================================================================================
	class IPlatformWindow
	{
	public:

		//=============================================================================================
		IPlatformWindow() = default;
		virtual ~IPlatformWindow() = default;

		//=============================================================================================
		virtual void* GetNativeWindow() const = 0;

		//=============================================================================================
		virtual Pair< int32_t, int32_t > GetSize() const = 0;

		//=============================================================================================
		virtual Pair< int32_t, int32_t > GetPosition() const = 0;

		//=============================================================================================
		virtual bool IsVSyncEnabled() const = 0;

		//=============================================================================================
		virtual bool IsMinimised() const = 0;

		//=============================================================================================
		virtual bool IsMaximised() const = 0;

		//=============================================================================================
		virtual bool IsFullscreen() const = 0;

		//=============================================================================================
		virtual void SetSize( uint32_t a_Width, uint32_t a_Height ) = 0;

		//=============================================================================================
		virtual void SetPosition( int32_t a_X, int32_t a_Y ) = 0;

		//=============================================================================================
		virtual void SetVSync( bool a_Enabled ) = 0;

		//=============================================================================================
		virtual void SetTitle( const String& a_Title ) = 0;

		//=============================================================================================
		virtual void SetIcon( PlatformWindowIcon a_Icon ) = 0;

		//=============================================================================================
		virtual void SetFullscreen( bool a_FullscreenEnabled, IPlatformDisplay* a_Display = nullptr ) = 0;

		//=============================================================================================
		virtual void Restore() = 0;

		//=============================================================================================
		virtual void Minimise() = 0;

		//=============================================================================================
		virtual void Maximise() = 0;

		//=============================================================================================
		virtual void Close() = 0;

	};

} // namespace Tridium