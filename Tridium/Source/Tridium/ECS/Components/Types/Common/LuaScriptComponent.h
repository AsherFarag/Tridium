#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Scripting/Script.h>

namespace Tridium {

	class LuaScriptComponent : public ScriptableComponent
	{
		REFLECT( LuaScriptComponent );
		friend class ScriptEngine;
		friend class GameObject;

	public:
		LuaScriptComponent();
		virtual ~LuaScriptComponent();

	protected:
		// Reference to the script asset.
		SharedPtr<Script> m_Script;
		// Contains an instance of the script that stores it's own local variables and functions.
		sol::environment m_Environment;
	};

}