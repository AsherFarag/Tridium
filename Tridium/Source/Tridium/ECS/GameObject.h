#pragma once
#include <Tridium/Reflection/ReflectionFwd.h>
#include <Tridium/Reflection/MetaTypes.h>
#include <Tridium/Scene/Scene.h>

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	// Forward declarations
	class Component;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;
	// -------------------

	// A GameObject is simply a wrapper around an EntityID.
	class GameObject
	{
		REFLECT( GameObject );
	public:
		GameObject() : m_ID( NullEntity ) {}
		GameObject( EntityID a_ID ) : m_ID( a_ID ) {}
		GameObject( EntityIDType a_ID ) : m_ID( static_cast<EntityID>( a_ID ) ) {}
		~GameObject() = default;

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
		inline T* TryGetComponentInChildren() const;

		template <typename T>
		inline bool HasComponent() const;

		template <typename T>
		inline void RemoveComponent();

		[[nodiscard]] std::vector<std::pair<Refl::MetaType, Component*>> GetAllComponents() const;

		static inline GameObject Create() { return Application::GetScene()->InstantiateGameObject(); }
		static inline GameObject Create( GUID a_GUID, const std::string& a_Name ) { return Application::GetScene()->InstantiateGameObject( a_GUID, a_Name ); }
		inline void Destroy() { Application::GetScene()->DestroyGameObject( *this ); }
		inline bool IsValid() const;
		inline void CopyFrom( GameObject a_Other ) { Application::GetScene()->CopyGameObject( *this, a_Other ); }

		GUID GetGUID() const;
		const EntityID ID() const { return m_ID; }
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
		const std::vector<GameObject>& GetChildren() const;

		// --------------------

	private:
		template <typename T>
		inline T* Internal_TryGetComponentInChildren() const;

	private:
		EntityID m_ID;

		friend class Scene;
	};
}

namespace std {
	template<>
	struct hash<Tridium::GameObject>
	{
		inline size_t operator()( const Tridium::GameObject& go ) const 
		{
			return hash<Tridium::EntityID>()( go.ID() );
		}
	};
}

#include "GameObject.inl"