#pragma once
#include "sol/sol.hpp"

namespace Tridium {

	class Script
	{
	public:
		static Ref<Script> Create( const std::string& a_FilePath, const std::string& a_Name );
		~Script() = default;

		void Recompile();
		void BindFunctions();
		auto& Environment() const { return m_Environment; }
	public:
		sol::protected_function Lua_OnConstruct;
		sol::protected_function Lua_OnDestroy;
		sol::protected_function Lua_OnUpdate;

	private:
		Script( sol::state& a_State, const std::string& a_FilePath );

	private:
		std::string m_FilePath;
		sol::environment m_Environment;
	};



	class ScriptLibrary
	{
		friend Script;
	public:
		static Ref<Script> Get( const std::string& a_Name );
		static bool Has( const std::string& a_Name );

	private:
		static void Add( const Ref<Script>& a_Script, const std::string& a_Name );

	private:
		std::unordered_map<std::string, Ref<Script>> m_Library;

	private:
		static ScriptLibrary* s_Instance;
	};

}