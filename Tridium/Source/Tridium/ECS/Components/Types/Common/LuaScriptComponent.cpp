#include "tripch.h"
#include "LuaScriptComponent.h"

#include <Tridium/Scripting/ScriptEngine.h>

namespace Tridium {

	LuaScriptComponent::LuaScriptComponent( const Ref<Script>& a_Script )
		: m_Environment( ScriptEngine::GetLuaState(), sol::create)
	{
		m_Script = a_Script;
	}

	LuaScriptComponent::~LuaScriptComponent()
	{

	}

	void LuaScriptComponent::Compile()
	{
		Compile( ScriptEngine::GetLuaState() );
	}

	void LuaScriptComponent::Compile( sol::state& lua )
	{
		lua.script_file( m_Script->GetFilePath(), m_Environment);

		m_Environment[ "gameObject" ] = GetGameObject();

		Lua_OnConstruct = m_Environment[ "OnConstruct" ];
		Lua_OnUpdate = m_Environment[ "OnUpdate" ];
		Lua_OnDestroy = m_Environment[ "OnDestroy" ];
	}

	void LuaScriptComponent::OnUpdate()
	{
		CallLuaFunction( Lua_OnUpdate, (float)Time::DeltaTime() );
	}

	void LuaScriptComponent::OnConstruct()
	{
		Compile();

		CallLuaFunction( Lua_OnConstruct );
	}

	void LuaScriptComponent::OnDestroy()
	{
		CallLuaFunction( Lua_OnDestroy );
	}
}