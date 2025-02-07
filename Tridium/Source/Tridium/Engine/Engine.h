#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/GameInstance.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Scene/SceneManager.h>
#include "EngineModule.h"

namespace Tridium {

	//================================================================
	// Engine
	//  
	class Engine final : public ISingleton<Engine>
	{
	public:
		using ModuleStorage = UnorderedMap<hash_t, UniquePtr<IEngineModule>>;

		// Get a module by its type hash. E.g. Hashing::TypeHash<MyModule>()
		// Returns nullptr if the module does not exist.
		IEngineModule* GetModule( hash_t a_TypeHash );

		template<typename T> requires Concepts::IsValidEngineModule<T>
		T* GetModule();

	private:
		UniquePtr<GameInstance> m_GameInstance;
		ModuleStorage m_EngineModules;

	protected:
		virtual void OnSingletonConstructed() override { Init(); }
		virtual void OnSingletonDestroyed() override { Shutdown(); }

		//////////////////////////////////////////////////////////////////////////
		// Engine Functions to be called by the Application class
		//////////////////////////////////////////////////////////////////////////

		//============================
		// Engine Initialization
		bool Init();
		bool InitModules( EEngineInitStage a_InitStage );
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

	// To be defined in CLIENT
	GameInstance* CreateGameInstance();



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