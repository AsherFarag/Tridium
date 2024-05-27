#pragma once
#include "sol/sol.hpp"

namespace Tridium {

	class ScriptEngine
	{
		friend class Script;

	public:
		static ScriptEngine* Get();

	private:
		ScriptEngine();
		~ScriptEngine();

		void Init();

		static inline sol::state& GetLuaState() { return Get()->m_LuaState; }

	private:
		sol::state m_LuaState;
	};

}