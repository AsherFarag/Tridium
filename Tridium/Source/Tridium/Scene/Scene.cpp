#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

	Scene::Scene(const std::string& name)
		: m_Name( name ), m_SceneRenderer(*this)
	{
		m_PhysicsScene = PhysicsScene::Create();
		m_PhysicsScene->m_Scene = this;
	}

	Scene::Scene( const Scene& a_Other )
		: m_Name( a_Other.m_Name ), m_SceneRenderer( *this )
	{
		m_MainCamera = a_Other.m_MainCamera;
		m_SceneEnvironment = a_Other.m_SceneEnvironment;
		m_Paused = a_Other.m_Paused;
		m_SceneRenderer.m_LightEnvironment = a_Other.m_SceneRenderer.m_LightEnvironment;
		m_SceneRenderer.m_RenderSettings = a_Other.m_SceneRenderer.m_RenderSettings;

		m_PhysicsScene = PhysicsScene::Create();
		m_PhysicsScene->m_Scene = this;

		// Copy registry
		{
			const entt::registry& src = a_Other.m_Registry;

			// Map from old entity IDs to new entity IDs
			std::unordered_map<entt::entity, entt::entity> entityMap;

			// Step 1: Create entities in the new registry
			for ( auto entity : src.view<GUIDComponent>() )
			{
				entt::entity newEntity = m_Registry.create();
				entityMap[entity] = newEntity;
			}

			// Step 2: Copy components
			for ( auto [id, srcStorage] : src.storage() )
			{
				auto* dstStorage = m_Registry.storage( id );
				for ( auto entity : srcStorage )
				{
					entt::entity newEntity = entityMap[entity];

					Refl::MetaType componentType = entt::resolve( srcStorage.type() );
					Refl::Internal::AddToGameObjectFunc addToGameObjectFunc;
					if ( !Refl::MetaRegistry::TryGetMetaPropertyFromClass( componentType, addToGameObjectFunc, Refl::Internal::AddToGameObjectPropID ) )
					{
						TE_CORE_ASSERT( false );
						continue;
					}

					Refl::MetaAny dstComponent = componentType.from_void( addToGameObjectFunc( *this, newEntity ) );
					Refl::MetaAny srcComponent = componentType.from_void( srcStorage.value( entity ) );
					dstComponent.assign( srcComponent );
				}
			}
		}
	}

	Scene::~Scene()
	{
		m_PhysicsScene = nullptr;
	}

	void Scene::OnBegin()
	{
		m_IsRunning = true;

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
	}

	void Scene::OnUpdate()
	{
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

			m_PhysicsScene->Tick( 1.0f / 60.0f );

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

		// Shutdown Physics Scene
		{
			m_PhysicsScene->Shutdown();
		}
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
		return go;
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go, a_GUID );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
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

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<GUIDComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TagComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TransformComponent>().info() )
				continue;

			if ( storage.contains(dst) )
				storage.erase( dst );

			storage.push( dst, storage.value( a_Source ) );
		}

		return dst;
	}

	void Scene::CopyGameObject( GameObject a_Destination, GameObject a_Source )
	{

	}

	bool Scene::IsGameObjectValid( GameObject a_GameObject ) const
	{
		return m_Registry.valid( a_GameObject.m_ID );
	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	CameraComponent* Scene::GetMainCamera()
	{
		// If the scene doesn't have a main camera,
		// get the first camera in the scene
		if ( !m_Registry.valid( m_MainCamera ) )
		{
			auto cameras = m_Registry.view<CameraComponent>();
			// Return nullptr as there are no cameras in the scene
			if ( cameras.empty() )
				return nullptr;

			m_MainCamera = cameras.front();
		}
		return m_Registry.try_get<CameraComponent>( m_MainCamera );
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