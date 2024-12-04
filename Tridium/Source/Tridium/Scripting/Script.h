#pragma once
#include <Tridium/Asset/Asset.h>
#include "ScriptProperty.h"
#include "sol/sol.hpp"

namespace Tridium {

	using ScriptFunction = sol::function;
	using ScriptFunctionMap = std::unordered_map<std::string, ScriptFunction>;

	// A script is a single Lua file that has been compiled and loaded into memory.
	class Script : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Lua );
		Script( const std::string& a_Source ) : m_Source( a_Source ) {}
		Script( std::string&& a_Source ) : m_Source( std::move( a_Source ) ) {}
		virtual ~Script() = default;

		bool IsCompiled() const { return m_LoadResult.valid(); }
		const std::string& GetCompileErrorMsg() const { return m_CompileErrorMsg; }

		const ScriptVariable& GetVariable( const std::string& a_Name ) const
		{
			if ( auto it = m_Variables.find( a_Name ); it != m_Variables.end() )
				return it->second;

			return ScriptVariable();
		}

		const ScriptFunction& GetFunction( const std::string& a_Name ) const
		{
			if ( auto it = m_Functions.find( a_Name ); it != m_Functions.end() )
				return it->second;

			return ScriptFunction();
		}

		const ScriptVariableMap& GetVariables() const { return m_Variables; }
		const ScriptFunctionMap& GetFunctions() const { return m_Functions; }

	protected:
		// Contains the source code of the script.
		std::string m_Source;
		// Contains the compiled script.
		sol::load_result m_LoadResult;
		// Contains a default environment for the script.
		sol::environment m_Environment;
		// Contains the error message if the script could not be compiled.
		std::string m_CompileErrorMsg;

		ScriptVariableMap m_Variables;
		ScriptFunctionMap m_Functions;

		friend class ScriptEngine;
	};

}