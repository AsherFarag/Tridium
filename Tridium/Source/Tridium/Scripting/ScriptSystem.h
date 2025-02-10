#pragma once
#include <Tridium/Scene/SceneSystem.h>
#include <Tridium/ECS/Components/Types/Common/LuaScriptComponent.h>

namespace Tridium {

	class ScriptSystem final : public ISceneSystem
	{
	public:
		virtual void Init() override {}
		virtual void Shutdown() override {}
		virtual void OnSceneEvent( const SceneEventPayload& a_Event ) override;

	private:
		void OnBeginPlay();
		void OnUpdate();
		void OnLuaScriptComponentDestroyed( LuaScriptComponent& a_Component );
		void InitLuaScriptComponent( LuaScriptComponent& a_Component );
	};

}