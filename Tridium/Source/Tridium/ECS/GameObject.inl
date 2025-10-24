#pragma once
#include "GameObject.h"

namespace Tridium {

	template <typename T, typename... Args>
	inline T& OldGameObject::AddComponent( Args&&... args )
	{
		return OldSceneManager::GetActiveScene()->AddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T, typename... Args>
	inline T* OldGameObject::TryAddComponent( Args&&... args )
	{
		return OldSceneManager::GetActiveScene()->TryAddComponentToGameObject<T>( *this, std::forward<Args>( args )... );
	}

	template <typename T>
	inline T& OldGameObject::GetComponent() const
	{
		ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return OldSceneManager::GetActiveScene()->GetComponentFromGameObject<T>( *this );
	}

	template <typename T>
	inline T* OldGameObject::TryGetComponent() const
	{
		return OldSceneManager::GetActiveScene()->TryGetComponentFromGameObject<T>( *this );
	}

	template<typename T>
	inline T* OldGameObject::TryGetComponentInChildren() const
	{
		const std::vector<OldGameObject>& children = GetChildren();
		for ( auto child : children )
		{
			if ( T* foundComponent = child.Internal_TryGetComponentInChildren<T>() )
				return foundComponent;
		}

		return nullptr;
	}

	template<typename T>
	inline T* OldGameObject::Internal_TryGetComponentInChildren() const
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
	inline bool OldGameObject::HasComponent() const
	{
		return OldSceneManager::GetActiveScene()->GameObjectHasComponent<T>( *this );
	}

	template <typename T>
	inline void OldGameObject::RemoveComponent()
	{
		OldSceneManager::GetActiveScene()->RemoveComponentFromGameObject<T>( *this );
	}

	inline bool Tridium::OldGameObject::IsValid() const
	{
		if ( m_ID == NullEntity )
			return false;

		return OldSceneManager::GetActiveScene()->IsGameObjectValid( *this );
	}

}