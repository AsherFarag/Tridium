#pragma once
#include <Tridium/Utils/Reflection/ReflectionFwd.h>

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	class Component;
	class ScriptableComponent;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;

	typedef entt::entity EntityID;

	// A GameObject is simply a wrapper around an EntityID.
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

		[[nodiscard]] std::vector<Component*>&& GetAllComponents() const;

		static inline GameObject Create() { return Application::GetScene()->InstantiateGameObject(); }
		static inline GameObject Create( GUID a_GUID, const std::string& a_Name ) { return Application::GetScene()->InstantiateGameObject( a_GUID, a_Name ); }
		inline void Destroy() { Application::GetScene()->m_Registry.destroy( m_ID ); }
		inline bool IsValid() const { return Application::GetScene()->m_Registry.valid( m_ID ); }

		GUID GetGUID() const;
		const inline EntityID ID() const { return m_ID; }
		std::string& GetTag() const;

		// - Transform Helpers -

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

		// --------------------

	private:
		EntityID m_ID;
	};
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

#include "GameObject.inl"