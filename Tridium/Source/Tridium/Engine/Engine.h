#pragma once
#include "EngineConfig.h"
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Project/Project.h>
#include <Tridium/Scene/SceneManager.h>
#include <Tridium/oldAsset/AssetManagerBase.h>
#include <Tridium/Asset/AssetDatabase.h>

// Engine Modules
#include "EngineModule.h"

// Gameplay Frameworks
#include <Tridium/GameFramework/GameInstance.h>

namespace Tridium {

	//================================================================
	// This function MUST be defined in CLIENT
	// Use this function so the engine can create your custom GameInstance class.
	GameInstance* CreateGameInstance();
	//================================================================

	// Forward Declarations
	class AssetManagerBase;
	class ImGuiLayer;

	//================================================================
	// Engine
	//  This class serves as the layer above the Application class and is the core of the engine.
	//  It is responsible for initialization and shutdown of the engine and its modules.
	class Engine final
	{
	public:
		using ModuleStorage = UnorderedMap<hash64_t, UniquePtr<IEngineModule>>;

		static Engine* Get() { return s_Instance; }
		static Project& GetActiveProject() { return Get()->m_ActiveProject; }
		static const EngineConfig& GetConfig() { return Get()->m_Config; }
		static void SetConfig( const EngineConfig& a_Config ) { Get()->m_Config = a_Config; }
		static const FilePath& GetEngineAssetsDirectory() { return GetConfig().EngineAssetsDirectory; }

		//=========================================================================
		// Get a module by its type hash. E.g. Hashing::TypeHash<MyModule>()
		// Returns nullptr if the module does not exist.
		static IEngineModule* GetModule( hash_t a_TypeHash );

		//=========================================================================
		// Get an engine module by its type.
		template<Concepts::Derived<IEngineModule> T>
		static T* GetModule();

	private:
		EngineConfig				m_Config;
		Project                     m_ActiveProject;
		ModuleStorage               m_EngineModules;
		UniquePtr<GameInstance>     m_GameInstance;

	protected:
		//================================================================
		friend class Application;
		friend class Editor;
		static Engine* s_Instance;

		static UniquePtr<Engine> Create( const EngineConfig& a_Config );

		//============================
		// Engine Initialization
		bool InitProject();
		bool InitScene();
		//============================

		Array<IEngineModule*> GetInitOrderedModules();

	public:
		Engine( const EngineConfig& a_Config );
		~Engine();
	};



	//////////////////////////////////////////////////////////////////////////
	// Inline Definitions
	//////////////////////////////////////////////////////////////////////////

	template<Concepts::Derived<IEngineModule> T>
	inline T* Engine::GetModule()
	{
		constexpr hash_t hash = Hashing::TypeHash<T>();
		return Cast<T*>( GetModule( hash ) );
	}

} // namespace Tridium