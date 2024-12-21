#include "Scene.h"

namespace Tridium {

	// Forward declarations
	class Component;
	class ScriptableComponent;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;
	class RigidBodyComponent;
	class SphereColliderComponent;
	class BoxColliderComponent;
	class CapsuleColliderComponent;
	class MeshColliderComponent;
	// -------------------

	//inline RayCastResult Tridium::Scene::CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_RayCastChannel, const PhysicsBodyFilter& a_BodyFilter, bool a_DrawDebug, Debug::EDrawDuration a_DrawDurationType, float a_DebugDrawDuration, Color a_DebugLineColor, Color a_DebugHitColor ) const
	//{
	//	RayCastResult result = m_PhysicsScene->CastRay( a_Start, a_Start + a_End, a_RayCastChannel, a_BodyFilter );

	//#if TE_DRAW_DEBUG
	//	if ( a_DrawDebug )
	//	{
	//		Debug::DrawLine( result.RayStart, result.RayEnd, a_DebugLineColor, a_DrawDurationType, a_DebugDrawDuration );

	//		if ( result.Hit )
	//		{
	//			TODO( "Draw a sphere at the hit position" );
	//			AABB aabb = { result.Position - Vector3( 0.1f ), result.Position + Vector3( 0.1f ) };
	//			Debug::DrawAABBFilled( aabb, a_DebugHitColor, a_DrawDurationType, a_DebugDrawDuration );
	//		}
	//	}
	//#endif

	//	return result;
	//}

	template<> void Scene::InitComponent( RigidBodyComponent& a_Component );
	template<> void Scene::InitComponent( SphereColliderComponent& a_Component );
	template<> void Scene::InitComponent( BoxColliderComponent& a_Component );
	template<> void Scene::InitComponent( CapsuleColliderComponent& a_Component );
	template<> void Scene::InitComponent( MeshColliderComponent& a_Component );

	template<typename T, typename ...Args>
	inline SharedPtr<T> Scene::AddSystem( Args && ...a_Args )
	{
		static_assert( std::is_base_of_v<ISceneSystem, T>, "T must be a derived class of ISceneSystem!" );
		static const size_t s_TypeHash = typeid( T ).hash_code();
		auto it = m_Systems.find( s_TypeHash );
		if ( it != m_Systems.end() )
		{
			TE_CORE_ASSERT( false, "Attempted to add a System that already exists!" );
			return SharedPtrCast<T>( it->second );
		}

		SharedPtr<T> system = MakeShared<T>( std::forward<Args>( a_Args )... );
		system->m_Scene = this;
		m_Systems[s_TypeHash] = system;
		return system;
	}

	template<typename T>
	inline SharedPtr<T> Scene::GetSystem()
	{
		static_assert( std::is_base_of_v<ISceneSystem, T>, "T must be a derived class of ISceneSystem!" );
		static const size_t s_TypeHash = typeid( T ).hash_code();
		auto it = m_Systems.find( s_TypeHash );
		if ( it != m_Systems.end() )
		{
			return SharedPtrCast<T>( it->second );
		}

		TE_CORE_WARN( "[SCENE] Attempted to get a System that does not exist!" );
		return nullptr;
	}

	template <typename T, typename... Args>
	inline T& Scene::AddComponentToGameObject( GameObject a_GameObject, Args&&... args )
	{
		TE_CORE_ASSERT( !GameObjectHasComponent<T>( a_GameObject ), "GameObject already has this component!" );

		T& component = m_Registry.emplace<T>( a_GameObject, std::forward<Args>( args )... );
		// T will always be a derived class of Component.
		// Set the GameObject ID to this GameObject
		static_cast<Component*>( &component )->m_GameObject = a_GameObject;

		TODO( "Implement a proper Component Initializer" )
			static bool SetupComponent = [&]{
			const auto OnDestroyComponent = [&]( entt::registry& a_Registry, entt::entity a_ID )
				{
					a_Registry.get< T >( a_ID ).OnDestroy();
				};
			m_Registry.on_destroy<T>().connect<OnDestroyComponent>();
			return true;
			}( );

		// Send OnComponentCreated event
		{
				SceneEventPayload payload =
				{
					.EventType = ESceneEventType::OnComponentCreated,
					.EventData = OnComponentCreatedEvent
					{
						.ComponentTypeID = entt::resolve<T>().id(),
						.Component = &component
					}
				};

				SendSceneEvent( payload );
		}

		if constexpr ( std::is_base_of_v<ScriptableComponent, T> )
		{
			auto scriptable = static_cast<ScriptableComponent*>( &component );

			if ( m_HasBegunPlay )
				scriptable->OnBeginPlay();
		}

		InitComponent( component );

		return component;
	}

	template <typename T, typename... Args>
	inline T* Scene::TryAddComponentToGameObject( GameObject a_GameObject, Args&&... args )
	{
		if ( GameObjectHasComponent<T>( a_GameObject ) )
		{
			return nullptr;
		}

		return &AddComponentToGameObject<T>( a_GameObject, std::forward<Args>( args )... );
	}

	template<typename T>
	inline T& Scene::GetComponentFromGameObject( GameObject a_GameObject )
	{
		TE_CORE_ASSERT( GameObjectHasComponent<T>( a_GameObject ), "GameObject does not have this component!" );
		return m_Registry.get<T>( a_GameObject );
	}

	template<typename T>
	inline T* Scene::TryGetComponentFromGameObject( GameObject a_GameObject )
	{
		return m_Registry.try_get<T>( a_GameObject );
	}

	template<typename T>
	inline bool Scene::GameObjectHasComponent( GameObject a_GameObject ) const
	{
		return m_Registry.any_of<T>( a_GameObject );
	}

	template<typename T>
	inline void Scene::RemoveComponentFromGameObject( GameObject a_GameObject )
	{
		TE_CORE_ASSERT( GameObjectHasComponent<T>( a_GameObject ), "GameObject does not have this component!" );
		m_Registry.remove<T>( a_GameObject );
	}

} // namespace Tridium