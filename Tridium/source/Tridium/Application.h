#pragma once

#include "Core/Core.h"
#include "Window.h"

#include <Tridium/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>

namespace Tridium
{
	class TRIDIUM_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent( Event& e );

		void PushLayer( Layer* layer );
		void PushOverlay( Layer* overlay );

	private:
		bool OnWindowClosed( WindowCloseEvent& e );

		std::unique_ptr<Window> m_Window;
		bool m_Running;
		LayerStack m_LayerStack;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

