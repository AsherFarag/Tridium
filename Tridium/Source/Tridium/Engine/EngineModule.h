#pragma once
#include "EngineInitialization.h"
#include <Tridium/Containers/Containers.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Assert.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	//================================================================
	// Engine Module Category
	struct EEngineModuleCategory 
	{
		enum Type
		{
			Generic = 0,
			Core = 1 << 0,
			Physics = 1 << 0,
			Audio = 1 << 1,
			Graphics = 1 << 2,
			Scripting = 1 << 3,
			Networking = 1 << 4,
			Platform = 1 << 5,
			Tools = 1 << 6,
			Editor = 1 << 7,
			Gameplay = 1 << 8,
		};
	};
	//================================================================



	//================================================================
	// Engine Module Error
	struct EngineModuleError
	{
		enum ELevel
		{
			Error,
			FatalError
		};

		ELevel Level;
		String Message;

		EngineModuleError( ELevel a_Level, const String& a_Message = "" )
			: Level( a_Level ), Message( a_Message ) {
		}
	};
	//================================================================



	//================================================================
	// Engine Module
	//  Base interface for engine modules.
	//  Engine modules are used to extend the engine with custom functionality.
	//  Modules can be globally accessed through the Engine class.
	//  Modules are initialized and shutdown in a specific order defined by the InitStage.
	//  An engine module can be registered with the REGISTER_ENGINE_MODULE macro.
	class IEngineModule
	{
	public:
		virtual Optional<EngineModuleError> Init() { return {}; }
		virtual Optional<EngineModuleError> Shutdown() { return {}; }
	};
	//================================================================



	//================================================================
	// Concepts
	namespace Concepts {
		template<typename T>
		concept IsValidEngineModule = Concepts::IsBaseOf<IEngineModule, T>;
	}
	//================================================================



	//================================================================
	// Engine Module Info
	//  Contains meta data about an engine module.
	struct EngineModuleInfo
	{
		using CreateModuleFunc = UniquePtr<IEngineModule>( * )( );

		CreateModuleFunc Create;
		EEngineInitStage InitStage;
		const char* Name;
		EEngineModuleCategory::Type Category;
		hash_t Hash;
	};
	//================================================================



	//================================================================
	// Engine Module Factory
	//  Engine modules can be registered with the engine module factory to be created and managed by the engine.
	//  Engine module info can be retrieved by hash or type.
	class EngineModuleFactory : public ISingleton<EngineModuleFactory, /* _ExplicitSetup */ false>
	{
	public:
		// Registers a module type with the engine module factory.
		// This can then be used by the engine to create and manage the module.
		template<typename T> requires Concepts::IsValidEngineModule<T>
		static void RegisterModule( const char* a_Name, EEngineInitStage a_InitStage, EEngineModuleCategory::Type a_Category )
		{
			static constexpr hash_t hash = Hashing::TypeHash<T>();

			// Ensure the module has not already been registered.
			if ( !ASSERT_LOG( Get()->m_ModuleTypes.find( hash ) == Get()->m_ModuleTypes.end(), "Module already registered" ) )
				return;

			EngineModuleInfo info;
			info.InitStage = a_InitStage;
			info.Name = a_Name;
			info.Category = a_Category;
			info.Hash = hash;

			info.Create = +[]() -> UniquePtr<IEngineModule> { return MakeUnique<T>(); };

			Get()->m_ModuleTypes[hash] = info;
		}

		// Returns a list of all registered module types.
		static const auto& ModuleTypes() { return Get()->m_ModuleTypes; }

		// Retrieves the module info for the specified hash.
		static EngineModuleInfo GetModuleInfo( hash_t a_Hash )
		{
			auto it = Get()->m_ModuleTypes.find( a_Hash );
			if ( it != Get()->m_ModuleTypes.end() )
				return it->second;

			return {};
		}

		// Retrieves the module info for the specified type.
		template<typename T> requires Concepts::IsValidEngineModule<T>
		static EngineModuleInfo GetModuleInfo()
		{
			static constexpr hash_t hash = Hashing::TypeHash<T>();
			return GetModuleInfo( hash );
		}

	private:
		virtual void OnSingletonConstructed() override { RegisterCoreModules(); }

		// These will be the first modules to be registered.
		void RegisterCoreModules();

		// Using an ordered map to ensure modules are initialized in the same order they were registered.
		Map<hash_t, EngineModuleInfo> m_ModuleTypes;
	};
	//===================================



	template<typename _ModuleType> requires Concepts::IsValidEngineModule<_ModuleType>
	struct EngineModuleInitializer
	{
		EngineModuleInitializer( const char* a_Name, EEngineInitStage a_InitStage )
		{
			EngineModuleFactory::RegisterModule<_ModuleType>( a_Name, a_InitStage );
		}
	};

	// Should be used in the implementation file of the module.
	// Registers the module with the engine module factory.
	// Use this macro if you want to register the module with the engine.
#define REGISTER_ENGINE_MODULE( _ModuleType, _InitStage, _Category ) \
	static EngineModuleInitializer<ModuleType> s_##ModuleType##Initializer = EngineModuleInitializer<ModuleType>( #_ModuleType, _InitStage, _Category )

} // namespace Tridium