#include "tripch.h"
#include "Scene.h"
#include <Tridium/Physics/PhysicsScene.h>
#include <Tridium/Graphics/Renderer/RendererSceneSystem.h>

namespace Tridium {

	REGISTER_ASSET_TYPE( Scene,
		AssetTypeInfo{}
			.SetName( "Scene" )
			.SetIcon( EditorIcons::MountainSun )
	);

	void Scene::Init()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		m_State = {};

		m_PhysicsSystem = AddSystem<PhysicsSceneSystem>();
		m_RendererSystem = AddSystem<RendererSceneSystem>();

		// Initialize Scene Systems
		{
			PROFILE_SCOPE( "Init Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->Init();
			}

			m_State.HasInit = true;
		}

		// Post Initialize Scene Systems
		{
			PROFILE_SCOPE( "PostInit Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->PostInit();
			}

			m_State.HasPostInit = true;
		}
	}

	void Scene::OnBeginPlay( EScenePlayMode a_PlayMode )
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		m_State.PlayMode = a_PlayMode;

		// OnBeginPlay Scene Systems
		{
			PROFILE_SCOPE( "OnBeginPlay Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->OnBeginPlay();
			}
		}
	}

	void Scene::OnTick( float a_DeltaTime )
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		a_DeltaTime *= m_TimeScale;

		const bool isPlaying = ( m_State.PlayMode == EScenePlayMode::Play ) && !m_State.IsPaused;
		const bool isSimulating = ( m_State.PlayMode == EScenePlayMode::Simulate );

		// Prephysics Tick
		if ( isPlaying )
		{
			PROFILE_SCOPE( "PrePhysics Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PrePhysics, a_DeltaTime );
		}

		// Physics Tick
		if ( m_PhysicsSystem && ( isPlaying || isSimulating ) )
		{
			PROFILE_SCOPE( "Physics Tick", ProfilerCategory::Scene );

		}

		// Postphysics Tick
		if ( isPlaying )
		{
			PROFILE_SCOPE( "PostPhysics Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PostPhysics, a_DeltaTime );
		}

		// Prerender Tick
		if ( isPlaying )
		{
			PROFILE_SCOPE( "PreRender Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PreRender, a_DeltaTime );
		}

		// Render Tick
		{
			PROFILE_SCOPE( "Render Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::Render, a_DeltaTime );
		}

		// Postrender Tick
		if ( isPlaying )
		{
			PROFILE_SCOPE( "PostRender Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PostRender, a_DeltaTime );
		}
	}

	void Scene::TickSceneSystems( ESceneTickGroup a_TickGroup, float a_DeltaTime )
	{
		for ( const auto& [typeHash, system] : m_SceneSystems )
		{
			if ( system->GetTickGroup() == a_TickGroup )
			{
				system->OnUpdate( a_DeltaTime );
			}
		}
	}

	void Scene::OnEndPlay()
	{
		// OnEndPlay Scene Systems
		{
			PROFILE_SCOPE( "OnEndPlay Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->OnEndPlay();
			}
		}
	}

	void Scene::Shutdown()
	{
		// Shutdown Scene Systems
		{
			PROFILE_SCOPE( "Shutdown Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->Shutdown();
			}
		}
	}

} // namespace Tridium