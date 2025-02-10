#include "tripch.h"
#include "Engine.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Debug/DebugDrawer.h>

#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Asset/RuntimeAssetManager.h>

#include <Editor/Editor.h>
#include <Tridium/ImGui/ImGuiLayer.h>

// Temp ?
#include <Tridium/IO/ProjectSerializer.h>
#include <Tridium/IO/FileManager.h>

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	AssetManagerBase* CreateAssetManager()
	{
		#if WITH_EDITOR
			return new EditorAssetManager();
		#else
			return new RuntimeAssetManager();
		#endif // IS_EDITOR

		// TEMP
		return new EditorAssetManager();
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

	bool Engine::Init( const EngineConfig& a_Config )
	{
		m_Config = a_Config;

		// Create Modules
		{
			for ( auto&& [key, module] : EngineModuleFactory::ModuleTypes() )
			{
				m_EngineModules[key] = module.Create();
			}
		}

		// 0. Initialize Project
		if ( !InitProject() )
		{
			TE_CORE_ERROR( "Engine::Init: Failed to initialize project" );
		}

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

		m_ImGuiLayer = new ImGuiLayer();
		Application::Get()->PushOverlay( m_ImGuiLayer );

		#if WITH_EDITOR
		{
			EditorConfig editorConfig;
			Editor::Init( std::move( editorConfig ) );
		}
		#endif // WITH_EDITOR

		// 5. Initialize Game Instance
		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

		// 6. Init Scene
		if ( !InitScene() )
		{
			return ASSERT_LOG( false, "Engine::Init: Failed to initialize scene" );
		}

		return true;
	}

	bool Engine::InitProject()
	{
		m_ActiveProject = {};

		// Retrieve the project path from the command line arguments
		FilePath projectPath;
		const CmdLineArgs& cmdLineArgs = Application::Get()->GetCommandLineArgs();
		TODO( "Handle project path from command line arguments" );
		if ( cmdLineArgs.Args.Size() > 1 )
		{
			projectPath = FilePath( cmdLineArgs.Args[1] );
		}

		// If a project path was provided, load the project
		if ( projectPath.Exists() )
		{
			ProjectSerializer::DeserializeText( m_ActiveProject.Config, projectPath);
			m_ActiveProject.Config.Editor.ProjectDirectory = FilePath( projectPath ).GetParentPath();
		}
		// Otherwise, attempt to find a project file in the current directory
		else
		{
			TE_CORE_TRACE( "No project path provided - searching for project file in '{0}'", IO::FileManager::GetWorkingDirectory().ToString() );
			FilePath projectPath = IO::FileManager::GetWorkingDirectory() / "Project.tproject"; //FileManager::FindFileWithExtension( ".tproject" );
			if ( projectPath.Exists() )
			{
				TE_CORE_TRACE( "Project file found at '{0}'", projectPath.ToString() );
				ProjectSerializer::DeserializeText( m_ActiveProject.Config, projectPath );
				m_ActiveProject.Config.Editor.ProjectDirectory = projectPath.GetParentPath();
			}
			else
			{
				TE_CORE_WARN( "No project file found!" );
			}
		}

		TODO( "Handle project initialization" );
		#if CONFIG_SHIPPING
		m_Config.EngineAssetsDirectory = "EngineAssets";
		#else
		m_Config.EngineAssetsDirectory = "../Tridium/EngineAssets";
		#endif // !CONFIG_SHIPPING

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
						continue;
					default:
						ASSERT( false, "Unknown error level" );
						break;
					}
				}
				
				TE_CORE_INFO( "[Engine] Initialized module: {0}", module.Name );
			}
		}
		return true;
	}

	bool Engine::InitScene()
	{
		{
			TE_CORE_INFO( "Loading start scene" );
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( m_ActiveProject.Config.StartScene ) )
			{
				SceneManager::SetActiveScene( scene.get() );
			}
			else
			{
				TE_CORE_WARN( "Failed to load start scene! - Creating new scene" );
				SceneManager::SetActiveScene( MakeShared<Scene>().get() );
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

		#if WITH_EDITOR
		Editor::Shutdown();
		#endif // WITH_EDITOR

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
