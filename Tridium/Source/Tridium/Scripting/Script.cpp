#include "tripch.h"
#include "Script.h"
#include "ScriptEngine.h"

namespace Tridium {

	Script::Script( const fs::path& a_FilePath )
		: m_FilePath( a_FilePath )
	{
	}

	Ref<Script> Script::Create( const fs::path& a_FilePath )
	{
		if ( a_FilePath.extension() != "lua" )
			return nullptr;

		Ref<Script> foundScript = ScriptLibrary::GetScript( a_FilePath.string() );
		if ( foundScript )
			return foundScript;

		auto script = Ref<Script>( new Script( a_FilePath ) );
		ScriptLibrary::Add( script, a_FilePath.string() );
		return script;
	}

	// LIBRARY

	ScriptLibrary* ScriptLibrary::Get()
	{
		static ScriptLibrary* s_Instance = new ScriptLibrary();
		return s_Instance;
	}

	Ref<Script> ScriptLibrary::GetScript( const std::string& a_Path )
	{
		auto it = Get()->m_Library.find( a_Path );
		return it != Get()->m_Library.end() ? it->second : nullptr;
	}

	bool ScriptLibrary::Has( const std::string& a_Path )
	{
		return Get()->m_Library.find( a_Path ) != Get()->m_Library.end();
	}

	void ScriptLibrary::Add( const Ref<Script>& a_Script, const std::string& a_Path )
	{
		Get()->m_Library.emplace( a_Path, a_Script );
	}

}