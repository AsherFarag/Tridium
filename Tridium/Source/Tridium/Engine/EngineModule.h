#pragma once
#include <Tridium/Application/Event.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Assert.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Containers/Containers.h>
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Utils/Macro.h>

namespace Tridium {

	//=============================================================================================
	// Used to declare and define an engine module.
	//  _Name: Name of the module class. (e.g. 'class _Name' )
	//  _Category: Category of the module. (e.g. EEngineModuleCategory::Server | EEngineModuleCategory::Client)
	//  __VA_ARGS__: Optional dependencies of the module. (e.g. "RendererModule", "ScriptModule" )
	#define DEFINE_ENGINE_MODULE( _Name, _Category, ... ) \
		static_assert( ValidateEngineModuleCategory( ( _Category ) ), "Invalid engine module category!" ); \
		class _Name; \
		namespace __Hidden { extern volatile ::Tridium::Detail::EngineModuleInitializer<_Name> s_##_Name##Initializer; EXPAND( ALWAYS_EXISTS( s_##_Name##Initializer ) ); } \
		class _Name : public ::Tridium::Detail::EngineModule<_Name, ::Tridium::Detail::StaticEngineModuleInfo<_Name, _Category __VA_OPT__(,) __VA_ARGS__ >>

	//=============================================================================================
	// Should be used in the implementation file of the module.
	// Registers the module with the engine module factory.
	// Use this macro if you want to register the module with the engine.
	#define REGISTER_ENGINE_MODULE( _ModuleType ) \
			volatile ::Tridium::Detail::EngineModuleInitializer<_ModuleType> __Hidden::s_##_ModuleType##Initializer{};

	//=============================================================================================
	// Engine Module Category: Defines which application types the module will be created for.
	//=============================================================================================
	enum class  EEngineModuleCategory : uint8_t
	{
		Unknown = 0,
		Server = 1 << 0,
		Client = 1 << 1,
		Editor = 1 << 2, // NOTE: A module with this flag can ONLY exist in the Editor application.
	};
	DEFINE_ENUM_BITMASK_OPERATORS( EEngineModuleCategory );

	//=============================================================================================
	// Validates the engine module category.
	inline constexpr bool ValidateEngineModuleCategory( EEngineModuleCategory a_Category )
	{
		if ( EnumFlags( a_Category ).HasFlag( EEngineModuleCategory::Editor ) )
			return Cast<uint8_t>( a_Category ) == Cast<uint8_t>( EEngineModuleCategory::Editor ); // If a Module is marked as Editor, it can only be used in the Editor application.

		return true;
	};

	//=============================================================================================
	// Engine Module Info
	//=============================================================================================
	struct EngineModuleInfo
	{
		HashedString Name{};
		EEngineModuleCategory Category = EEngineModuleCategory::Unknown;
		Span<const HashedString> Dependencies{};
	};

	//=============================================================================================
	// Engine Module Interface: Base interface for engine modules.
	// Engine modules are used to extend the engine with custom functionality.
	// Modules can be globally accessed through the Engine class.
	// Modules are initialized after all their dependencies have been initialized
	// and are shut down before their dependencies are shut down.
	// An engine module can be registered with the REGISTER_ENGINE_MODULE macro.
	//=============================================================================================
	class IEngineModule
	{
	public:

		//=========================================================================================
		virtual EngineModuleInfo GetModuleInfo() const = 0;

	protected:

		//=========================================================================================
		friend class Engine;
		virtual void Init() {}
		virtual void Shutdown() {}
		virtual void OnEvent( Event& ) {}
		virtual bool Validate() const { return true; }

	};

	//=============================================================================================
	// Engine Module Factory: 
	// Engine modules can be registered with the engine module factory to be created and managed by the engine.
	// Engine module info can be retrieved by hash or type.
	//=============================================================================================
	class EngineModuleFactory : public ISingleton<EngineModuleFactory, /* _ExplicitSetup */ false>
	{
	public:

		//=========================================================================================
		using ModuleCreateFunc = UniquePtr<IEngineModule>( * )( );

		//=========================================================================================
		struct RegisteredModule
		{
			EngineModuleInfo Info;
			ModuleCreateFunc Create;
		};

		//=========================================================================================
		// Returns all registered module types.
		static const auto& ModuleTypes() { return Get()->m_ModuleTypes; }

		//=========================================================================================
		// Registers a module type with the engine module factory.
		// This can then be used by the engine to create and manage the module.
		template<Concepts::Derived<IEngineModule> T>
		static void RegisterModule()
		{
			// Ensure the module has not already been registered.
			if ( !ASSERT( Get()->m_ModuleTypes.find( Hashing::TypeHash<T>() ) == Get()->m_ModuleTypes.end(), "Module already registered" ) )
				return;

			RegisteredModule module;
			module.Info = T::StaticModuleInfo();
			module.Create = +[]() -> UniquePtr<IEngineModule> 
			{
				UniquePtr<IEngineModule> instance = MakeUnique<T>();
				T::Singleton::BindExisting( Cast<T*>( instance.get() ) );
				return instance;
			};

			Get()->m_ModuleTypes[ Hashing::TypeHash<T>() ] = module;
		}

	private:

		//=========================================================================================
		virtual void OnSingletonConstructed() override {}

		//=========================================================================================
		UnorderedMap<hash_t, RegisteredModule> m_ModuleTypes;

	};

	namespace Detail {

		//=========================================================================================
		// Static Engine Module Info: Helper for creating engine module info.
		//=========================================================================================
		template<typename T, EEngineModuleCategory _Category, StringLiteral... _Dependencies>
		struct StaticEngineModuleInfo
		{
			static constexpr HashedString Name = GetStrippedTypeName<T>();
			static constexpr EEngineModuleCategory Category = _Category;
			static constexpr FixedArray<HashedString, sizeof...( _Dependencies )> Dependencies{ HashedString( _Dependencies )... };
			static constexpr bool HasDependencies = true;
		};

		//=========================================================================================
		// Static Engine Module Info: Helper for creating engine module info with no dependencies.
		//=========================================================================================
		template<typename T, EEngineModuleCategory _Category>
		struct StaticEngineModuleInfo<T, _Category>
		{
			static constexpr HashedString Name = GetStrippedTypeName<T>();
			static constexpr EEngineModuleCategory Category = _Category;
			static constexpr bool HasDependencies = false;
		};

		//=========================================================================================
		// Engine Module Template
		//=========================================================================================
		template<typename T, typename _ModuleInfo>
		class EngineModule : public IEngineModule, public ISingleton<T, true, false>
		{
		public:

			//=====================================================================================
			static consteval EngineModuleInfo StaticModuleInfo()
			{
				EngineModuleInfo info;
				info.Name = _ModuleInfo::Name;
				info.Category = _ModuleInfo::Category;
				if constexpr ( _ModuleInfo::HasDependencies )
					info.Dependencies = _ModuleInfo::Dependencies;

				return info;
			}

			//=====================================================================================
			EngineModuleInfo GetModuleInfo() const override final
			{
				return StaticModuleInfo();
			}

			//=====================================================================================
			static bool Valid()
			{
				return ISingleton<T, true, false>::Get() && ISingleton<T, true, false>::Get()->Validate();
			}

		};

		//=========================================================================================
		// Engine Module Initializer: 
		// Used to automatically register a module type with the engine module factory.
		//=========================================================================================
		template<typename T>
		struct EngineModuleInitializer
		{
			EngineModuleInitializer()
			{
				EngineModuleFactory::RegisterModule<T>();
			}
		};

	} // namespace Detail

} // namespace Tridium