#include "tripch.h"
#include "Engine.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Debug/DebugDrawer.h>

#include <Tridium/oldAsset/AssetManager.h>
#include <Tridium/oldAsset/EditorAssetManager.h>
#include <Tridium/oldAsset/RuntimeAssetManager.h>

#include <Tridium/Editor/Editor.h>

#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Physics/PhysicsModule.h>
#include <Tridium/Scripting/ScriptModule.h>

// Temp ?
#include <Tridium/IO/ProjectSerializer.h>
#include <Tridium/IO/FileManager.h>

namespace Tridium {

	Engine* Engine::s_Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	AssetManagerBase* CreateAssetManager()
	{
		if ( false )
		{
			( void )RendererModule::Print();
		}

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
		auto it = Get()->m_EngineModules.find( a_TypeHash );
		if ( it != Get()->m_EngineModules.end() )
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
			LOG( LogCategory::Engine, Trace, "No project path provided - searching for project file in '{0}'", IO::FileManager::GetWorkingDirectory().ToString() );
			FilePath projectPath = IO::FileManager::GetWorkingDirectory() / "Project.tproject"; //FileManager::FindFileWithExtension( ".tproject" );
			if ( projectPath.Exists() )
			{
				LOG( LogCategory::Engine, Trace, "Project file found at '{0}'", projectPath.ToString() );
				ProjectSerializer::DeserializeText( m_ActiveProject.Config, projectPath );
				m_ActiveProject.Config.Editor.ProjectDirectory = projectPath.GetParentPath();
			}
			else
			{
				LOG( LogCategory::Engine, Warn, "No project file found!" );
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

	bool Engine::InitScene()
	{
		if ( false )
		{
			LOG( LogCategory::Engine, Info, "Loading start scene" );
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( m_ActiveProject.Config.StartScene ) )
			{
				SceneManager::SetActiveScene( scene.get() );
			}
			else
			{
				LOG( LogCategory::Engine, Warn, "Failed to load start scene! - Creating new scene" );
				SceneManager::SetActiveScene( MakeShared<Scene>().get() );
			}
		}

		return true;
	}

	Array<IEngineModule*> Engine::GetInitOrderedModules()
	{
		Array<IEngineModule*> result;
		result.Reserve( m_EngineModules.size() );

		struct PrereqNode
		{
			IEngineModule* Module;
			Array< PrereqNode* > Dependents;
		};

		Map<HashedString, PrereqNode> prereqMap;
		Queue<PrereqNode*> queue;

		for ( const auto& [hash, module] : m_EngineModules )
		{
			const EngineModuleInfo moduleInfo = module->GetModuleInfo();
			PrereqNode& node = prereqMap[ moduleInfo.Name ];
			node.Module = module.get();

			if ( moduleInfo.Dependencies.empty() )
			{
				queue.push( &node );
			}
			else
			{
				for ( const HashedString& dependency : moduleInfo.Dependencies )
				{
					prereqMap[ dependency ].Dependents.PushBack( &node );
				}
			}
		}

		UnorderedSet<hash_t> visited;
		while ( !queue.empty() )
		{
			PrereqNode* node = queue.front();
			queue.pop();

			const hash_t nodeHash = node->Module->GetModuleInfo().Name.Hash();
			if ( visited.contains( nodeHash ) )
				continue;

			visited.emplace( nodeHash );
			result.PushBack( node->Module );

			for ( PrereqNode* dependent : node->Dependents )
			{
				queue.push( dependent );
			}
		}

		return result;
	}

	UniquePtr<Engine> Engine::Create( const EngineConfig& a_Config )
	{
		ENSURE( !s_Instance, "An Engine instance already exists!" );
		return UniquePtr<Engine>( new Engine( a_Config ) );
	}

	Engine::Engine( const EngineConfig& a_Config )
	{
		s_Instance = this;
		m_Config = a_Config;

		// Create Modules
		for ( const auto& [key, moduleInfo] : EngineModuleFactory::ModuleTypes() )
		{
			m_EngineModules[ key ] = moduleInfo.Create();
		}

		// Initialize Project
		if ( !InitProject() )
		{
			LOG( LogCategory::Engine, Error, "Engine::Init: Failed to initialize project" );
		}

		// Initialize Modules
		for ( IEngineModule* module : GetInitOrderedModules() )
		{
			LOG( LogCategory::Engine, Info, "Initializing module: {0}", module->GetModuleInfo().Name.String() );
			module->Init();
		}

		// Initialize Asset Database
		if ( auto error = AssetDatabase::Init(); error.IsError() )
		{
			LOG( LogCategory::Engine, Error, "Engine::Init: Failed to initialize Asset Database: {0}", error.Error() );
			TODO( "Handle fatal error" );
		}

		// Initialize Scene Manager
		SceneManager::Singleton::Construct();

		// Initialize Game Instance
		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

		// Init Scene
		if ( !InitScene() )
		{
			ENSURE( false, "Engine::Init: Failed to initialize scene" );
		}
	}

	Engine::~Engine()
	{
		// Shutdown Game Instance
		m_GameInstance->Shutdown();

		// Shutdown Scene Manager
		SceneManager::Singleton::Destroy();

		// Shutdown Modules
		{
			Array<IEngineModule*> initOrderedModules = GetInitOrderedModules();
			for ( auto it = initOrderedModules.RBegin(); it != initOrderedModules.REnd(); ++it )
			{
				LOG( LogCategory::Engine, Info, "Shutting down module: {0}", ( *it )->GetModuleInfo().Name.String());
				( *it )->Shutdown();
			}
		}

		s_Instance = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
