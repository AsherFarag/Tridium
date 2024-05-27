#include "tripch.h"
#include "Script.h"
#include "ScriptEngine.h"

namespace Tridium {

	Script::Script( sol::state& a_State, const std::string& a_FilePath )
		: m_FilePath( a_FilePath ), m_Environment( a_State, sol::create, a_State.globals() )
	{
		a_State.script_file( a_FilePath, m_Environment );
		Recompile();
	}

	Ref<Script> Script::Create( const std::string& a_FilePath, const std::string& a_Name )
	{
		TE_ASSERT( !ScriptLibrary::Has( a_Name ), "A script already exists with name {0}", a_Name );

		auto script = Ref<Script>( new Script( ScriptEngine::GetLuaState(), a_FilePath ) );
		ScriptLibrary::Add( script, a_Name );
		return script;
	}

	void Script::Recompile()
	{
		ScriptEngine::GetLuaState().script_file(m_FilePath, m_Environment);
		BindFunctions();
	}

	void Script::BindFunctions()
	{
		Lua_OnConstruct = m_Environment[ "OnConstruct" ];
		Lua_OnDestroy = m_Environment[ "OnDestroy" ];
		Lua_OnUpdate = m_Environment[ "OnUpdate" ];
	}

	// LIBRARY

	ScriptLibrary* ScriptLibrary::s_Instance = new ScriptLibrary();

	Ref<Script> ScriptLibrary::Get( const std::string& a_Name )
	{
		return s_Instance->m_Library.find( a_Name )->second;
	}

	bool ScriptLibrary::Has( const std::string& a_Name )
	{
		return s_Instance->m_Library.find(a_Name) != s_Instance->m_Library.end();
	}

	void ScriptLibrary::Add( const Ref<Script>& a_Script, const std::string& a_Name )
	{
		s_Instance->m_Library.emplace( a_Name, a_Script );
	}

}