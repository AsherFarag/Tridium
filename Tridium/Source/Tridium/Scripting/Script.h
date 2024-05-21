#pragma once
#include "sol/sol.hpp"

namespace Tridium {

	typedef sol::protected_function LuaFunc;

	class Script
	{
	public:
		static Ref<Script> Create( const std::string& a_FilePath, const std::string& a_Name );
		~Script() = default;

		void Recompile();
		void BindFunctions();
		auto& Environment() const { return m_Environment; }
	public:
		LuaFunc LuaFunc_OnConstruct;
		LuaFunc LuaFunc_OnDestroy;
		LuaFunc LuaFunc_OnUpdate;

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

	private:
		static void Add( const Ref<Script>& a_Script, const std::string& a_Name );

	private:
		std::unordered_map<std::string, Ref<Script>> m_Library;

	private:
		static ScriptLibrary* s_Instance;
	};

}