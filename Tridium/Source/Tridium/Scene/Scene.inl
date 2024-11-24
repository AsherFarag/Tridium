#include "Scene.h"

namespace Tridium {

	class Component;
	class ScriptableComponent;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;
	// Physics Components
	class RigidBodyComponent;
	class SphereColliderComponent;
	class BoxColliderComponent;
	class CapsuleColliderComponent;
	class MeshColliderComponent;

	template<> void Scene::InitComponent( RigidBodyComponent& a_Component );
	template<> void Scene::InitComponent( SphereColliderComponent& a_Component );
	template<> void Scene::InitComponent( BoxColliderComponent& a_Component );
	template<> void Scene::InitComponent( CapsuleColliderComponent& a_Component );
	template<> void Scene::InitComponent( MeshColliderComponent& a_Component );

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