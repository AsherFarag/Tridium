#include "tripch.h"
#include "LuaScriptComponent.h"

namespace Tridium {

	LuaScriptComponent::LuaScriptComponent( const Ref<Script>& a_Script )
	{
		m_Script = a_Script;
	}

	LuaScriptComponent::~LuaScriptComponent()
	{
		OnDestroy();
	}

	void LuaScriptComponent::OnUpdate()
	{
		if ( Lua_OnUpdate->valid() )
			(*Lua_OnUpdate)( (float)Time::DeltaTime() );
		else
			TE_CORE_ERROR( "Lua: On Update is invalid!" );
	}

	void LuaScriptComponent::OnConstruct()
	{
		Lua_OnConstruct = &m_Script->Lua_OnConstruct;
		Lua_OnDestroy = &m_Script->Lua_OnDestroy;
		Lua_OnUpdate = &m_Script->Lua_OnUpdate;

		(*Lua_OnConstruct)( );
	}

	void LuaScriptComponent::OnDestroy()
	{
		(*Lua_OnDestroy)( );
	}
}