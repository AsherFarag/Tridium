#pragma once

#include "tripch.h"

#include "Tridium/Core/Core.h"
#include "Tridium/Events/Event.h"

#include <sstream>

namespace Tridium {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps( const std::string& title = "Application",
			uint32_t width = 1280,
			uint32_t height = 720 )
			: Title( title ), Width( width ), Height( height )
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void( Event& )>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool IsMinimized() const = 0;

		// Window attributes
		virtual void SetEventCallback( const EventCallbackFn& callback ) = 0;
		virtual void SetVSync( bool enabled ) = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetTitle( const std::string& title ) = 0;
		virtual void SetIcon( const std::string& path ) = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void MakeContextCurrent( Window* window ) = 0;

		// Implemented in platform-specific child classes
		static UniquePtr<Window> Create( const WindowProps& props = WindowProps() );
	};

}