#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>

// Systems
#include <Tridium/Scripting/ScriptSystem.h>

namespace Tridium {

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

	Scene::Scene(const std::string& name)
		: m_Name( name ), m_SceneRenderer(*this)
	{
		m_PhysicsScene = PhysicsScene::Create();
		m_PhysicsScene->m_Scene = this;

		// Add default systems
		InitSystems();
	}

	Scene::Scene( const Scene& a_Other )
		: m_Name( a_Other.m_Name ), m_SceneEnvironment(a_Other.m_SceneEnvironment), m_SceneRenderer( *this )
	{
		m_Handle = a_Other.m_Handle;
		m_Paused = a_Other.m_Paused;
		m_SceneRenderer.m_LightEnvironment = a_Other.m_SceneRenderer.m_LightEnvironment;
		m_SceneRenderer.m_RenderSettings = a_Other.m_SceneRenderer.m_RenderSettings;
		m_SceneRenderer.m_SceneEnvironment = a_Other.m_SceneRenderer.m_SceneEnvironment;

		m_PhysicsScene = PhysicsScene::Create();
		m_PhysicsScene->m_Scene = this;

		// Entity registry
		{
			const entt::registry& src = a_Other.m_Registry;

			// Step 1: Create entities
			for ( auto entity : src.view<GUIDComponent>() ) {
				entt::entity newEntity = m_Registry.create( entity );
			}

			// Step 2: Copy components
			for ( auto [id, srcStorage] : src.storage() ) 
			{
				for ( auto it = srcStorage.rbegin(); it != srcStorage.rend(); ++it ) 
				{
					entt::entity entity = *it;

					// Handle component copy
					Refl::MetaType componentType = entt::resolve( srcStorage.type() );
					Component* component = componentType.TryAddToGameObject( *this, static_cast<GameObjectID>( entity ) );

					Refl::MetaAny dstComponent = componentType.FromVoid( component );
					Refl::MetaAny srcComponent = componentType.FromVoid( srcStorage.value( entity ) );
					dstComponent.assign( srcComponent );
					reinterpret_cast<Component*>( dstComponent.data() )->m_GameObject = GameObject( entity );
				}
			}
		}

		InitSystems();
	}

	Scene::~Scene()
	{
	}

	void Scene::OnBegin()
	{
		m_IsRunning = true;
		m_HasBegunPlay = true;

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
			m_PhysicsScene->Init();

			// Add all GameObjects with RigidBodyComponent to the physics scene
			auto view = m_Registry.view<RigidBodyComponent, TransformComponent>();
			for ( auto entity : view )
			{
				auto& rb = view.get<RigidBodyComponent>( entity );
				auto& tc = view.get<TransformComponent>( entity );

				if ( m_PhysicsScene->AddPhysicsBody( GameObject( entity ), rb, tc ) )
					rb.GetBodyProxy().SetPhysicsScene( m_PhysicsScene );
			}
		}

		// Initialize scriptable objects
		for ( const auto& storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = storage.second.value( storage.second[0] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto go : storage.second )
				reinterpret_cast<ScriptableComponent*>( storage.second.value( go ) )->OnBeginPlay();
		}
	}

	void Scene::OnUpdate()
	{
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
			TODO( "We should not be constantly updating transforms unless they are dirty." );

			auto view = m_Registry.view<RigidBodyComponent, TransformComponent>();

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

		// Update scriptable objects
		for ( const auto& storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = storage.second.value( storage.second[ 0 ] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto go : storage.second )
				reinterpret_cast<ScriptableComponent*>( storage.second.value( go ) )->OnUpdate();
		}
	}

	void Scene::OnEnd()
	{
		m_IsRunning = false;
		m_HasBegunPlay = false;

		// Send OnEnd Event to all systems
		{
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnEnd
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

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		return InstantiateGameObject( GUID::Create(), a_Name );
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go, a_GUID );
		AddComponentToGameObject<TagComponent>( go, a_Name );
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
		GameObject dst( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( dst );
		AddComponentToGameObject<TagComponent>( dst, a_Source.GetTag() );
		TransformComponent& tc = AddComponentToGameObject<TransformComponent>( dst );
		tc.Position = a_Source.GetTransform().Position;
		tc.Rotation = a_Source.GetTransform().Rotation;
		tc.Scale = a_Source.GetTransform().Scale;

		for ( auto [id, storage] : m_Registry.storage() )
		{
			if ( !storage.contains( a_Source ) )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<GUIDComponent>().Info() )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<TagComponent>().Info() )
				continue;

			if ( storage.type() == Refl::ResolveMetaType<TransformComponent>().Info() )
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

		for ( auto [id, storage] : m_Registry.storage() )
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
		return m_Registry.valid( a_GameObject.m_ID );
	}

	GameObject Scene::FindGameObjectByTag( const std::string& a_Tag ) const
	{
		auto view = m_Registry.view<TagComponent>();
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
		auto view = m_Registry.view<TagComponent>();
		for ( const auto&& [entity, Tag] : view.each() )
		{
			if ( Tag.Tag == a_Tag )
				gameObjects.push_back( GameObject( entity ) );
		}

		return gameObjects;
	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// Systems
	//////////////////////////////////////////////////////////////////////////

	void Scene::InitSystems()
	{
		AddSystem<Script::ScriptSystem>();

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

		m_MainCamera = INVALID_ENTITY_ID;
		auto cameras = m_Registry.view<CameraComponent>();
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

	template<>
	void Scene::InitComponent( RigidBodyComponent& a_Component )
	{
		if ( !m_IsRunning )
			return;

		GameObject go = a_Component.GetGameObject();

		if ( auto* tc = TryGetComponentFromGameObject<TransformComponent>( go ) )
		{
			m_PhysicsScene->UpdatePhysicsBody( go, a_Component, *tc );
		}
	}

	template<>
	void Scene::InitComponent( SphereColliderComponent& a_Component )
	{
		if ( auto* rb = TryGetComponentFromGameObject<RigidBodyComponent>( a_Component.GetGameObject() ) )
			InitComponent( *rb );
	}

	template<>
	void Scene::InitComponent( BoxColliderComponent& a_Component )
	{
		if ( auto* rb = TryGetComponentFromGameObject<RigidBodyComponent>( a_Component.GetGameObject() ) )
			InitComponent( *rb );
	}

	template<>
	void Scene::InitComponent( CapsuleColliderComponent& a_Component )
	{
		if ( auto* rb = TryGetComponentFromGameObject<RigidBodyComponent>( a_Component.GetGameObject() ) )
			InitComponent( *rb );
	}

	template<>
	void Scene::InitComponent( MeshColliderComponent& a_Component )
	{
		if ( auto* rb = TryGetComponentFromGameObject<RigidBodyComponent>( a_Component.GetGameObject() ) )
			InitComponent( *rb );
	}

}