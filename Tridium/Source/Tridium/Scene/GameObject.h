#pragma once
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	//=================================================================================================
	// GameObject: Simple struct containing an Entity and a pointer to the owning Scene.
	// Since GameObjects are small, simple POD types, they can be passed around by value.
	//=================================================================================================
	struct GameObject final
	{
	private:

		//=============================================================================================
		Scene* m_Scene = nullptr;
		Entity m_Entity = NullEntity;

	public:

		//=============================================================================================
		GameObject() = default;
		GameObject( Scene* a_Scene, Entity a_Entity ) : m_Scene( a_Scene ), m_Entity( a_Entity ) {}
		GameObject( Scene& a_Scene, Entity a_Entity ) : m_Scene( &a_Scene ), m_Entity( a_Entity ) {}
		~GameObject() = default;

		//=============================================================================================
		operator Entity() const { return m_Entity; }
		operator bool() const { return Valid(); }

		//=============================================================================================
		bool operator==( const GameObject& a_Other ) const
		{
			return m_Scene == a_Other.m_Scene && m_Entity == a_Other.m_Entity;
		}

		//=============================================================================================
		// Returns the underlying Entity of this GameObject.
		[[nodiscard]] Entity Entity() const { return m_Entity; }

		//=============================================================================================
		// Returns a pointer to the Scene that owns this GameObject.
		[[nodiscard]] Scene* Scene() const { return m_Scene; }

		//=============================================================================================
		// Checks if this GameObject is valid and usable.
		[[nodiscard]] bool Valid() const
		{
			return m_Scene && m_Entity != NullEntity && m_Scene->Registry().Valid( m_Entity );
		}

		//=============================================================================================
		void Destroy()
		{
			if ( Valid() )
			{
				m_Scene->DestroyGameObject( *this );
				m_Entity = NullEntity;
				m_Scene = nullptr;
			}
		}

		//=============================================================================================
		// Constructs a new component of type T with the passed in arguments,
		// adds it to the GameObject and returns a reference to it.
		// This will assert if the component already exists.
		template<typename T, typename... _Args>
		T& Add( _Args&&... a_Args )
		{
			return m_Scene->Registry().Emplace<T>( m_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get() const
		{
			return m_Scene->Registry().Get<T...>( m_Entity );
		}

		//=============================================================================================
		// Checks if the GameObject has the component,
		// if not it creates a new component with the passed in arguments and returns a reference to it.
		// If the component already exists, it returns a reference to the existing component.
		template<typename T, typename... _Args>
		T& GetOrAdd( _Args&&... a_Args )
		{
			return m_Scene->Registry().GetOrEmplace<T>( m_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns pointers to the components of types T...
		template<typename... T>
		[[nodiscard]] auto TryGet() const -> decltype( m_Scene->Registry().TryGet<T...>( m_Entity ) )
		{
			if ( Valid() )
			{
				return m_Scene->Registry().TryGet<T...>( m_Entity );
			}

			return {};
		}

		//=============================================================================================
		// Checks if the GameObject has the specified components.
		template<typename T>
		[[nodiscard]] bool Has() const
		{
			return m_Scene->Registry().AnyOf<T>( m_Entity );
		}

		//=============================================================================================
		// Checks if the GameObject has any of the specified components.
		template<typename... T>
		[[nodiscard]] bool HasAny() const
		{
			return m_Scene->Registry().AnyOf<T...>( m_Entity );
		}

		//=============================================================================================
		// Checks if the GameObject has all of the specified components.
		template<typename... T>
		[[nodiscard]] bool HasAll() const
		{
			return m_Scene->Registry().AllOf<T...>( m_Entity );
		}

		//=============================================================================================
		// Removes the specified components from the GameObject.
		template<typename... T>
		void Remove()
		{
			m_Scene->Registry().Remove<T...>( m_Entity );
		}

	};

} // namespace Tridium