#pragma once
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	//=================================================================================================
	// GameObject: Simple struct containing an EntityID and a pointer to the owning Scene.
	// Since GameObjects are small, simple POD types, they can be passed around by value.
	//=================================================================================================
	struct GameObject final
	{
	private:

		//=============================================================================================
		Scene* m_Scene = nullptr;
		EntityID m_EntityID = NullEntity;

	public:

		//=============================================================================================
		GameObject() = default;
		GameObject( Scene* a_Scene, EntityID a_EntityID ) : m_Scene( a_Scene ), m_EntityID( a_EntityID ) {}
		GameObject( Scene& a_Scene, EntityID a_EntityID ) : m_Scene( &a_Scene ), m_EntityID( a_EntityID ) {}
		~GameObject() = default;

		//=============================================================================================
		operator EntityID() const { return m_EntityID; }
		operator bool() const { return Valid(); }

		//=============================================================================================
		bool operator==( const GameObject& a_Other ) const
		{
			return m_Scene == a_Other.m_Scene && m_EntityID == a_Other.m_EntityID;
		}

		//=============================================================================================
		// Returns the underlying EntityID of this GameObject.
		[[nodiscard]] EntityID ID() const { return m_EntityID; }

		//=============================================================================================
		// Returns a pointer to the Scene that owns this GameObject.
		[[nodiscard]] Scene* Scene() const { return m_Scene; }

		//=============================================================================================
		// Checks if this GameObject is valid and usable.
		[[nodiscard]] bool Valid() const
		{
			return m_Scene && m_EntityID != NullEntity && m_Scene->Registry().Valid( m_EntityID );
		}

		//=============================================================================================
		// Constructs a new component of type T with the passed in arguments,
		// adds it to the GameObject and returns a reference to it.
		// This will assert if the component already exists.
		template<typename T, typename... _Args>
		T& Add( _Args&&... a_Args )
		{
			return m_Scene->Registry().Emplace<T>( m_EntityID, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get() const
		{
			return m_Scene->Registry().Get<T...>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has the component,
		// if not it creates a new component with the passed in arguments and returns a reference to it.
		// If the component already exists, it returns a reference to the existing component.
		template<typename T, typename... _Args>
		T& GetOrAdd( _Args&&... a_Args )
		{
			return m_Scene->Registry().GetOrEmplace<T>( m_EntityID, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns pointers to the components of types T...
		template<typename... T>
		[[nodiscard]] auto TryGet() const -> decltype( m_Scene->Registry().TryGet<T...>( m_EntityID ) )
		{
			if ( Valid() )
			{
				return m_Scene->Registry().TryGet<T...>( m_EntityID );
			}

			return {};
		}

		//=============================================================================================
		// Checks if the GameObject has the specified components.
		template<typename T>
		[[nodiscard]] bool Has() const
		{
			return m_Scene->Registry().AnyOf<T>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has any of the specified components.
		template<typename... T>
		[[nodiscard]] bool HasAny() const
		{
			return m_Scene->Registry().AnyOf<T...>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has all of the specified components.
		template<typename... T>
		[[nodiscard]] bool HasAll() const
		{
			return m_Scene->Registry().AllOf<T...>( m_EntityID );
		}

		//=============================================================================================
		// Removes the specified components from the GameObject.
		template<typename... T>
		void Remove()
		{
			m_Scene->Registry().Remove<T...>( m_EntityID );
		}

		//=============================================================================================
		// Computes and returns the world position of this GameObject by traversing up the hierarchy.
		Vector3 GetWorldPosition() const
		{
			TransformComponent* transform = TryGet<TransformComponent>();
			if ( !transform )
			{
				return Vector3::Zero();
			}

			Vector3 worldPosition = transform->LocalPosition();
			GameObject currentParent = GetParent();

			while ( currentParent.Valid() )
			{
				TransformComponent* parentTransform = currentParent.TryGet<TransformComponent>();
				if ( parentTransform )
				{
					worldPosition = parentTransform->LocalPosition() + worldPosition;
				}
				currentParent = currentParent.GetParent();
			}

			return worldPosition;
		}

		//=============================================================================================
		// Computes and returns the world transform matrix of this GameObject by traversing up the hierarchy.
		Matrix4 GetWorldTransform() const
		{
			TransformComponent* transform = TryGet<TransformComponent>();
			if ( !transform )
			{
				return Matrix4( 1.0f );
			}

			Matrix4 worldTransform = transform->LocalTransform();
			GameObject currentParent = GetParent();

			while ( currentParent.Valid() )
			{
				TransformComponent* parentTransform = currentParent.TryGet<TransformComponent>();
				if ( parentTransform )
				{
					worldTransform = parentTransform->LocalTransform() * worldTransform;
				}

				currentParent = currentParent.GetParent();
			}

			return worldTransform;
		}

		//=============================================================================================
		// Returns the parent GameObject, or a null GameObject if there is no parent.
		// Requires HierarchyComponent.
		GameObject GetParent() const
		{
			HierarchyComponent* hierarchy = TryGet<HierarchyComponent>();
			return hierarchy ? GameObject( m_Scene, hierarchy->Parent ) : GameObject();
		}

		//=============================================================================================
		// Recursively searches up the hierarchy and returns the root parent GameObject.
		// The root parent is the top-most ancestor in the hierarchy, which has no parent.
		// If this GameObject has no parent, it returns itself.
		// Requires HierarchyComponent.
		GameObject GetRootParent() const
		{
			GameObject current = *this;
			GameObject parent = current.GetParent();

			while ( parent.Valid() )
			{
				current = parent;
				parent = current.GetParent();
			}

			return current;
		}

		//=============================================================================================
		// Checks if this GameObject is the root in its hierarchy (i.e., has no parent).
		// Requires HierarchyComponent.
		bool IsRoot() const
		{
			return !GetParent().Valid();
		}

		//=============================================================================================
		// Sets the parent of this GameObject to the specified new parent.
		void SetParent( const GameObject& a_NewParent )
		{
			HierarchyComponent& hierarchy = GetOrAdd<HierarchyComponent>();

			// Remove from current parent if exists
			if ( hierarchy.Parent != NullEntity )
			{
				GameObject currentParent( m_Scene, hierarchy.Parent );
				HierarchyComponent& parentHierarchy = currentParent.GetOrAdd<HierarchyComponent>();

				if ( parentHierarchy.FirstChild == m_EntityID )
				{
					parentHierarchy.FirstChild = hierarchy.NextSibling;
				}

				if ( hierarchy.PrevSibling != NullEntity )
				{
					HierarchyComponent& prevSiblingHierarchy = m_Scene->Registry().Get<HierarchyComponent>( hierarchy.PrevSibling );
					prevSiblingHierarchy.NextSibling = hierarchy.NextSibling;
				}

				if ( hierarchy.NextSibling != NullEntity )
				{
					HierarchyComponent& nextSiblingHierarchy = m_Scene->Registry().Get<HierarchyComponent>( hierarchy.NextSibling );
					nextSiblingHierarchy.PrevSibling = hierarchy.PrevSibling;
				}
			}

			hierarchy.Parent = a_NewParent.ID();
		}

	};

} // namespace Tridium