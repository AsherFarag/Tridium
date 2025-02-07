#include "tripch.h"
#include "Engine.h"

#include <Tridium/Debug/DebugDrawer.h>

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	//
	// PUBLIC FUNCTIONS
	//
	//////////////////////////////////////////////////////////////////////////

	IEngineModule* Engine::GetModule( hash_t a_TypeHash )
	{
		auto it = m_EngineModules.find( a_TypeHash );
		if ( it != m_EngineModules.end() )
			return it->second.get();
		return nullptr;
	}



	//////////////////////////////////////////////////////////////////////////
	//
	// PRIVATE FUNCTIONS
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Engine Initialization
	//////////////////////////////////////////////////////////////////////////

	bool Engine::Init()
	{
		// Initialize Engine Modules
		if ( !InitModules( EEngineInitStage::PreEngineInit ) )
		{
			TE_CORE_ERROR( "Engine::Init: Failed to initialize engine modules" );
			return false;
		}

		// Initialize Scene Manager
		SceneManager::Singleton::Construct();

		// Initialize Game Instance
		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

		// Initialize Engine Modules
		if ( !InitModules( EEngineInitStage::PostEngineInit ) )
		{
			TE_CORE_ERROR( "Engine::Init: Failed to initialize engine modules" );
			return false;
		}

		return true;
	}

	bool Engine::InitModules( EEngineInitStage a_InitStage )
	{
		for ( auto&& [key, module] : EngineModuleFactory::ModuleTypes() )
		{
			if ( module.InitStage == a_InitStage )
			{
				UniquePtr<IEngineModule> engineModule = module.Create();
				Optional<EngineModuleError> error = engineModule->Init();
				if ( error )
				{
					switch ( error->Level )
					{
					case EngineModuleError::ELevel::FatalError:
						TE_CORE_ERROR( "Engine::InitModules: Fatal error initializing module: {0} - Msg: '{1}'", module.Name, error->Message.c_str() );
						TODO( "Handle fatal error" );
						return false;
					case EngineModuleError::ELevel::Error:
						TE_CORE_WARN( "Engine::InitModules: Error initializing module: {0} - Msg: '{1}'", module.Name, error->Message.c_str() );
						break;
					default:
						ASSERT( false, "Unknown error level" );
						break;
					}
				}
			}
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Engine Shutdown
	//////////////////////////////////////////////////////////////////////////

	void Engine::Shutdown()
	{
		ShutdownModules( EEngineInitStage::PostEngineInit );

		// Shutdown Game Instance
		m_GameInstance->Shutdown();

		// Shutdown Engine Modules
		ShutdownModules( EEngineInitStage::PreEngineInit );
	}

	bool Engine::ShutdownModules( EEngineInitStage a_Stage )
	{
		for ( auto&& [key, module] : EngineModuleFactory::ModuleTypes() )
		{
			if ( module.InitStage == a_Stage )
			{
				UniquePtr<IEngineModule> engineModule = module.Create();
				Optional<EngineModuleError> error = engineModule->Shutdown();
				if ( error )
				{
					switch ( error->Level )
					{
					case EngineModuleError::ELevel::FatalError:
						TE_CORE_ERROR( "Engine::ShutdownModules: Fatal error shutting down module: {0} - Msg: '{1}'", module.Name, error->Message.c_str() );
						TODO( "Handle fatal error" );
						return false;
					case EngineModuleError::ELevel::Error:
						TE_CORE_WARN( "Engine::ShutdownModules: Error shutting down module: {0} - Msg: '{1}'", module.Name, error->Message.c_str() );
						break;
					default:
						ASSERT( false, "Unknown error level" );
						break;
					}
				}
			}
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
