#pragma once
#include "EngineConfig.h"
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Project/Project.h>
#include <Tridium/Scene/SceneManager.h>
#include <Tridium/Asset/AssetManagerBase.h>

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
	class Engine final : public ISingleton<Engine>
	{
	public:
		Project& GetActiveProject() { return m_ActiveProject; }
		const EngineConfig& GetConfig() const { return m_Config; }
		const FilePath& GetEngineAssetsDirectory() const { return GetConfig().EngineAssetsDirectory; }
		AssetManagerBase* GetAssetManager() { return m_AssetManager.get(); }

		void SetConfig( const EngineConfig& a_Config ) { m_Config = a_Config; }

		//================================================================
		// Engine Modules
		using ModuleStorage = UnorderedMap< hash_t, Pair< EngineModuleInfo, UniquePtr<IEngineModule> > >;
		// Get a module by its type hash. E.g. Hashing::TypeHash<MyModule>()
		// Returns nullptr if the module does not exist.
		IEngineModule* GetModule( hash_t a_TypeHash );
		// Get a module by its type.
		template<typename T> requires Concepts::IsValidEngineModule<T>
		T* GetModule();
		//================================================================

	private:
		EngineConfig				m_Config;
		Project                     m_ActiveProject;
		ModuleStorage               m_EngineModules;
		UniquePtr<AssetManagerBase> m_AssetManager;
		UniquePtr<GameInstance>     m_GameInstance;
		ImGuiLayer*                 m_ImGuiLayer = nullptr;

	protected:
		//////////////////////////////////////////////////////////////////////////
		// Engine Functions to be called by the Application class
		//////////////////////////////////////////////////////////////////////////

		//============================
		// Engine Initialization
		bool Init( const EngineConfig& a_Config );
		bool InitProject();
		bool InitModules( EEngineInitStage a_InitStage );
		bool InitScene();
		//============================

		//============================
		// Engine Shutdown
		void Shutdown();
		// Modules are shutdown in reverse order of initialization
		bool ShutdownModules( EEngineInitStage a_ShutdownStage );
		//============================

		//////////////////////////////////////////////////////////////////////////

		friend class Application;
	};



	//////////////////////////////////////////////////////////////////////////
	// Inline Definitions
	//////////////////////////////////////////////////////////////////////////

	template<typename T> requires Concepts::IsValidEngineModule<T>
	inline T* Engine::GetModule()
	{
		constexpr hash_t hash = Hashing::TypeHash<T>();
		return static_cast<T*>( GetModule( hash ) );
	}

} // namespace Tridium