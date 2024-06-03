#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Scripting/Script.h>

namespace Tridium {

	DEFINE_COMPONENT( LuaScriptComponent, ScriptableComponent )
	{
		friend class ScriptEngine;
		friend class GameObject;

	public:
		LuaScriptComponent() = default;
		LuaScriptComponent(const Ref<Script>& a_Script);
		LuaScriptComponent( const std::string& a_Script );
		~LuaScriptComponent();

		virtual void OnUpdate() override;

		Ref<Script>& GetScript() { return m_Script; }
		void SetScript( const Ref<Script>&a_Script );

		template <typename T>
		auto operator[]( T&& key )& { return m_Environment[ key ]; }
		template <typename T>
		auto operator[]( T&& key ) const& { return m_Environment[ key ]; }
		template <typename T>
		auto operator[]( T&& key )&& { return m_Environment[ key ]; }

	private:
		virtual void OnConstruct() override;
		virtual void OnDestroy() override;

		void Compile();

		template <typename... Args>
		bool CallLuaFunction( sol::protected_function & func, Args&&... args )
		{
			if ( !m_Script )
				return false;

			if ( func.valid() )
			{
				func( std::forward<Args>( args )... );
				return true;
			}
			else
			{
				TE_CORE_ERROR( "LUA - Function call failed!" );
				return false;
			}
		}

	private:
		Ref<Script> m_Script;
		// Every instance of LuaScriptComponent has its own environment so,
		// multiple instances can reference a script but still store their local members.
		sol::environment m_Environment;

		sol::protected_function Lua_OnConstruct;
		sol::protected_function Lua_OnDestroy;
		sol::protected_function Lua_OnUpdate;
	};

}