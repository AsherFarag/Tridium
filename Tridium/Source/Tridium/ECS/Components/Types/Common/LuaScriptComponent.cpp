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
		(*LuaFunc_OnUpdate)( (float)Time::DeltaTime() );
	}

	void LuaScriptComponent::OnConstruct()
	{
		LuaFunc_OnConstruct = &m_Script->LuaFunc_OnConstruct;
		LuaFunc_OnDestroy = &m_Script->LuaFunc_OnDestroy;
		LuaFunc_OnUpdate = &m_Script->LuaFunc_OnUpdate;

		(*LuaFunc_OnConstruct)( );
	}

	void LuaScriptComponent::OnDestroy()
	{
		(*LuaFunc_OnDestroy)( );
	}
}