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

		static inline sol::state& GetState() { return Get()->m_State; }

	private:
		sol::state m_State;

	private:
		static ScriptEngine* s_Instance;
	};

}