#include "tripch.h"
#include "LuaScriptComponent.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Scripting/ScriptEngine.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( LuaScriptComponent )
		BASE( ScriptableComponent )
	END_REFLECT( LuaScriptComponent );

	LuaScriptComponent::LuaScriptComponent()
	{
	}

	LuaScriptComponent::~LuaScriptComponent()
	{
	}
}