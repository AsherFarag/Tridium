#pragma once
#include <Tridium/Reflection/ReflectionFwd.h>
#include <Tridium/Reflection/MetaTypes.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Scene/SceneManager.h>
#include <Tridium/Core/EnumFlags.h>

namespace Tridium {

	// Forward declarations
	class Component;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;

	//================================================================
	// GameObject Flags
	//  Bit flags used to represent the state of a GameObject.
	namespace EGameObjectFlag {
		enum Type : uint32_t
		{
			None = 0,
			// Is this GameObject pending destruction?
			PendingKill = 1 << 0,
			// If not active, this GameObject will not be updated.
			Active = 1 << 1,
			// If not visible, this GameObject will not be rendered.
			Visible = 1 << 2,
		};
	}

	using GameObjectFlags = EnumFlags<EGameObjectFlag::Type>;
	static constexpr GameObjectFlags DefaultGameObjectFlags{ EGameObjectFlag::Active | EGameObjectFlag::Visible };

	//================================================================

	//================================================================
	// GameObject
	//  GameObjects are the main objects in the ECS.
	//  They are used to represent entities in the scene.
	//  GameObjects can have components attached to them.
	//
	//  GameObjects are essentially wrappers around EntityIDs.
	//  They provide a user-friendly interface for interacting for
	//  interacting with a scene and its ECS.
	//================================================================
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

		static GameObject Create() { return SceneManager::GetActiveScene()->InstantiateGameObject(); }
		static GameObject Create( GUID a_GUID, const std::string& a_Name ) { return SceneManager::GetActiveScene()->InstantiateGameObject( a_GUID, a_Name ); }
		inline void Destroy() { SceneManager::GetActiveScene()->DestroyGameObject( *this ); }
		inline void CopyFrom( GameObject a_Other ) { SceneManager::GetActiveScene()->CopyGameObject( *this, a_Other ); }

		GUID GetGUID() const;
		const EntityID ID() const { return m_ID; }
		std::string& GetTag() const;

		//================================================================
		// GameObject Flags
		GameObjectFlags GetFlags();
		bool IsValid() const;
		bool IsActive() { return GetFlags().HasFlag( EGameObjectFlag::Active ); }
		void SetActive( bool a_Active, bool a_PropagateToChildren = false );
		bool IsVisible() { return GetFlags().HasFlag( EGameObjectFlag::Visible ); }
		void SetVisible( bool a_Visible, bool a_PropagateToChildren = false );
		//================================================================

		//================================================================
		// Component Functions
		// 
		// Constructs a new component of type T with the passed in arguments,
		// adds it to the GameObject and returns a reference to it.
		template <typename T, typename... Args>
		T& AddComponent( Args&&... args );
		// Checks if the GameObject has the component,
		// if not it creates a new component with the passed in arguments and returns a reference to it.
		// If the component already exists, it returns a reference to the existing component.
		template <typename T, typename... Args>
		T* TryAddComponent( Args&&... args );
		// Retrieves the component from the GameObject.
		template <typename T>
		inline T& GetComponent() const;
		// Attempts to retrieve the component from the GameObject if it exists.
		template <typename T>
		inline T* TryGetComponent() const;
		// Tries to get the requested component from the GameObject's children.
		template <typename T>
		inline T* TryGetComponentInChildren() const;
		// Checks if the GameObject has the requested component.
		template <typename T>
		inline bool HasComponent() const;
		// Removes the component from the GameObject.
		template <typename T>
		inline void RemoveComponent();
		// Retrieves all components attached to the GameObject with an associated MetaType.
		[[nodiscard]] std::vector<std::pair<Refl::MetaType, Component*>> GetAllComponents() const;
		//================================================================

		//================================================================
		// Transform Functions
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
		//================================================================

	private:
		EntityID m_ID;

	private:
		template <typename T>
		inline T* Internal_TryGetComponentInChildren() const;

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