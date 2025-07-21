#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Core/CommandLine.h>
#include <Tridium/Events/Event.h>
#include <Tridium/Scene/Scene.h>

#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	// Forward Declarations
	class AssetManagerBase;
	class Engine;
	class ImGuiLayer;

	struct FrameInfo
	{
		uint32_t FPS = 0u;
		uint32_t MinFPS = 0u;
		uint32_t MaxFPS = 0u;
	};

	enum class EAppExitCode
	{
		Success = 0,
		Failure = 1,
	};

	//==============================================
	// Application
	//  The core system that manages interactions between the engine and the OS.
	//  It is responsible for initializing the engine, creating the window, 
	//  and running the engine loop.
	class Application
	{
	public:
		static Application* Get() { ASSERT( s_Instance ); return s_Instance; }

		static void RequestExit( EAppExitCode a_ExitCode );

		//================================================================
		// Event Handling
		static void EnqueueEvent( const Event& a_Event ) { Get()->m_EventQueue.emplace( a_Event ); }
		//================================================================

		//================================================================
		// Layer Stack
		template<Concepts::Derived<Layer> _Layer, typename... _Args>
		static _Layer* PushLayer( _Args&&... a_Args );
		template<Concepts::Derived<Layer> _Layer, typename... _Args>
		static _Layer* PushOverlay( _Args&&... a_Args );
		static void PopLayer( Layer* a_Layer, bool a_Destroy = true ) { Get()->m_LayerStack.PopLayer( a_Layer, a_Destroy ); }
		static void PopOverlay( Layer* a_Overlay, bool a_Destroy = true ) { Get()->m_LayerStack.PopOverlay( a_Overlay, a_Destroy ); }
		//================================================================

		static const CmdLineArgs& GetCommandLineArgs() { return Get()->m_CommandLineArgs; }
		static Window& GetWindow() { return *Get()->m_Window; }
		static uint32_t GetFPS() { return Get()->m_PrevFrameInfo.FPS; }
		static double GetFrameTime() { return 1000.0 / Get()->m_PrevFrameInfo.FPS; }
		static const FrameInfo& GetFrameInfo() { return Get()->m_PrevFrameInfo; }

	public:
		Application( CmdLineArgs a_ProjectPath );
		~Application();

		// The starting point of the application.
		// This handles the initialization, game loop and shutdown stage of the engine.
		virtual EAppExitCode Run();

	protected:
		bool              m_Running = false;
		EAppExitCode      m_ExitCode = EAppExitCode::Success;
		CmdLineArgs       m_CommandLineArgs{};
		UniquePtr<Window> m_Window = nullptr;
		LayerStack        m_LayerStack{};
		FrameInfo         m_PrevFrameInfo{};
		uint32_t          m_MaxFPS = 144u;
		Queue<Event>      m_EventQueue;
		UniquePtr<Engine> m_Engine = nullptr;

	protected:
		bool OnWindowResized( const WindowResizeEvent& a_Event );
		bool OnWindowClosed( const WindowCloseEvent& a_Event );

		virtual void OnUpdate();

		void FlushEventQueue();

		static Application* s_Instance;
	};

	template<Concepts::Derived<Layer> _Layer, typename ..._Args>
	inline _Layer* Application::PushLayer( _Args && ...a_Args )
	{
		_Layer* layer = new _Layer( std::forward<_Args>( a_Args )... );
		Get()->m_LayerStack.PushLayer( layer );
		return layer;
	}

	template<Concepts::Derived<Layer> _Layer, typename... _Args>
	inline _Layer* Application::PushOverlay( _Args&&... a_Args )
	{
		_Layer* layer = new _Layer( std::forward<_Args>( a_Args )... );
		Get()->m_LayerStack.PushOverlay( layer );
		return layer;
	}


}

