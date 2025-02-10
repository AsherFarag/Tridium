#include "Scene.h"

namespace Tridium {

	// Forward declarations
	class Component;
	class NativeScriptComponent;
	// -------------------


	//////////////////////////////////////////////////////////////////////////
	// Scene Systems
	//////////////////////////////////////////////////////////////////////////

	template<typename T, typename ...Args> requires Concepts::IsBaseOf<ISceneSystem, T>
	inline SharedPtr<T> Scene::AddSystem( Args && ...a_Args )
	{
		static constexpr size_t s_TypeHash = entt::type_hash<T>::value();
		auto it = m_Systems.find( s_TypeHash );
		if ( !ASSERT_LOG( it == m_Systems.end(), "Attempted to add a System that already exists!" ) )
		{
			return SharedPtrCast<T>( it->second );
		}

		SharedPtr<T> system = MakeShared<T>( std::forward<Args>( a_Args )... );
		system->m_Scene = this;
		m_Systems[s_TypeHash] = system;
		return system;
	}

	template<typename T> requires Concepts::IsBaseOf<ISceneSystem, T>
	inline SharedPtr<T> Scene::GetSystem()
	{
		static_assert( std::is_base_of_v<ISceneSystem, T>, "T must be a derived class of ISceneSystem!" );
		static const size_t s_TypeHash = typeid( T ).hash_code();
		auto it = m_Systems.find( s_TypeHash );
		if ( it != m_Systems.end() )
		{
			return SharedPtrCast<T>( it->second );
		}

		LOG( LogCategory::Scene, Warn, "[SCENE] Attempted to get a System that does not exist!" );
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	// GameObjects and Components
	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void Scene::OnComponentCreated( entt::registry& a_Registry, entt::entity a_Entity )
	{
		GameObject gameObject( a_Entity );
		T& component = a_Registry.get<T>( a_Entity );

		// Set the GameObject for the component
		if constexpr ( Concepts::IsBaseOf<Component, T> )
		{
			component.m_GameObject = gameObject;
		}

		// Send an OnComponentCreated event to all scene systems
		{
			static const Refl::MetaIDType componentTypeID = entt::resolve<T>().id();
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnComponentCreated,
				.EventData = OnComponentCreatedEvent
				{
					.GameObjectID = gameObject,
					.ComponentTypeID = componentTypeID,
					.Component = &component
				}
			};
			SendSceneEvent( payload );
		}

		if constexpr ( Concepts::HasOnBeginPlayFunction<T> )
		{
			if ( HasBegunPlay() )
			{
				component.OnBeginPlay();
			}
		}
	}

	template<typename T>
	inline void Scene::OnComponentDestroyed( entt::registry& a_Registry, entt::entity a_Entity )
	{
		// Send an OnComponentDestroyed event to all scene systems
		{
			static const Refl::MetaIDType componentTypeID = entt::resolve<T>().id();
			SceneEventPayload payload =
			{
				.EventType = ESceneEventType::OnComponentDestroyed,
				.EventData = OnComponentDestroyedEvent
				{
					.ComponentTypeID = componentTypeID,
					.Component = a_Registry.try_get<T>( a_Entity )
				}
			};
			SendSceneEvent( payload );
		}

		if constexpr ( Concepts::HasOnEndPlayFunction<T> )
		{
			if ( HasBegunPlay() )
			{
				T& component = a_Registry.get<T>( a_Entity );
				component.OnEndPlay();
			}
		}
	}

	template<typename T>
	inline bool Scene::__InitComponentType()
	{
		m_ECS.GetRegistry().on_construct<T>().connect<&Scene::OnComponentCreated<T>>( *this );
		m_ECS.GetRegistry().on_destroy<T>().connect<&Scene::OnComponentDestroyed<T>>( *this );

		// Add a component ticker if the component is a NativeScriptComponent and has an OnUpdate or OnBeginPlay function
		if constexpr ( Concepts::HasOnUpdateFunction<T> || Concepts::HasOnBeginPlayFunction<T> )
		{
			AddEntityTicker<ComponentTicker<T>>();
		}

		return true;
	}

	template<typename T, typename ..._Args>
	inline void Scene::AddEntityTicker( _Args && ...a_Args )
	{
		UniquePtr<IEntityTicker> ticker;
		ticker.reset( new T( std::forward<_Args>( a_Args )... ) );
		ticker->m_ECS = &m_ECS;
		m_EntityTickers.push_back( std::move( ticker ) );
	}

	template <typename T, typename... Args>
	inline T& Scene::AddComponentToGameObject( GameObject a_GameObject, Args&&... args )
	{
		CORE_ASSERT_LOG( !GameObjectHasComponent<T>( a_GameObject ), "GameObject already has this component!" );
		T& component = m_ECS.AddComponentToEntity<T>( a_GameObject, std::forward<Args>( args )... );
		return component;
	}

	template <typename T, typename... Args>
	inline T* Scene::TryAddComponentToGameObject( GameObject a_GameObject, Args&&... args )
	{
		if ( T* existingComponent = TryGetComponentFromGameObject<T>( a_GameObject ) )
		{
			return existingComponent;
		}

		return &AddComponentToGameObject<T>( a_GameObject, std::forward<Args>( args )... );
	}

	template<typename T>
	inline T& Scene::GetComponentFromGameObject( GameObject a_GameObject )
	{
		CORE_ASSERT_LOG( GameObjectHasComponent<T>( a_GameObject ), "GameObject does not have this component!" );
		return m_ECS.GetComponentFromEntity<T>( a_GameObject );
	}

	template<typename T>
	inline T* Scene::TryGetComponentFromGameObject( GameObject a_GameObject )
	{
		return m_ECS.TryGetComponentFromEntity<T>( a_GameObject );
	}

	template<typename T>
	inline bool Scene::GameObjectHasComponent( GameObject a_GameObject ) const
	{
		return m_ECS.EntityHasComponent<T>( a_GameObject );
	}

	template<typename T>
	inline void Scene::RemoveComponentFromGameObject( GameObject a_GameObject )
	{
		CORE_ASSERT_LOG( GameObjectHasComponent<T>( a_GameObject ), "GameObject does not have this component!" );
		m_ECS.RemoveComponentFromEntity<T>( a_GameObject );
	}

} // namespace Tridium