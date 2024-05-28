#pragma once
#include "sol/sol.hpp"

namespace Tridium {


	// The script engine is a singleton that handles the lua state
	class ScriptEngine
	{
		friend class Script;

	public:
		static ScriptEngine* Get();
		static void Recompile();

		static inline sol::state& GetLuaState() { return Get()->m_LuaState; }

	private:
		ScriptEngine();
		~ScriptEngine();

		void Init();

	private:
		sol::state m_LuaState;
	};

}