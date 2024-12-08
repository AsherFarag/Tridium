#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Scripting/Script.h>

namespace Tridium {

	// Forward declarations
	namespace Script {
		class ScriptEngine;
		class ScriptSystem;
	}
	// -------------------

	class LuaScriptComponent : public Component
	{
		REFLECT( LuaScriptComponent );
	public:
		LuaScriptComponent();
		virtual ~LuaScriptComponent();

	protected:
		LuaScriptHandle m_Script;
		// Contains an instance of the script that stores it's own local variables and functions.
		Script::ScriptInstance m_Environment;

		Script::ScriptFunction m_OnBeginPlay;
		Script::ScriptFunction m_OnUpdate;
		Script::ScriptFunction m_OnDestroy;

		// ================================
		friend Script::ScriptEngine;
		friend Script::ScriptSystem;
	};

}