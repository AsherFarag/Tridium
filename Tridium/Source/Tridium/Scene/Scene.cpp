#include "tripch.h"
#include "Scene.h"

namespace Tridium {

	void Scene::Init()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		// Initialize Scene Systems
		{
			PROFILE_SCOPE( "Init Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->Init();
			}
		}

		// Post Initialize Scene Systems
		{
			PROFILE_SCOPE( "PostInit Scene Systems", ProfilerCategory::Scene );

			for ( const auto& [typeHash, system] : m_SceneSystems )
			{
				system->PostInit();
			}
		}
	}

	void Scene::OnBeginPlay()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

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
		if ( m_IsPaused || m_SceneState == ESceneState::None )
			return;

		PROFILE_FUNCTION( ProfilerCategory::Scene );

		a_DeltaTime *= m_TimeScale;

		// Prephysics Tick
		if ( m_SceneState == ESceneState::Play )
		{
			PROFILE_SCOPE( "PrePhysics Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PrePhysics, a_DeltaTime );
		}

		// Physics Tick
		{
			PROFILE_SCOPE( "Physics Tick", ProfilerCategory::Scene );
		}

		// Postphysics Tick
		if ( m_SceneState == ESceneState::Play )
		{
			PROFILE_SCOPE( "PostPhysics Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PostPhysics, a_DeltaTime );
		}

		// Prerender Tick
		if ( m_SceneState == ESceneState::Play )
		{
			PROFILE_SCOPE( "PreRender Tick", ProfilerCategory::Scene );

			TickSceneSystems( ESceneTickGroup::PreRender, a_DeltaTime );
		}

		// Render Tick
		{
			PROFILE_SCOPE( "Render Tick", ProfilerCategory::Scene );
		}

		// Postrender Tick
		if ( m_SceneState == ESceneState::Play )
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