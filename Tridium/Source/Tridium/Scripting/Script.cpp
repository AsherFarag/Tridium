#include "tripch.h"
#include "Script.h"
#include "ScriptEngine.h"

namespace Tridium {

	Script::Script( sol::state& a_State, const std::string& a_FilePath )
		: m_FilePath( a_FilePath )
	{
	}

	Ref<Script> Script::Create( const std::string& a_FilePath, const std::string& a_Name )
	{
		TE_ASSERT( !ScriptLibrary::Has( a_Name ), "A script already exists with name {0}", a_Name );

		auto script = Ref<Script>( new Script( ScriptEngine::GetLuaState(), a_FilePath ) );
		ScriptLibrary::Add( script, a_Name );
		return script;
	}

	// LIBRARY

	ScriptLibrary* ScriptLibrary::Get()
	{
		static ScriptLibrary* s_Instance = new ScriptLibrary();
		return s_Instance;
	}

	Ref<Script> ScriptLibrary::GetScript( const std::string& a_Name )
	{
		return Get()->m_Library.find(a_Name)->second;
	}

	bool ScriptLibrary::Has( const std::string& a_Name )
	{
		return Get()->m_Library.find(a_Name) != Get()->m_Library.end();
	}

	void ScriptLibrary::Add( const Ref<Script>& a_Script, const std::string& a_Name )
	{
		Get()->m_Library.emplace( a_Name, a_Script );
	}

}