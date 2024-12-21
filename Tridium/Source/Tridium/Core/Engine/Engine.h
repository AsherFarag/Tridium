#pragma once
#include <Tridium/Core/Core.h>
#include "EngineModule.h"

namespace Tridium {

	struct EngineModuleList
	{
		template<typename T>
		void AddModule( UniquePtr<T> a_Module )
		{
			const size_t hash = typeid( T ).hash_code();
			Modules[hash].emplace_back( std::move( a_Module ) );
		}

		template<typename T>
		T* GetModule()
		{
			constexpr size_t hash = typeid( T ).hash_code();
			auto it = Modules.find( hash );
			if ( it == Modules.end() )
				return nullptr;

			UniquePtr<IEngineModule>& module = it->second;
			return static_cast<T*>( module.get() );
		}

		std::unordered_map<size_t, UniquePtr<IEngineModule>> Modules;
	};

	class Engine final
	{
	public:
		static Engine& Get() { return *s_Instance; }

		template<typename T>
		T* GetEngineModule() const { return m_Modules.GetModule<T>(); }


	private:
		EngineModuleList m_Modules;

	private:
		Engine() = default;
		~Engine() = default;
		static Engine* s_Instance;
		friend class Application;
	};

}