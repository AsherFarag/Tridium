#include "tripch.h"
#include "Script.h"
#include "ScriptEngine.h"

namespace Tridium {

	Script::Script( const IO::FilePath& a_FilePath )
		: m_FilePath( a_FilePath )
	{
	}

	SharedPtr<Script> Script::Create( const IO::FilePath& a_FilePath )
	{
		if ( a_FilePath.GetExtension() != ".lua" )
			return nullptr;

		SharedPtr<Script> foundScript = ScriptLibrary::GetScript( a_FilePath.ToString() );
		if ( foundScript )
			return foundScript;

		auto script = SharedPtr<Script>( new Script( a_FilePath ) );
		ScriptLibrary::Add( script, a_FilePath.ToString() );
		return script;
	}

	// LIBRARY

	ScriptLibrary* ScriptLibrary::Get()
	{
		static ScriptLibrary* s_Instance = new ScriptLibrary();
		return s_Instance;
	}

	SharedPtr<Script> ScriptLibrary::GetScript( const std::string& a_Path )
	{
		auto it = Get()->m_Library.find( a_Path );
		return it != Get()->m_Library.end() ? it->second : nullptr;
	}

	bool ScriptLibrary::Has( const std::string& a_Path )
	{
		return Get()->m_Library.find( a_Path ) != Get()->m_Library.end();
	}

	void ScriptLibrary::Add( const SharedPtr<Script>& a_Script, const std::string& a_Path )
	{
		Get()->m_Library.emplace( a_Path, a_Script );
	}

}