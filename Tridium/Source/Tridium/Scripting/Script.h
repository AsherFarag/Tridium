#pragma once
#include "sol/sol.hpp"

namespace Tridium {

	class Script
	{
		friend class ScriptEngine;
	public:
		static Ref<Script> Create( const std::string& a_FilePath, const std::string& a_Name );
		~Script() = default;

		const std::string& GetFilePath() const { return m_FilePath; }

	private:
		Script( sol::state& a_State, const std::string& a_FilePath );

	private:
		std::string m_FilePath;
	};



	class ScriptLibrary
	{
		friend class ScriptEngine;
		friend Script;
	public:
		static ScriptLibrary* Get();
		static Ref<Script> GetScript( const std::string& a_Name );
		static bool Has( const std::string& a_Name );

	private:
		static void Add( const Ref<Script>& a_Script, const std::string& a_Name );

	private:
		std::unordered_map<std::string, Ref<Script>> m_Library;
	};

}