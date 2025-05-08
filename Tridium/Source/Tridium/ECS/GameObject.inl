#pragma once
#include "GameObject.h"

namespace Tridium {

	template <typename T, typename... Args>
	inline T& GameObject::AddComponent( Args&&... args )
	{
		return SceneManager::GetActiveScene()->AddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T, typename... Args>
	inline T* GameObject::TryAddComponent( Args&&... args )
	{
		return SceneManager::GetActiveScene()->TryAddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T>
	inline T& GameObject::GetComponent() const
	{
		ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return SceneManager::GetActiveScene()->GetComponentFromGameObject<T>( *this );
	}

	template <typename T>
	inline T* GameObject::TryGetComponent() const
	{
		return SceneManager::GetActiveScene()->TryGetComponentFromGameObject<T>( *this );
	}

	template<typename T>
	inline T* GameObject::TryGetComponentInChildren() const
	{
		const std::vector<GameObject>& children = GetChildren();
		for ( auto child : children )
		{
			if ( T* foundComponent = child.Internal_TryGetComponentInChildren<T>() )
				return foundComponent;
		}

		return nullptr;
	}

	template<typename T>
	inline T* GameObject::Internal_TryGetComponentInChildren() const
	{
		if ( T* foundComponent = TryGetComponent<T>() )
			return foundComponent;

		for ( auto child : GetChildren() )
		{
			if ( T* foundComponent = child.Internal_TryGetComponentInChildren<T>() )
				return foundComponent;
		}

		return nullptr;
	}

	template <typename T>
	inline bool GameObject::HasComponent() const
	{
		return SceneManager::GetActiveScene()->GameObjectHasComponent<T>( *this );
	}

	template <typename T>
	inline void GameObject::RemoveComponent()
	{
		SceneManager::GetActiveScene()->RemoveComponentFromGameObject<T>( *this );
	}

	inline bool Tridium::GameObject::IsValid() const
	{
		if ( m_ID == NullEntity )
			return false;

		return SceneManager::GetActiveScene()->IsGameObjectValid( *this );
	}

}