#pragma once
#include "entt.hpp"

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	class TagComponent;
	class TransformComponent;

	typedef entt::entity EntityID;

#define INVALID_GAMEOBJECT (EntityID)UINT32_MAX


	class GameObject
	{
		friend class Scene;
	public:
		GameObject( EntityID id = INVALID_GAMEOBJECT );
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

		const inline EntityID ID() const { return m_ID; }
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
			TODO("Implement a proper Component Initializer")
			static bool SetupScriptableComponent = [] {

				const auto OnDestroyScriptableObject = +[]( entt::registry& a_Registry, entt::entity a_ID )
				{
					a_Registry.get< T >( a_ID ).OnDestroy();
				};
				Application::GetScene()->m_Registry.on_destroy<T>().connect<OnDestroyScriptableObject>();
				return true;
			}();


			auto scriptable = static_cast<ScriptableComponent*>( &component );

			scriptable->OnConstruct();
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