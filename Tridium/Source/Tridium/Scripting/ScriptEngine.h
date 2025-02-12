#pragma once
#include "Script.h"
#include <Tridium/Containers/Containers.h>
#include <Tridium/Reflection/MetaTypes.h>
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	//================================================================
	// Script Engine
	//  The script engine is a singleton that handles the global lua state.
	//  It is responsible for compiling scripts and registering user-defined types.
	//================================================================
	class ScriptEngine : public ISingleton<ScriptEngine, /* _ExplicitSetup */ true, /* _IsOwning */ false, /* _IsThreadSafe */ true>
	{
	public:
		sol::state& GetLuaState() { return m_LuaState; }
		bool RecompileScript( ScriptAsset& a_Script );
		void RecompileAllScripts();

		template<typename T>
		sol::usertype<T> RegisterNewType( const String& a_TypeName )
		{
			static bool s_TypeInitialized = false;
			if ( s_TypeInitialized )
			{
				ASSERT_LOG( false, "RegisterType can only be called once per type!" );
				return {};
			}

			sol::usertype<T> newType = GetLuaState().new_usertype<T>( a_TypeName );
			m_RegisteredTypes.insert( a_TypeName );
			s_TypeInitialized = true;

			return newType;
		}

		template<typename T>
		Optional<sol::usertype<T>> GetRegisteredType( const String& a_TypeName )
		{
			if ( m_RegisteredTypes.find( a_TypeName ) == m_RegisteredTypes.end() )
			{
				ASSERT_LOG( false, "Type not registered!" );
				return {};
			}

			return GetLuaState()[a_TypeName];
		}

	private:
		bool Init();
		void Shutdown();

		const StringView GetUserDataTypeName( sol::userdata a_UserData );

	private:
		sol::state m_LuaState;
		// A set of registered User-Types names that can be used to query the type in the reflection system.
		UnorderedSet<String> m_RegisteredTypes;

		friend class ScriptModule;
	};
	//================================================================

} // namespace Tridium