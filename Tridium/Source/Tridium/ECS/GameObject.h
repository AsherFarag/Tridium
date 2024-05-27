#pragma once
#include "entt.hpp"

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	class TagComponent;
	class TransformComponent;

	typedef entt::entity EntityID;

	class GameObject
	{
		friend class Scene;
	public:
		GameObject( EntityID id = (EntityID)UINT32_MAX );
		operator EntityID ( ) { return m_ID; }
		operator const EntityID( ) const { return m_ID; }

		template <typename T, typename... Args>
		T& AddComponent( Args&&... args );

		template <typename T>
		inline T& GetComponent() const;

		template <typename T>
		inline bool HasComponent() const;

		template <typename T>
		inline void RemoveComponent();

		inline void Destroy() { Application::GetScene()->m_Registry.destroy( m_ID ); }
		inline bool IsValid() const { return Application::GetScene()->m_Registry.valid( m_ID ); }

		std::string& GetTag() const;
		TransformComponent& GetTransform() const;

	private:
		void Init( const std::string& name );

	private:
		EntityID m_ID;
	};



#pragma region Game Object Template Definitions

	template <typename T, typename... Args>
	T& GameObject::AddComponent( Args&&... args )
	{
		TE_CORE_ASSERT( !HasComponent<T>(), "GameObject already has this component!" );

		entt::registry& registry = Application::GetScene()->m_Registry;

		T& component = registry.emplace<T>( m_ID, std::forward<Args>( args )... );
		// T will always be a derived class of Component.
		// Set the GameObject ID to this GameObject
		static_cast<Component*>(&component)->m_GameObject = m_ID;

		if constexpr ( std::is_base_of_v<ScriptableComponent, T> )
		{
			static_cast<ScriptableComponent*>( &component )->OnConstruct();
		}

		return component;
	}

	template <typename T>
	T& GameObject::GetComponent() const
	{
		TE_CORE_ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return Application::GetScene()->m_Registry.get<T>( m_ID );
	}

	template <typename T>
	bool GameObject::HasComponent() const
	{
		return Application::GetScene()->m_Registry.any_of<T>( m_ID );
	}

	template <typename T>
	inline void GameObject::RemoveComponent()
	{
		if ( HasComponent<T>() )
			Application::GetScene()->m_Registry.remove<T>( m_ID );
	}

#pragma endregion

}