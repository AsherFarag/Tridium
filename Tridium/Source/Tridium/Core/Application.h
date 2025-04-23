#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Core/CommandLine.h>
#include <Tridium/Events/Event.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Graphics/Rendering/GameViewport.h>

#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	// Forward Declarations
	class AssetManagerBase;

	struct FrameInfo
	{
		uint32_t FPS = 0u;
		uint32_t MinFPS = 0u;
		uint32_t MaxFPS = 0u;
	};


	TODO( "Make this description actually true" );
	//==============================================
	// Application
	//  The core system that manages interactions between the engine and the OS.
	//  It is responsible for initializing the engine, creating the window, 
	//  and running the engine loop.
	//==============================================
	class Application final
	{
	public:
		Application( CmdLineArgs a_ProjectPath );
		~Application();

		static Application* Get() { return s_Instance; }

		// The starting point of the application.
		// This handles the initialization, game loop and shutdown stage of the engine.
		void Run();

		// Stops the application loop and enters the shutdown stage.
		void Quit();

		//================================================================
		// Event Handling
		void EnqueueEvent( const Event& a_Event ) { m_EventQueue.emplace( a_Event ); }
		//================================================================

		//================================================================
		// Layer Stack
		void PushLayer( Layer* a_Layer ) { m_LayerStack.PushLayer( a_Layer ); }
		void PushOverlay( Layer* a_Overlay ) { m_LayerStack.PushOverlay( a_Overlay ); }
		void PopLayer( Layer* a_Layer, bool a_Destroy = false ) { m_LayerStack.PopLayer( a_Layer, a_Destroy ); }
		void PopOverlay( Layer* a_Overlay, bool a_Destroy = false ) { m_LayerStack.PopOverlay( a_Overlay, a_Destroy ); }
		//================================================================

		const CmdLineArgs& GetCommandLineArgs() const { return m_CommandLineArgs; }
		Window& GetWindow() { return *m_Window; }
		uint32_t GetFPS() const { return m_PrevFrameInfo.FPS; }
		double GetFrameTime() const { return 1000.0 / m_PrevFrameInfo.FPS; }
		const FrameInfo& GetFrameInfo() const { return m_PrevFrameInfo; }

	private:
		bool              m_Running = false;
		CmdLineArgs       m_CommandLineArgs{};
		UniquePtr<Window> m_Window = nullptr;
		LayerStack        m_LayerStack{};
		GameViewport      m_GameViewport{};
		FrameInfo         m_PrevFrameInfo{};
		uint32_t          m_MaxFPS = 144u;
		Queue<Event>      m_EventQueue;

	private:
		bool OnWindowResized( const WindowResizeEvent& a_Event );
		bool OnWindowClosed( const WindowCloseEvent& a_Event );

		bool Init();
		void Update();
		void Shutdown();

		void FlushEventQueue();

		static Application* s_Instance;
	};
}

