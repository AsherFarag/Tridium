#pragma once
#include <Tridium/Application/Layer.h>
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Engine/EngineConfig.h>
#include <Tridium/Engine/EngineModule.h>
#include <Tridium/Project/Project.h>

// Gameplay Frameworks
#include <Tridium/GameFramework/GameInstance.h>

namespace Tridium {

	//================================================================
	// This function MUST be defined in CLIENT
	// Use this function so the engine can create your custom GameInstance class.
	TODO( "We no longer use Sandbox" );
	GameInstance* CreateGameInstance();
	//================================================================

	//=============================================================================================
	// Engine: This class serves as the layer above the Application class and is the core of the engine.
	// It is responsible for initialization and shutdown of the engine and its modules.
	//=============================================================================================
	class Engine final : public IAppLayer
	{
	public:

		//=========================================================================================
		using ModuleStorage = UnorderedMap<hash64_t, UniquePtr<IEngineModule>>;

		//=========================================================================================
		Engine( EngineConfig a_Config );
		~Engine() override;

		//=========================================================================================
		static Engine* Get() { return s_Instance; }

		//=========================================================================================
		Project& ActiveProject() { return m_Project; }
		const auto& Config() const { return m_Config; }
		const auto& EngineAssetsDirectory() const { return Config().EngineAssetsDirectory; }

		//=========================================================================================
		// Get a module by its type hash. E.g. Hashing::TypeHash<MyModule>()
		// Returns nullptr if the module does not exist.
		static IEngineModule* GetModule( hash_t a_TypeHash );

		//=========================================================================================
		// Get an engine module by its type.
		template<Concepts::Derived<IEngineModule> T>
		static T* GetModule()
		{
			constexpr hash_t hash = Hashing::TypeHash<T>();
			return Cast<T*>( GetModule( hash ) );
		}

	private:

		//=============================================================================================
		void OnAttach() override {}
		void OnDetach() override {}
		void OnEvent( Event& a_Event ) override;

		//=========================================================================================
		bool InitProject();
		Array<IEngineModule*> GetInitOrderedModules();

	private:

		//=========================================================================================
		friend class Application;
		friend class Editor;
		static Engine* s_Instance;

		//=========================================================================================
		Project m_Project;
		EngineConfig m_Config;
		ModuleStorage m_EngineModules;
		UniquePtr<GameInstance> m_GameInstance;

	};

} // namespace Tridium