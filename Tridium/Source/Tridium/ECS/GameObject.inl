#pragma once
#include "GameObject.h"

namespace Tridium {

	template <typename T, typename... Args>
	inline T& GameObject::AddComponent( Args&&... args )
	{
		TE_CORE_ASSERT( !HasComponent<T>(), "GameObject already has this component!" );

		entt::registry& registry = Application::GetScene()->m_Registry;

		T& component = registry.emplace<T>( m_ID, std::forward<Args>( args )... );
		// T will always be a derived class of Component.
		// Set the GameObject ID to this GameObject
		static_cast<Component*>( &component )->m_GameObject = m_ID;

		TODO( "Implement a proper Component Initializer" )
			static bool SetupComponent = [] {
			const auto OnDestroyComponent = +[]( entt::registry& a_Registry, entt::entity a_ID )
				{
					a_Registry.get< T >( a_ID ).OnDestroy();
				};
			Application::GetScene()->m_Registry.on_destroy<T>().connect<OnDestroyComponent>();
			return true;
			}( );

		if constexpr ( std::is_base_of_v<ScriptableComponent, T> )
		{
			auto scriptable = static_cast<ScriptableComponent*>( &component );

			scriptable->OnConstruct();
		}

		return component;
	}

	template <typename T, typename... Args>
	inline T* GameObject::TryAddComponent( Args&&... args )
	{
		if ( HasComponent<T>() )
		{
			TE_CORE_ERROR( "'{0}' already has a component of type '{1}'!", GetTag(), typeid( T ).name() );
			return nullptr;
		}

		return &AddComponent<T>( std::forward<Args>( args )... );
	}

	template <typename T>
	inline T& GameObject::GetComponent() const
	{
		TE_CORE_ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return Application::GetScene()->m_Registry.get<T>( m_ID );
	}

	template <typename T>
	inline T* GameObject::TryGetComponent() const
	{
		return Application::GetScene()->m_Registry.try_get<T>( m_ID );
	}

	template <typename T>
	inline bool GameObject::HasComponent() const
	{
		return Application::GetScene()->m_Registry.any_of<T>( m_ID );
	}

	template <typename T>
	inline void GameObject::RemoveComponent()
	{
		if ( HasComponent<T>() )
			Application::GetScene()->m_Registry.remove<T>( m_ID );
	}

	inline bool Tridium::GameObject::IsValid() const
	{
		if ( m_ID == entt::null )
			return false;

		if ( !Application::GetScene() )
			return false;

		return Application::GetScene()->m_Registry.valid( m_ID );
	}

}