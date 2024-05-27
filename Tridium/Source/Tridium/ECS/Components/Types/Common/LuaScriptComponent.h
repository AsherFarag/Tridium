#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Scripting/Script.h>

namespace Tridium {

	DEFINE_COMPONENT( LuaScriptComponent, ScriptableComponent )
	{
	public:
		LuaScriptComponent(const Ref<Script>& a_Script);
		~LuaScriptComponent();

		virtual void OnUpdate() override;

	protected:
		virtual void OnConstruct() override;
		virtual void OnDestroy() override;

	private:
		Ref<Script> m_Script;

		sol::protected_function* Lua_OnConstruct;
		sol::protected_function* Lua_OnDestroy;
		sol::protected_function* Lua_OnUpdate;
	};

}