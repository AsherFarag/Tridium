#include "tripch.h"
#include "LuaScriptComponent.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Scripting/ScriptEngine.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( LuaScriptComponent )
		BASE( Component )
		PROPERTY( m_Script, Serialize | EditAnywhere )
	END_REFLECT( LuaScriptComponent );

	LuaScriptComponent::LuaScriptComponent()
		: m_Script( LuaScriptHandle::InvalidID )
		, m_Environment()
	{
	}

	LuaScriptComponent::~LuaScriptComponent()
	{
		m_Environment.abandon();
	}
}