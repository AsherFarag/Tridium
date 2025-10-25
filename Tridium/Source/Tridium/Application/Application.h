#pragma once
#include <Tridium/Application/Layer.h>
#include <Tridium/Application/Window.h>
#include <Tridium/Core/CommandLine.h>
#include <Tridium/Common/Function.h>
#include <Tridium/Events/Event.h>
#include <Tridium/Utils/StaticInitializer.h>

namespace Tridium {

	// Forward Declarations
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

	//=================================================================================================
	// Defines a tick group that can be used
	// to register callbacks that will be invoked during the application loop.
	#define DEFINE_TICK_GROUP( _Name ) \
		DECLARE_INITIALIZER( _Name ) \
		namespace TickGroups { \
			constexpr HashedString _Name = StringView{ #_Name }; \
		} // namespace TickGroups

	#define REGISTER_TICK_GROUP_0( _Name ) \
		DEFINE_INITIALIZER( _Name ) \
		{ \
			::Tridium::Application::RegisterTickGroup( TickGroups::_Name, {} ); \
		}

	#define REGISTER_TICK_GROUP_1( _Name, ... ) \
		DEFINE_INITIALIZER( _Name ) \
		{ \
			::Tridium::Application::RegisterTickGroup( TickGroups::_Name, { __VA_ARGS__ } ); \
		}

	//=================================================================================================
	// Registers a tick group that has been defined with DEFINE_TICK_GROUP.
	// Parameters:
	//   - Name: The name of the tick group to register.
	//   - ...: Optional prerequisites for the tick group. e.g. `REGISTER_TICK_GROUP( MyEndTick, "MyBeginTick" )`
	#define REGISTER_TICK_GROUP( ... ) EXPAND( SELECT_MACRO_2( __VA_ARGS__, REGISTER_TICK_GROUP_1, REGISTER_TICK_GROUP_0 )(__VA_ARGS__) )

	DEFINE_TICK_GROUP( BeginTick );
	DEFINE_TICK_GROUP( BeginAppUpdate );
	DEFINE_TICK_GROUP( EndAppUpdate );
	DEFINE_TICK_GROUP( EndTick );

	//=================================================================================================
	// Application: The core system that manages interactions between the engine and the OS.
	// It is responsible for initializing the engine, creating the window, 
	// and running the engine loop.
	//=================================================================================================
	class Application final
	{
	public:

		//=============================================================================================
		Application( CmdLineArgs a_ProjectPath );
		~Application();

		//=============================================================================================
		// The starting point of the application.
		// This handles the initialization, game loop and shutdown stage of the engine.
		EAppExitCode Run();

		//=================================================================================================
		// Gets the static application instance.
		static Application* Get() { ASSERT( s_Instance ); return s_Instance; }
		static const CmdLineArgs& GetCommandLineArgs() { return Get()->m_CommandLineArgs; }
		static Window& GetWindow() { return *Get()->m_Window; }
		static uint32_t GetFPS() { return Get()->m_PrevFrameInfo.FPS; }
		static double GetFrameTime() { return 1000.0 / Get()->m_PrevFrameInfo.FPS; }
		static const FrameInfo& GetFrameInfo() { return Get()->m_PrevFrameInfo; }
		static AppLayerStack& GetLayerStack() { return Get()->m_LayerStack; }

		//=================================================================================================
		// Notifies the application to exit the main loop and shutdown.
		static void RequestExit( EAppExitCode a_ExitCode );

		//=============================================================================================
		// Event Handling
		static void EnqueueEvent( const Event& a_Event ) { Get()->m_EventQueue.emplace( a_Event ); }

		//=============================================================================================
		static void RegisterTickGroup( const HashedString& a_TickGroup, InitList< HashedString > a_Prerequisites = {} ) 
		{
			GetTickGroups()[ a_TickGroup ] = TickGroup{ a_TickGroup, { a_Prerequisites }, {} }; 
		}

		//=============================================================================================
		template<typename _OnTick>
		static void AddOnTick( const HashedString& a_TickGroup, _OnTick&& a_OnTick )
		{
			ENSURE( GetTickGroups().contains( a_TickGroup ), "Tick group '{}' does not exist!", a_TickGroup.String() );
			GetTickGroups()[ a_TickGroup ].Callbacks.EmplaceBack( std::forward<_OnTick>( a_OnTick ) );
		}

	protected:

		//=============================================================================================
		static Application* s_Instance;

		//=============================================================================================
		bool              m_Running = false;
		EAppExitCode      m_ExitCode = EAppExitCode::Success;
		CmdLineArgs       m_CommandLineArgs{};
		UniquePtr<Window> m_Window = nullptr;
		AppLayerStack        m_LayerStack{};
		FrameInfo         m_PrevFrameInfo{};
		uint32_t          m_MaxFPS = 144u;
		Queue<Event>      m_EventQueue;
		UniquePtr<Engine> m_Engine = nullptr;

		struct TickGroup
		{
			HashedString Name;
			Array<HashedString> Prerequisites;
			Array<Delegate<void()>> Callbacks;
		};

		static MulticastDelegate<void()> s_TickCallback;

	protected:

		//=============================================================================================
		bool OnWindowResized( const WindowResizeEvent& a_Event );
		bool OnWindowClosed( const WindowCloseEvent& a_Event );
		void FlushEventQueue();

		//=============================================================================================
		void SetUpTickGroups();
		static UnorderedMap<HashedString, TickGroup>& GetTickGroups();

	};

} // namespace Tridium