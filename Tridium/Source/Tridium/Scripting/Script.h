#pragma once
#include "sol/sol.hpp"

#include <filesystem>
namespace fs = std::filesystem;

namespace Tridium {

	class Script
	{
		friend class ScriptEngine;
	public:
		static Ref<Script> Create( const fs::path& a_FilePath );
		~Script() = default;

		const fs::path& GetFilePath() const { return m_FilePath; }

	private:
		Script( const fs::path& a_FilePath );

	private:
		fs::path m_FilePath;
	};



	class ScriptLibrary
	{
		friend class ScriptEngine;
		friend Script;
	public:
		static ScriptLibrary* Get();
		static Ref<Script> GetScript( const std::string& a_Path );
		static bool Has( const std::string& a_Path );

	private:
		static void Add( const Ref<Script>& a_Script, const std::string& a_Path );

	private:
		std::unordered_map<fs::path, Ref<Script>> m_Library;
	};

}