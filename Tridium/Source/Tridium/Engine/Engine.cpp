#include "tripch.h"
#include "Engine.h"

#include <Tridium/Debug/DebugDrawer.h>

#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Asset/RuntimeAssetManager.h>

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	AssetManagerBase* CreateAssetManager()
	{
		#if WITH_EDITOR
			return new Editor::EditorAssetManager();
		#else
			return new RuntimeAssetManager();
		#endif // IS_EDITOR

		// TEMP
		return new Editor::EditorAssetManager();
	};

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

	bool Engine::Init( const ProjectConfig& a_ProjectConfig )
	{
		// 1. Initialize Pre-Engine-Init Modules
		if ( !InitModules( EEngineInitStage::PreEngineInit ) )
		{
			TE_CORE_ERROR( "Engine::Init: Failed to initialize engine modules" );
			return false;
		}

		// 2. Initialize Asset Manager
		m_AssetManager.reset( CreateAssetManager() );
		if ( !m_AssetManager )
		{
			return ASSERT_LOG( false, "Engine::Init: Failed to create Asset Manager" );
		}
		m_AssetManager->Init();

		// 3. Initialize Scene Manager
		SceneManager::Singleton::Construct();

		// 4. Initialize Post-Engine-Init Modules
		if ( !InitModules( EEngineInitStage::PostEngineInit ) )
		{
			return ASSERT_LOG( false, "Engine::Init: Failed to initialize engine modules" );
		}

		// 5. Initialize Game Instance
		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

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
		// 5. Shutdown Game Instance
		m_GameInstance->Shutdown();

		// 4. Shutdown Post-Engine-Init Modules
		ShutdownModules( EEngineInitStage::PostEngineInit );

		// 3. Shutdown Scene Manager
		SceneManager::Singleton::Destroy();

		// 2. Shutdown Asset Manager
		m_AssetManager->Shutdown();

		// 1. Shutdown Pre-Engine-Init Modules
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
