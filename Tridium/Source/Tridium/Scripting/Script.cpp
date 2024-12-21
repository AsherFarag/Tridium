#include "tripch.h"
#include "Script.h"
#include "ScriptEngine.h"

namespace Tridium::Script {

	const ScriptVariable& ScriptAsset::GetVariable( const std::string& a_Name ) const
	{
		if ( auto it = m_Variables.find( a_Name ); it != m_Variables.end() )
			return it->second;

		return ScriptVariable();
	}

	const ScriptFunction& ScriptAsset::GetFunction( const std::string& a_Name ) const
	{
		if ( auto it = m_Functions.find( a_Name ); it != m_Functions.end() )
			return it->second;

		return ScriptFunction();
	}

	void ScriptAsset::Invalidate()
	{
		m_LoadResult = sol::load_result();
		m_Environment = sol::environment();
		m_Variables.clear();
		m_Functions.clear();
		m_CompileErrorMsg.clear();
	}

}