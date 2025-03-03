#include "tripch.h"

#include "Scene.h"
#include <Tridium/Core/Assert.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Physics/RayCast.h>

// Systems
#include <Tridium/Scripting/ScriptSystem.h>

namespace Tridium {


	template<typename... _Types>
	using ViewType = decltype( entt::registry::view<_Types...> );

	RayCastResult Scene::CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_RayCastChannel, const PhysicsBodyFilter& a_BodyFilter, bool a_DrawDebug, Debug::EDrawDuration a_DrawDurationType, float a_DebugDrawDuration, Color a_DebugLineColor, Color a_DebugHitColor ) const
	{
		RayCastResult result = m_PhysicsScene->CastRay( a_Start, a_End, a_RayCastChannel, a_BodyFilter );

#if TE_DRAW_DEBUG
		if ( a_DrawDebug )
		{
			Debug::DrawLine( result.RayStart, result.RayStart + result.RayEnd, a_DebugLineColor, a_DrawDurationType, a_DebugDrawDuration );

			if ( result.Hit )
			{
				TODO( "Draw a sphere at the hit position" );
				AABB aabb = { result.Position - Vector3( 0.1f ), result.Position + Vector3( 0.1f ) };
				Debug::DrawAABBFilled( aabb, a_DebugHitColor, a_DrawDurationType, a_DebugDrawDuration );
			}
		}
#endif

		return result;
	}

	Scene::Scene(const String& name)
		: m_Name( name ), m_SceneRenderer(*this)
	{
		Initialize();
	}

	Scene::Scene( const Scene& a_Other )
		: m_Name( a_Other.m_Name ), m_SceneEnvironment(a_Other.m_SceneEnvironment), m_SceneRenderer( *this )
	{
		m_Handle = a_Other.m_Handle;
		m_State = a_Other.m_State;
		m_SceneRenderer.m_LightEnvironment = a_Other.m_SceneRenderer.m_LightEnvironment;
		m_SceneRenderer.m_RenderSettings = a_Other.m_SceneRenderer.m_RenderSettings;
		m_SceneRenderer.m_SceneEnvironment = a_Other.m_SceneRenderer.m_SceneEnvironment;

		Initialize();

		// Entity registry
		{
			const entt::registry& src = a_Other.GetECS().GetRegistry();

			// Step 1: Create entities
			for ( auto entity : src.view<GUIDComponent>() ) 
			{
				entt::entity newEntity = m_ECS.CreateEntity( entity );
			}

			// Step 2: Copy components
			for ( auto [id, srcStorage] : src.storage() ) 
			{
				Refl::MetaType componentType = entt::resolve( srcStorage.type() );
				Refl::AddToGameObjectFunc addToGameObjectFunc = nullptr;
				auto addToGameObjectFuncProp = componentType.GetMetaAttribute<Refl::Props::AddToGameObjectProp::Type>( Refl::Props::AddToGameObjectProp::ID );
				if ( ASSERT( addToGameObjectFuncProp.has_value() ) )
					addToGameObjectFunc = addToGameObjectFuncProp.value();

				//// Handle TransformComponent separately
				//if ( componentType == Refl::ResolveMetaType<TransformComponent>() )
				//{
				//	auto view = src.view<TransformComponent>();
				//	for ( auto entity : view )
				//	{
				//		const TransformComponent& tc = view.get<TransformComponent>( entity );
				//		TransformComponent& newTc = m_ECS.AddComponentToEntity<TransformComponent>( entity, tc );
				//		newTc.m_EntityID = GameObject( entity );
				//	}
				//	continue;
				//}

				for ( auto it = srcStorage.rbegin(); it != srcStorage.rend(); ++it ) 
				{
					entt::entity entity = *it;

					// Handle component copy
					Component* component = addToGameObjectFunc( *this, entity );
					if ( !ASSERT(component) )
						continue;

					Refl::MetaAny dstComponent = componentType.FromVoid( component );
					Refl::MetaAny srcComponent = componentType.FromVoid( srcStorage.value( entity ) );
					dstComponent.assign( srcComponent );

					if ( Component* dstAsComponent = dstComponent.try_cast<Component>() )
						dstAsComponent->m_EntityID = GameObject( entity );
				}
			}

		}
	}

	Scene::~Scene()
	{
	}

	bool Scene::Initialize()
	{
		m_PhysicsScene = PhysicsScene::Create();
		m_PhysicsScene->m_Scene = this;

		InitSystems();

		InitAllComponentTypes();

		return true;
	}

	void Scene::OnBeginPlay()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		m_State.IsRunning = true;
		m_State.HasBegunPlay = true;

		GetMainCamera();

		// Send OnBeginPlay Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnBeginPlay
			};

			SendSceneEvent( payload );
		}

		// Initialize Physics Scene
		{
			PROFILE_SCOPE( "Physics Scene Init", ProfilerCategory::Physics );
			m_PhysicsScene->Init();

			// Add all GameObjects with RigidBodyComponent to the physics scene
			auto view = m_ECS.View<RigidBodyComponent, TransformComponent>();
			for ( auto entity : view )
			{
				auto& rb = view.get<RigidBodyComponent>( entity );
				auto& tc = view.get<TransformComponent>( entity );

				if ( m_PhysicsScene->AddPhysicsBody( GameObject( entity ), rb, tc ) )
					rb.GetBodyProxy().SetPhysicsScene( m_PhysicsScene );
			}
		}

		// Call OnBeginPlay on all entity tickers
		for ( const auto& ticker : m_EntityTickers )
		{
			PROFILE_SCOPE( "Entity Ticker OnBeginPlay", ProfilerCategory::GameLogic );
			ticker->OnBeginPlay();
		}
	}

	void Scene::OnUpdate()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );
		// Send OnUpdate Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnUpdate
			};

			SendSceneEvent( payload );
		}

		// Update physics
		{
			PROFILE_SCOPE( "Physics Update", ProfilerCategory::Physics );
			TODO( "We should not be constantly updating transforms unless they are dirty." );

			auto view = m_ECS.View<RigidBodyComponent, TransformComponent>();

			// Update the transforms in the physics scene
			{
				view.each( [&]( auto entity, RigidBodyComponent& rb, TransformComponent& tc )
					{
						m_PhysicsScene->UpdatePhysicsBodyTransform( rb, tc );
					} );
			}

			m_PhysicsScene->Tick( Time::DeltaTime() );

			// Update the transforms from the physics scene
			view.each( [&]( auto entity, RigidBodyComponent& rb, TransformComponent& tc )
				{
					tc.Position = rb.m_BodyProxy.GetPosition();
					tc.Rotation.SetFromQuaternion( rb.m_BodyProxy.GetRotation() );
				} );
		}

		// Call OnUpdate for all scriptable objects
		for ( const auto& ticker : m_EntityTickers )
		{
			PROFILE_SCOPE( "Entity Ticker OnUpdate", ProfilerCategory::GameLogic );
			ticker->OnUpdate( Time::DeltaTime() );
		}
	}

	void Scene::OnEndPlay()
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );

		m_State.IsRunning = false;
		m_State.HasBegunPlay = false;

		// Send OnEndPlay Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnEndPlay
			};

			SendSceneEvent( payload );
		}

		// Shutdown Physics Scene
		{
			m_PhysicsScene->Shutdown();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Physics
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// GameObjects
	//////////////////////////////////////////////////////////////////////////

	void Scene::DestroyGameObject( GameObject a_GameObject )
	{
		if ( !IsGameObjectValid( a_GameObject ) )
			return;
		// Send OnGameObjectDestroyed Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnGameObjectDestroyed,
				.EventData = OnGameObjectDestroyedEvent
				{
					.GameObjectID = a_GameObject
				}
			};
			SendSceneEvent( payload );
		}

		m_ECS.DestroyEntity( entt::entity( a_GameObject.m_ID ) );
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		return InstantiateGameObject( GUID::Create(), a_Name );
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_ECS.CreateEntity() );
		AddComponentToGameObject<GUIDComponent>( go, a_GUID );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<GameObjectFlagsComponent>( go, EGameObjectFlags::Enabled );
		AddComponentToGameObject<TransformComponent>( go );

		// Send OnGameObjectCreated Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnGameObjectCreated,
				.EventData = OnGameObjectCreatedEvent
				{
					.GameObjectID = go
				}
			};

			SendSceneEvent( payload );
		}

		return go;
	}

	GameObject Scene::InstantiateGameObjectFrom( GameObject a_Source )
	{
		static const Refl::MetaType GUIDComponentType = Refl::ResolveMetaType<GUIDComponent>();
		static const Refl::MetaType TagComponentType = Refl::ResolveMetaType<TagComponent>();
		static const Refl::MetaType GameObjectFlagsComponentType = Refl::ResolveMetaType<GameObjectFlagsComponent>();
		static const Refl::MetaType TransformComponentType = Refl::ResolveMetaType<TransformComponent>();

		GameObject dst( m_ECS.CreateEntity() );
		AddComponentToGameObject<GUIDComponent>( dst );
		AddComponentToGameObject<TagComponent>( dst, a_Source.GetTag() );
		AddComponentToGameObject<GameObjectFlagsComponent>( dst, a_Source.GetFlags() );
		TransformComponent& tc = AddComponentToGameObject<TransformComponent>( dst );
		tc.Position = a_Source.GetTransform().Position;
		tc.Rotation = a_Source.GetTransform().Rotation;
		tc.Scale = a_Source.GetTransform().Scale;

		for ( auto [id, storage] : m_ECS.Storage() )
		{
			if ( !storage.contains( a_Source ) )
				continue;

			if ( storage.type() == GUIDComponentType.Info() )
				continue;

			if ( storage.type() == TagComponentType.Info() )
				continue;

			if ( storage.type() == TransformComponentType.Info() )
				continue;

			if ( storage.type() == GameObjectFlagsComponentType.Info() )
				continue;

			if ( storage.contains(dst) )
				storage.erase( dst );

			storage.push( dst, storage.value( a_Source ) );
		}

		// Send OnGameObjectCreated Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnGameObjectCopied,
				.EventData = OnGameObjectCopiedEvent
				{
					.SourceID = a_Source,
					.DestinationID = dst
				}
			};

			SendSceneEvent( payload );
		}

		return dst;
	}

	void Scene::CopyGameObject( GameObject a_Destination, GameObject a_Source )
	{
		if ( !IsGameObjectValid( a_Destination ) || !IsGameObjectValid( a_Source ) )
			return;

		TransformComponent& tc = GetComponentFromGameObject<TransformComponent>( a_Destination );
		tc.Position = a_Source.GetTransform().Position;
		tc.Rotation = a_Source.GetTransform().Rotation;
		tc.Scale = a_Source.GetTransform().Scale;

		for ( auto [id, storage] : m_ECS.Storage() )
		{
			if ( !storage.contains( a_Source ) )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<GUIDComponent>().Info() )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<TagComponent>().Info() )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<TransformComponent>().Info() )
				continue;

			if ( storage.contains( a_Destination ) )
				storage.erase( a_Destination );

			storage.push( a_Destination, storage.value( a_Source ) );
		}

		// Send OnGameObjectCreated Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnGameObjectCopied,
				.EventData = OnGameObjectCopiedEvent
				{
					.SourceID = a_Source,
					.DestinationID = a_Destination
				}
			};

			SendSceneEvent( payload );
		}
	}

	bool Scene::IsGameObjectValid( GameObject a_GameObject ) const
	{
		bool exists = m_ECS.IsValidEntity( a_GameObject.m_ID );
		if ( !exists )
			return false;

		return !a_GameObject.GetFlags().HasFlag( EGameObjectFlags::PendingKill );
	}

	GameObject Scene::FindGameObjectByTag( const std::string& a_Tag ) const
	{
		auto view = GetECS().View<TagComponent>();
		for ( auto&& [ entity, tagComponent ] : view.each() )
		{
			if ( tagComponent.Tag == a_Tag )
				return GameObject( entity );
		}

		return GameObject{};
	}

	std::vector<GameObject> Scene::FindAllGameObjectsByTag( const std::string& a_Tag ) const
	{
		std::vector<GameObject> gameObjects;
		// Reserve an arbitrary amount of space for the vector
		gameObjects.reserve( 8 );
		auto view = m_ECS.View<TagComponent>();
		for ( const auto&& [entity, Tag] : view.each() )
		{
			if ( Tag.Tag == a_Tag )
				gameObjects.push_back( GameObject( entity ) );
		}

		return gameObjects;
	}

	void Scene::InitAllComponentTypes()
	{
		for ( auto [id, type] : Refl::ResolveMetaTypes() )
		{
			Refl::MetaType componentType = type;
			if ( !componentType.IsComponent() )
				continue;

			Optional<Refl::InitComponentFunc> initComponentFuncProp = componentType.GetMetaAttribute<Refl::InitComponentFunc>( Refl::Props::InitComponentProp::ID );
			if ( ASSERT_LOG( initComponentFuncProp.has_value(), "Component meta type does not have an InitComponent function!" ) )
				initComponentFuncProp.value()( *this );
		}
	}

	void Scene::Clear()
	{
		m_ECS.Clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// Systems
	//////////////////////////////////////////////////////////////////////////

	void Scene::InitSystems()
	{
		AddSystem<ScriptSystem>();

		for ( auto&& [type, system] : m_Systems )
		{
			system->Init();
		}
	}

	void Scene::ShutdownSystems()
	{
		for ( auto&& [type, system] : m_Systems )
		{
			system->Shutdown();
		}
	}

	void Scene::SendSceneEvent( const SceneEventPayload& a_EventPayload )
	{
		PROFILE_FUNCTION( ProfilerCategory::Scene );
		for ( auto&& [type, system] : m_Systems )
		{
			system->OnSceneEvent( a_EventPayload );
		}
	}

	//////////////////////////////////////////////////////////////////////////

	CameraComponent* Scene::GetMainCamera()
	{
		if ( IsGameObjectValid( m_MainCamera ) )
		{
			if ( auto* camera = TryGetComponentFromGameObject<CameraComponent>( m_MainCamera ) )
				return camera;
		}

		m_MainCamera = NullEntity;
		auto cameras = m_ECS.View<CameraComponent>();
		CameraComponent* mainCamera = nullptr;
		for ( GameObject camera : cameras )
		{
			if ( cameras.get<CameraComponent>( camera ).IsMainCamera )
			{
				m_MainCamera = camera;
				mainCamera = &cameras.get<CameraComponent>( camera );
			}
		}

		if ( !IsGameObjectValid( m_MainCamera ) && !cameras.empty() )
		{
			m_MainCamera = cameras.front();
			mainCamera = &cameras.get<CameraComponent>( m_MainCamera );
		}

		return mainCamera;
	}

	GameObject Scene::GetMainCameraGameObject() const
	{
		return GameObject( m_MainCamera );
	}

	void Scene::SetMainCamera( GameObject a_CameraGameObject )
	{
		m_MainCamera = a_CameraGameObject;
	}

}