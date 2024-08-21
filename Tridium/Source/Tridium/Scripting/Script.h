#pragma once
#include "sol/sol.hpp"

#include <filesystem>
namespace fs = std::filesystem;

namespace Tridium {

	class Script
	{
		friend class ScriptEngine;
	public:
		static SharedPtr<Script> Create( const IO::FilePath& a_FilePath );
		~Script() = default;

		const IO::FilePath& GetFilePath() const { return m_FilePath; }

	private:
		Script( const IO::FilePath& a_FilePath );

	private:
		IO::FilePath m_FilePath;
	};



	class ScriptLibrary
	{
		friend class ScriptEngine;
		friend Script;
	public:
		static ScriptLibrary* Get();
		static SharedPtr<Script> GetScript( const std::string& a_Path );
		static bool Has( const std::string& a_Path );

	private:
		static void Add( const SharedPtr<Script>& a_Script, const std::string& a_Path );

	private:
		std::unordered_map<IO::FilePath, SharedPtr<Script>> m_Library;
	};

}