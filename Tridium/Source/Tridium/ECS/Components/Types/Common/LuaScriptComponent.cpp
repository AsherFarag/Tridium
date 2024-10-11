#include "tripch.h"
#include "LuaScriptComponent.h"

#include <Tridium/Scripting/ScriptEngine.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( LuaScriptComponent )
		BASE( ScriptableComponent )
		//PROPERTY( m_Script, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( LuaScriptComponent )

	//LuaScriptComponent::LuaScriptComponent()
	//	: m_Environment( ScriptEngine::GetLuaState(), sol::create, ScriptEngine::GetLuaState().globals() )
	//{

	//}

	//LuaScriptComponent::LuaScriptComponent( const SharedPtr<Script>& a_Script )
	//	: m_Environment( ScriptEngine::GetLuaState(), sol::create, ScriptEngine::GetLuaState().globals() )
	//{
	//	m_Script = a_Script;
	//}

	//LuaScriptComponent::LuaScriptComponent( const std::string& a_Script )
	//	: m_Environment( ScriptEngine::GetLuaState(), sol::create, ScriptEngine::GetLuaState().globals() )
	//{
	//	m_Script = Script::Create( a_Script );
	//}

	//LuaScriptComponent::~LuaScriptComponent()
	//{
	//	m_Environment.reset();
	//}

	//void LuaScriptComponent::Compile()
	//{
	//	if ( !m_Script )
	//		return;

	//	sol::state& lua = ScriptEngine::GetLuaState();
	//	if ( !lua.safe_script_file( m_Script->GetFilePath().ToString(), m_Environment, &sol::script_pass_on_error).valid() )
	//	{
	//		TE_CORE_ERROR( "Failed to compile script [{0}]!", m_Script->GetFilePath().ToString() );
	//		return;
	//	}

	//	m_Environment[ "gameObject" ] = GetGameObject();

	//	Lua_OnConstruct = m_Environment[ "OnConstruct" ];
	//	Lua_OnUpdate = m_Environment[ "OnUpdate" ];
	//	Lua_OnDestroy = m_Environment[ "OnDestroy" ];
	//}


	//void LuaScriptComponent::SetScript( const SharedPtr<Script>& a_Script )
	//{
	//	m_Script = a_Script;
	//	Compile();
	//}

	//void LuaScriptComponent::OnUpdate()
	//{
	//	CallLuaFunction( Lua_OnUpdate, (float)Time::DeltaTime() );
	//}

	//void LuaScriptComponent::OnConstruct()
	//{
	//	Compile();

	//	CallLuaFunction( Lua_OnConstruct );
	//}

	//void LuaScriptComponent::OnDestroy()
	//{
	//	CallLuaFunction( Lua_OnDestroy );
	//}
}