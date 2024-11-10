#pragma once
#include "GameObject.h"

namespace Tridium {

	template <typename T, typename... Args>
	inline T& GameObject::AddComponent( Args&&... args )
	{
		return Application::GetScene()->AddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T, typename... Args>
	inline T* GameObject::TryAddComponent( Args&&... args )
	{
		return Application::GetScene()->TryAddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T>
	inline T& GameObject::GetComponent() const
	{
		TE_CORE_ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return Application::GetScene()->GetComponentFromGameObject<T>( *this );
	}

	template <typename T>
	inline T* GameObject::TryGetComponent() const
	{
		return Application::GetScene()->TryGetComponentFromGameObject<T>( *this );
	}

	template <typename T>
	inline bool GameObject::HasComponent() const
	{
		return Application::GetScene()->GameObjectHasComponent<T>( *this );
	}

	template <typename T>
	inline void GameObject::RemoveComponent()
	{
		Application::GetScene()->RemoveComponentFromGameObject<T>( *this );
	}

	inline bool Tridium::GameObject::IsValid() const
	{
		if ( m_ID == entt::null )
			return false;

		return Application::GetScene()->IsGameObjectValid( *this );
	}

}