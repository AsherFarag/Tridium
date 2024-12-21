#pragma once
#include <Tridium/Asset/Asset.h>
#include "ScriptProperty.h"
#include "sol/sol.hpp"

namespace Tridium::Script {

	// Forward declaration
	class ScriptEngine;
	// -------------------

	using ScriptFunction = sol::function;
	using ScriptFunctionMap = std::unordered_map<std::string, ScriptFunction>;
	using ScriptInstance = sol::environment;

	// A script is a single Lua file that has been compiled and loaded into memory.
	class ScriptAsset : public Asset
	{
	public:
		ASSET_CLASS_TYPE( LuaScript );
		ScriptAsset( const std::string& a_Source ) : m_Source( a_Source ) {}
		ScriptAsset( std::string&& a_Source ) : m_Source( std::move( a_Source ) ) {}
		virtual ~ScriptAsset() = default;

		bool IsCompiled() const { return m_LoadResult.valid(); }
		const std::string& GetCompileErrorMsg() const { return m_CompileErrorMsg; }
		const std::string& GetSource() const { return m_Source; }
		void SetSource( const std::string& a_Source ) { m_Source = a_Source; Invalidate(); }

		const ScriptVariableMap& GetVariables() const { return m_Variables; }
		const ScriptFunctionMap& GetFunctions() const { return m_Functions; }
		const ScriptVariable& GetVariable( const std::string& a_Name ) const;
		const ScriptFunction& GetFunction( const std::string& a_Name ) const;

		void Invalidate();

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
		friend class ScriptSystem;
		friend class LuaScriptLoader;
	};

}