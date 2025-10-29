#include "tripch.h"
#include "Application.h"
#include <Tridium/Engine/Engine.h>
#include <Tridium/Editor/Editor.h>
#include <Tridium/Runtime/Runtime.h>
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	REGISTER_TICK_GROUP( BeginTick );
	REGISTER_TICK_GROUP( BeginAppUpdate );
	REGISTER_TICK_GROUP( EndAppUpdate, "BeginAppUpdate"_H );
	REGISTER_TICK_GROUP( EndTick );

	decltype( Application::s_Instance ) Application::s_Instance = nullptr;
	decltype( Application::s_TickCallback ) Application::s_TickCallback{};

	void Application::RequestExit( EAppExitCode a_ExitCode )
	{
		Get()->m_ExitCode = a_ExitCode;
		Get()->m_Running = false;
	}

	Application::Application( CmdLineArgs a_CmdLine )
	{
		ENSURE( !s_Instance, "An Application instance already exists!" );

		s_Instance = this;
		m_CommandLineArgs = std::move( a_CmdLine );

		PlatformSettings platformSettings;
		m_PlatformInterface = IPlatformInterface::Create( platformSettings );
		ENSURE( m_PlatformInterface, "Failed to create platform interface!" );

		// Create the main window
		PlatformWindowProps windowProps;
		windowProps.Width = 1280;
		windowProps.Height = 720;
		windowProps.EventCallback = +[]( const Event& a_Event ) { Application::Get()->EnqueueEvent(a_Event); };
	#if WITH_EDITOR
		windowProps.Decorated = false;
	#endif // WITH_EDITOR

		m_Window = m_PlatformInterface->CreateWindow( windowProps );
		ENSURE( m_Window, "Failed to create platform window!" );

		m_Window->SetPosition( 100, 100 );

		// Initialise the Engine
		EngineConfig engineConfig;
		m_LayerStack.EmplaceLayer<Engine>( engineConfig );

	#if WITH_EDITOR

		m_LayerStack.EmplaceOverlay<Editor>();

	#elif CONFIG_RUNTIME

		m_LayerStack.EmplaceOverlay<Runtime>();

	#endif // WITH_EDITOR

	}

	Application::~Application()
	{
		s_Instance = nullptr;
	}
	
	EAppExitCode Application::Run()
	{
		m_Running = true;

		uint32_t frameCounter = 0;
		double fpsInterval = 0.0;
		uint32_t minFPS = 0xFFFFFFFF;
		uint32_t maxFPS = 0;

		SetUpTickGroups();

		while ( m_Running )
		{
			PROFILE_FRAME();

			const double lastFrameTime = Time::Now();
			Time::Update();

			// Update FPS
			++frameCounter;
			fpsInterval += Time::DeltaTime();
			uint32_t curFrameRate = 1.0 / Time::DeltaTime();
			minFPS = Math::Min( minFPS, curFrameRate );
			maxFPS = Math::Max( maxFPS, curFrameRate );
			if ( fpsInterval >= 1.0 )
			{
				m_PrevFrameInfo.FPS = frameCounter;
				m_PrevFrameInfo.MinFPS = minFPS;
				m_PrevFrameInfo.MaxFPS = maxFPS;
				frameCounter = 0;
				fpsInterval = 0.0;
				minFPS = 0xFFFFFFFF;
				maxFPS = 0;
			}

			FlushEventQueue();

			// Invoke the tick groups
			s_TickCallback.Broadcast();

			m_PlatformInterface->PollEvents();
		}

		m_LayerStack = {};

		return m_ExitCode;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::FlushEventQueue()
	{
		while ( !m_EventQueue.empty() )
		{
			Event& event = m_EventQueue.front();

			EventDispatcher dispatcher( event );
			dispatcher.Dispatch<WindowCloseEvent>( [this]( const WindowCloseEvent& a_Event ) -> bool { return OnWindowClosed( a_Event ); } );
			dispatcher.Dispatch<WindowResizeEvent>( [this]( const WindowResizeEvent& a_Event ) -> bool { return OnWindowResized( a_Event ); } );

			m_LayerStack.OnEvent( event );

			m_EventQueue.pop();
		}
	}

	void Application::SetUpTickGroups()
	{
		s_TickCallback.Clear();

		struct TickGroupNode
		{
			bool Registered = false;
			HashedString Name;
			Array< TickGroupNode* > Dependents;
		};

		Map< HashedString, TickGroupNode > tickGroupNodes;
		Queue< TickGroupNode* > queue;

		// Add in BeginTick
		for ( const auto& tickFn : GetTickGroups()[ TickGroups::BeginTick ].Callbacks )
		{
			s_TickCallback.Add( tickFn );
		}

	#define LOG_TICK_GROUP( _Name ) LOG( LogCategory::Application, Info, "	- {}", _Name )

		LOG( LogCategory::Application, Info, "Setting up Tick Groups..." );
		// Log tick group order

		LOG( LogCategory::Application, Info, "Tick Group Order:" );
		LOG_TICK_GROUP( "BeginTick" );

		for ( const auto& [name, tickGroup] : GetTickGroups() )
		{
			if ( name == TickGroups::BeginTick || name == TickGroups::EndTick )
			{
				continue;
			}

			TickGroupNode& node = tickGroupNodes[ name ];

			node.Registered = true;
			node.Name = name;

			for ( const HashedString& Prereq : tickGroup.Prerequisites )
			{
				TickGroupNode& PrereqNode = tickGroupNodes[ Prereq ];

				PrereqNode.Registered = false;
				PrereqNode.Dependents.EmplaceBack( &node );
			}

			// If there are no prereqs, add to queue.
			// We want tick groups with no prerequisites to fire first.
			if ( tickGroup.Prerequisites.Empty() )
			{
				queue.push( &node );
			}
		}

		while ( !queue.empty() )
		{
			TickGroupNode* node = queue.front();
			queue.pop();

			LOG_TICK_GROUP( node->Name.String() );

			// We want to add to the delegate in the order of the tick groups.
			const auto it = GetTickGroups().find( node->Name );

			for ( const auto& tickFn : it->second.Callbacks )
			{
				s_TickCallback.Add( tickFn );
			}

			for ( TickGroupNode* Dependent : node->Dependents )
			{
				queue.push( Dependent );
			}
		}

		// Add in EndTick
		for ( const auto& tickFn : GetTickGroups()[ TickGroups::EndTick ].Callbacks )
		{
			s_TickCallback.Add( tickFn );
		}

		LOG_TICK_GROUP( "EndTick" );
	}

	bool Application::OnWindowResized( const WindowResizeEvent& a_Event )
	{
		if ( !RHI::GetDynamicRHI()->GetSwapChain() )
			return false;

		RHI::GetDynamicRHI()->GetSwapChain()->Resize( a_Event.Width, a_Event.Height );
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	bool Application::OnWindowClosed( const WindowCloseEvent& a_Event )
	{
		m_Running = false;
		return true;
	}

	UnorderedMap<HashedString, Application::TickGroup>& Application::GetTickGroups()
	{
		static UnorderedMap<HashedString, TickGroup> s_TickGroups{};
		return s_TickGroups;
	}

} // namespace Tridium