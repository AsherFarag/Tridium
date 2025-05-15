#include "tripch.h"
#include "ScriptSystem.h"
#include "ScriptEngine.h"
#include <Tridium/Scene/Scene.h>
#include <Tridium/Asset/AssetManager.h>

#include <Tridium/ECS/Components/Types.h>

//TEMP
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Asset/Loaders/LuaScriptLoader.h>

namespace Tridium {


	static const char* s_OnBeginPlayFunctionKey = "OnBeginPlay";
	static const char* s_OnUpdateFunctionKey = "OnUpdate";
	static const char* s_OnDestroyFunctionKey = "OnDestroy";

	void ScriptSystem::OnSceneEvent( const SceneEventPayload& a_Event )
	{
		switch ( a_Event.EventType )
		{
			case ESceneEventType::OnBeginPlay:
			{
				OnBeginPlay();
				break;
			}
			case ESceneEventType::OnUpdate:
			{
				OnUpdate();
				break;
			}
			case ESceneEventType::OnComponentCreated:
			{
				const auto& event = std::get<OnComponentCreatedEvent>( a_Event.EventData );
				if ( event.ComponentTypeID == Refl::ResolveMetaType<LuaScriptComponent>().ID() )
				{
					InitLuaScriptComponent( *Cast<LuaScriptComponent*>( event.Component ) );
				}
				break;
			}
			case ESceneEventType::OnComponentDestroyed:
			{
				const auto& event = std::get<OnComponentDestroyedEvent>( a_Event.EventData );
				if ( event.ComponentTypeID == Refl::ResolveMetaType<LuaScriptComponent>().ID() )
				{
					OnLuaScriptComponentDestroyed( *Cast<LuaScriptComponent*>( event.Component ) );
				}
				break;
			}
			default:
			{
				break;
			}
		};
	}

	void ScriptSystem::OnBeginPlay()
	{
		auto view = GetOwningScene()->GetECS().View<LuaScriptComponent>();
		view.each( [this]( auto go, LuaScriptComponent& a_Component )
			{
				InitLuaScriptComponent( a_Component );
			} );

		view.each( []( auto go, LuaScriptComponent& a_Component )
			{
				if ( a_Component.m_OnBeginPlay.valid() )
				{
					a_Component.m_OnBeginPlay();
				}
			} );
	}

	void ScriptSystem::OnUpdate()
	{
		auto view = GetOwningScene()->GetECS().View<LuaScriptComponent>();
		view.each( []( LuaScriptComponent& a_Component )
			{
				if ( a_Component.m_OnUpdate.valid() )
				{
					a_Component.m_OnUpdate();
				}
			} );
	}

	void ScriptSystem::OnLuaScriptComponentDestroyed( LuaScriptComponent& a_Component )
	{
		if ( a_Component.m_OnDestroy.valid() )
		{
			a_Component.m_OnDestroy();
		}
	}

	void ScriptSystem::InitLuaScriptComponent( LuaScriptComponent& a_Component )
	{
		SharedPtr<ScriptAsset> script = AssetManager::GetAsset<ScriptAsset>( a_Component.m_Script );
		if ( !script )
		{
			return;
		}

		if ( !script->IsCompiled() )
		{
			LOG( LogCategory::Script, Error, "Attempted to use uncompiled script '{0}'", script->GetHandle().ID() );
			return;
		}

		// Create a new environment for the script.
		a_Component.m_Environment = ScriptInstance( ScriptEngine::Get()->GetLuaState(), sol::create, ScriptEngine::Get()->GetLuaState().globals() );
		sol::protected_function scriptFunc = script->m_LoadResult;
		a_Component.m_Environment.set_on( scriptFunc );

		// Add local variables to the environment.
		a_Component.m_Environment["gameObject"] = Cast<GameObject>( a_Component.GetGameObject() );
		TransformComponent& transform = a_Component.GetGameObject().GetTransform();
		a_Component.m_Environment["transform"] = &transform;

		// Execute the script to populate the environment.
		scriptFunc();

		// Extract the the scriptable component functions
		if ( auto onBeginPlay = a_Component.m_Environment[s_OnBeginPlayFunctionKey]; onBeginPlay.valid() )
			a_Component.m_OnBeginPlay = onBeginPlay;

		if ( auto onUpdate = a_Component.m_Environment[s_OnUpdateFunctionKey]; onUpdate.valid() )
			a_Component.m_OnUpdate = onUpdate;

		if ( auto onDestroy = a_Component.m_Environment[s_OnDestroyFunctionKey]; onDestroy.valid() )
			a_Component.m_OnDestroy = onDestroy;
	}

}