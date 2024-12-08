#pragma once
#include "Script.h"
#include <unordered_set>
#include <Tridium/Reflection/MetaTypes.h>

namespace Tridium::Script {

	// The script engine is a singleton that handles the lua state
	class ScriptEngine
	{
	public:
		ScriptEngine() = default;
		~ScriptEngine() = default;
		static ScriptEngine& Get() { return *s_Instance; }
		static sol::state& GetLuaState() { return s_Instance->m_LuaState; }

		static bool RecompileScript( ScriptAsset& a_Script );
		static void RecompileAllScripts();

		template<typename T>
		static sol::usertype<T> RegisterNewType( const char* a_TypeName )
		{
			static bool s_Initialized = false;
			if ( s_Initialized )
			{
				TE_CORE_ASSERT( false, "RegisterType can only be called once per type!" );
				return {};
			}

			sol::usertype<T> newType = GetLuaState().new_usertype<T>( a_TypeName );
			s_Instance->m_RegisteredTypes.insert( a_TypeName );
			s_Initialized = true;

			return newType;
		}

	private:
		void Init();

		static const std::string& GetUserDataTypeName( sol::userdata a_UserData );

	private:
		static UniquePtr<ScriptEngine> s_Instance;
		sol::state m_LuaState;
		// A set of registered User-Types names that can be used to query the type in the reflection system.
		std::unordered_set<std::string> m_RegisteredTypes;

		friend class Application;
	};

}