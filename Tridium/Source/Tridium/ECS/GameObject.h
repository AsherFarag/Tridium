#pragma once
#include <Tridium/Reflection/ReflectionFwd.h>
#include <Tridium/Reflection/MetaTypes.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Scene/SceneManager.h>
#include <Tridium/ECS/Components/CoreComponents.h>

namespace Tridium {

	// Forward declarations
	class Component;
	class TagComponent;
	class GUIDComponent;
	class TransformComponent;

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
	class OldGameObject
	{
		REFLECT( OldGameObject );
	public:
		OldGameObject() : m_ID( NullEntity ) {}
		OldGameObject( EntityID a_ID ) : m_ID( a_ID ) {}
		OldGameObject( EntityIDType a_ID ) : m_ID( Cast<EntityID>( a_ID ) ) {}
		~OldGameObject() = default;

		operator EntityID () { return m_ID; }
		operator const EntityID() const { return m_ID; }
		operator bool () { return IsValid(); }
		operator const bool() const { return IsValid(); }
		bool operator ==( const OldGameObject& other ) const { return m_ID == other.m_ID; }
		bool operator !=( const OldGameObject& other ) const { return m_ID != other.m_ID; }

		static OldGameObject Create() { return SceneManager::GetActiveScene()->InstantiateGameObject(); }
		static OldGameObject Create( GUID a_GUID, const std::string& a_Name ) { return SceneManager::GetActiveScene()->InstantiateGameObject( a_GUID, a_Name ); }
		inline void Destroy() { SceneManager::GetActiveScene()->DestroyGameObject( *this ); }
		inline void CopyFrom( OldGameObject a_Other ) { SceneManager::GetActiveScene()->CopyGameObject( *this, a_Other ); }

		GUID GetGUID() const;
		EntityID ID() const { return m_ID; }
		TagComponent* GetTagComponent() const;
		const String& GetTag() const;

		//================================================================
		// GameObject Flags
		EnumFlags<EGameObjectFlags> GetFlags();
		bool IsValid() const;
		bool IsActive() { return GetFlags().HasFlag( EGameObjectFlags::Active ); }
		void SetActive( bool a_Active, bool a_PropagateToChildren = false );
		bool IsVisible() { return GetFlags().HasFlag( EGameObjectFlags::Visible ); }
		void SetVisible( bool a_Visible, bool a_PropagateToChildren = false );
		bool IsEnabled() { return GetFlags().HasFlag( EGameObjectFlags::Enabled ); }
		void SetEnabled( bool a_Enabled, bool a_PropagateToChildren = false );
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
		[[nodiscard]] Array<Pair<Refl::MetaType, Component*>> GetAllComponents() const;
		//================================================================

		//================================================================
		// Transform Functions
		TransformComponent& GetTransform() const;
		Matrix4 GetWorldTransform() const;
		Matrix4 GetLocalTransform() const;
		bool HasParent() const;
		OldGameObject GetParent() const;
		void AttachToParent( OldGameObject a_Parent );
		void DetachFromParent();
		void AttachChild( OldGameObject a_Child );
		void DetachChild( OldGameObject a_Child );
		OldGameObject GetChild( const std::string& a_Tag ) const; /* Slow operation, avoid if possible. */
		std::vector<OldGameObject>& GetChildren();
		const std::vector<OldGameObject>& GetChildren() const;
		//================================================================

	private:
		EntityID m_ID;

	private:
		template <typename T>
		inline T* Internal_TryGetComponentInChildren() const;

		friend class OldScene;
	};
}

namespace std {
	template<>
	struct hash<Tridium::OldGameObject>
	{
		inline size_t operator()( const Tridium::OldGameObject& go ) const 
		{
			return hash<Tridium::EntityID>()( go.ID() );
		}
	};
}

#include "GameObject.inl"