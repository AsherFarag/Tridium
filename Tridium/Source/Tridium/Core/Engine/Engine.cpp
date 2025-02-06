#include "tripch.h"
#include "Engine.h"

#include <Tridium/Debug/DebugDrawer.h>

// Subsystems
#include <Tridium/Physics/PhysicsSubsystem.h>

namespace Tridium {

	bool Engine::Initialize()
	{
		if ( !CORE_ASSERT( Singleton::IsValid() ) )
			return false;

		InitSubsystems();

		// Initialize Scene Manager
		{
			SceneManager::Singleton::Construct();
		}

		return true;
	}

	void Engine::Shutdown()
	{
		ShutdownSubsystems();
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Subsystems
	//////////////////////////////////////////////////////////////////////////

	void Engine::InitSubsystems()
	{
		RegisterSubsystems();

		for ( auto&& [key, subsystem] : m_SubsystemManager.GetSubsystems() )
		{
			SubsystemStatus status = subsystem->Initialize();

			switch ( status.Status )
			{
			case ESubsystemStatusType::Success:
				TE_CORE_INFO( "Engine::InitSubsystems: Initialized subsystem: {0}", subsystem->GetName() );
				break;
			case ESubsystemStatusType::Failure:
				TE_CORE_WARN( "Engine::InitSubsystems: Failed to initialize subsystem: {0} - Msg: '{1}'", subsystem->GetName(), status.Message.c_str() );
				break;
			case ESubsystemStatusType::FatalError:
				TE_CORE_ERROR( "Engine::InitSubsystems: Fatal error initializing subsystem: {0} - Msg: '{1}'", subsystem->GetName(), status.Message.c_str() );
				TODO( "Handle fatal error" );
				break;
			}
		}
	}
	 
	void Engine::ShutdownSubsystems()
	{
		for ( auto&& [key, subsystem] : m_SubsystemManager.GetSubsystems() )
		{
			subsystem->Shutdown();
		}
	}

	void Engine::RegisterSubsystems()
	{
		m_SubsystemManager.RegisterSubsystem<PhysicsSubsystem>();
	}

	//////////////////////////////////////////////////////////////////////////
} // namespace Tridium
