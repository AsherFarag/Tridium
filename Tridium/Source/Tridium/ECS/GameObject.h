#pragma once
#include "Reflection.h"

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	class TagComponent;
	class GUIDComponent;
	class TransformComponent;

	typedef entt::entity EntityID;

	class GameObject
	{
		REFLECT;
		friend class Scene;
	public:
		GameObject( EntityID id = entt::null );
		operator uint32_t () { return (uint32_t)m_ID; }
		operator const uint32_t() const { return (uint32_t)m_ID; }
		operator EntityID () { return m_ID; }
		operator const EntityID() const { return m_ID; }
		operator bool () { return IsValid(); }
		operator const bool() const { return IsValid(); }
		bool operator ==( const GameObject& other ) const { return m_ID == other.m_ID; }
		bool operator !=( const GameObject& other ) const { return m_ID != other.m_ID; }

		template <typename T, typename... Args>
		T& AddComponent( Args&&... args );

		template <typename T, typename... Args>
		T* TryAddComponent( Args&&... args );

		template <typename T>
		inline T& GetComponent() const;

		template <typename T>
		inline T* TryGetComponent() const;

		template <typename T>
		inline bool HasComponent() const;

		template <typename T>
		inline void RemoveComponent();

		static inline GameObject Create() { return Application::GetScene()->InstantiateGameObject(); }
		static inline GameObject Create( GUID a_GUID, const std::string& a_Name ) { return Application::GetScene()->InstantiateGameObject( a_GUID, a_Name ); }
		inline void Destroy() { Application::GetScene()->m_Registry.destroy( m_ID ); }
		inline bool IsValid() const { return Application::GetScene()->m_Registry.valid( m_ID ); }

		GUID GetGUID() const;
		const inline EntityID ID() const { return m_ID; }
		std::string& GetTag() const;

		TransformComponent& GetTransform() const;
		Matrix4 GetWorldTransform() const;
		Matrix4 GetLocalTransform() const;
		bool HasParent() const;
		GameObject GetParent() const;
		void AttachToParent( GameObject a_Parent );
		void DetachFromParent();
		void AttachChild( GameObject a_Child );
		void DetachChild( GameObject a_Child );
		GameObject GetChild( const std::string& a_Tag ) const; /* Slow operation, avoid if possible. */
		std::vector<GameObject>& GetChildren();

	private:
		EntityID m_ID;
	};



#pragma region GameObject Template Definitions

	template <typename T, typename... Args>
	T& GameObject::AddComponent( Args&&... args )
	{
		TE_CORE_ASSERT( !HasComponent<T>(), "GameObject already has this component!" );

		entt::registry& registry = Application::GetScene()->m_Registry;

		T& component = registry.emplace<T>( m_ID, std::forward<Args>( args )... );
		// T will always be a derived class of Component.
		// Set the GameObject ID to this GameObject
		static_cast<Component*>(&component)->m_GameObject = m_ID;

		TODO( "Implement a proper Component Initializer" )
		static bool SetupComponent = []{
			const auto OnDestroyComponent = +[]( entt::registry& a_Registry, entt::entity a_ID )
			{
				a_Registry.get< T >( a_ID ).OnDestroy();
			};
			Application::GetScene()->m_Registry.on_destroy<T>().connect<OnDestroyComponent>();
			return true;
		}();

		if constexpr ( std::is_base_of_v<ScriptableComponent, T> )
		{
			auto scriptable = static_cast<ScriptableComponent*>( &component );

			scriptable->OnConstruct();
		}

		return component;
	}

	template <typename T, typename... Args>
	T* GameObject::TryAddComponent( Args&&... args )
	{
		if ( HasComponent<T>() )
		{
			TE_CORE_ERROR( "'{0}' already has a component of type '{1}'!", GetTag(), typeid( T ).name() );
			return nullptr;
		}

		return &AddComponent<T>( std::forward<Args>( args )... );
	}

	template <typename T>
	T& GameObject::GetComponent() const
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

namespace std {
	template<>
	struct hash<Tridium::GameObject>
	{
		size_t operator()( const Tridium::GameObject& go ) const 
		{
			return (size_t)go.ID();
		}
	};
}