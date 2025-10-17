#pragma once
#include <entt/entity/registry.hpp>
#include <Tridium/Core/Assert.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	using EntityIDType = uint32_t;
	using EntityID = entt::entity;
	using EntityVersion = entt::entt_traits<EntityID>::version_type;
	constexpr EntityID NullEntity = entt::null;

	template<typename... _Exclude>
	using EntityExcludeType = entt::exclude_t<_Exclude...>;
	template<typename... _Exclude>
	static constexpr EntityExcludeType<_Exclude...> EntityExclude{};

	//=================================================================================================
	// Entity View: A wrapper around an EnTT view to provide additional functionality.
	//=================================================================================================
	template<typename _UnderlyingType>
	class EntityView
	{
	public:

		//=============================================================================================
		using UnderlyingType = _UnderlyingType;

		//=============================================================================================
		EntityView() = default;
		explicit EntityView( UnderlyingType&& a_View ) : m_View( std::move( a_View ) ) {}

		//=============================================================================================
		// Iterator support
		[[nodiscard]] auto Begin() noexcept { return m_View.begin(); }
		[[nodiscard]] auto End() noexcept { return m_View.end(); }
		[[nodiscard]] auto Begin() const noexcept { return m_View.begin(); }
		[[nodiscard]] auto End() const noexcept { return m_View.end(); }

		//=============================================================================================
		// Access component(s) of type T... for the specified entity.
		template<typename... T>
		decltype( auto ) Get( const EntityID a_Entity ) const
		{
			return m_View.template get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns an iterable object to iterate over each entity and its components.
		// E.g. , for ( auto [ entity, comp1, comp2 ] : view.Each() ) { ... }
		[[nodiscard]] decltype( auto ) Each() const noexcept
		{
			return m_View.each();
		}

		//=============================================================================================
		// Applies the provided function to each entity and its components.
		// E.g., view.Each( []( auto entity, auto& comp1, auto& comp2 ) { ... } );
		template<typename _Func>
		void Each( _Func&& a_Func ) const
		{
			m_View.each( std::forward<_Func>( a_Func ) );
		}

		//=============================================================================================
		UnderlyingType& Underlying() { return m_View; }
		const UnderlyingType& Underlying() const { return m_View; }

	private:

		//=============================================================================================
		UnderlyingType m_View;

	};

	//=============================================================================================
	// begin/end for EntityView
	template<typename _UnderlyingType> auto begin( EntityView<_UnderlyingType>& a_View ) { return a_View.Begin(); }
	template<typename _UnderlyingType> auto end( EntityView<_UnderlyingType>& a_View ) { return a_View.End(); }
	template<typename _UnderlyingType> auto begin( const EntityView<_UnderlyingType>& a_View ) { return a_View.Begin(); }
	template<typename _UnderlyingType> auto end( const EntityView<_UnderlyingType>& a_View ) { return a_View.End(); }

	//=================================================================================================
	// Entity Component Registry (ECR): Stores and manages entities and their components,
	// in an Entity Component System (ECS) architecture.
	//=================================================================================================
	class EntityComponentRegistry
	{
	public:

		//=============================================================================================
		// Checks if the entity is valid (i.e., currently in use).
		[[nodiscard]] bool Valid( const EntityID a_Entity ) const
		{
			return m_Registry.valid( a_Entity );
		}

		//=============================================================================================
		// Creates a new entity or recycles an old one and returns its ID.
		[[nodiscard]] EntityID Create()
		{
			return m_Registry.create();
		}

		//=============================================================================================
		// If the requested entity isn't in use, the suggested identifier is used.
		// Otherwise, a new identifier is generated.
		[[nodiscard]] EntityID Create( const EntityID a_Hint )
		{
			return m_Registry.create( a_Hint );
		}

		//=============================================================================================
		// Creates and assigns entity IDs in the range [a_Begin, a_End).
		template<typename _Iterator>
		void Create( _Iterator a_Begin, _Iterator a_End )
		{
			m_Registry.create( a_Begin, a_End );
		}

		//=============================================================================================
		// Destroys the specified entity and removes all its components.
		// Returns the version of the destroyed entity.
		EntityVersion Destroy( const EntityID a_Entity )
		{
			return m_Registry.destroy( a_Entity );
		}

		//=============================================================================================
		// Destroys all entities in the range [a_Begin, a_End) and removes all their components.
		template<typename _Iterator>
		void Destroy( _Iterator a_Begin, _Iterator a_End )
		{
			m_Registry.destroy( a_Begin, a_End );
		}

		//=============================================================================================
		// Adds a component of type T to the specified entity with the given arguments.
		// Returns a reference to the newly added component.
		template<typename T, typename... _Args>
		decltype( auto ) Emplace( const EntityID a_Entity, _Args&&... a_Args )
		{
			return m_Registry.emplace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Adds or replaces a component of type T on the specified entity with the given arguments.
		// Returns a reference to the added or replaced component.
		template<typename T, typename... _Args>
		decltype( auto ) EmplaceOrReplace( const EntityID a_Entity, _Args&&... a_Args )
		{
			return m_Registry.emplace_or_replace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Removes components of types T... from the specified entity.
		// Returns the number of components removed.
		template<typename... T>
		size_t Remove( const EntityID a_Entity )
		{
			return m_Registry.remove<T...>( a_Entity );
		}

		//=============================================================================================
		// Removes components of types T... from entities in the range [a_Begin, a_End).
		// Returns the number of components removed.
		template<typename... T, typename _Iterator>
		size_t Remove( _Iterator a_Begin, _Iterator a_End )
		{
			return m_Registry.remove<T...>( a_Begin, a_End );
		}

		//=============================================================================================
		// Returns true if the specified entity has all components of types T...
		template<typename... T>
		[[nodiscard]] bool AllOf( const EntityID a_Entity ) const
		{
			return m_Registry.all_of<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns true if the specified entity has any component of types T...
		template<typename... T>
		[[nodiscard]] bool AnyOf( const EntityID a_Entity ) const
		{
			return m_Registry.any_of<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get( const EntityID a_Entity )
		{
			return m_Registry.get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns const references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get( const EntityID a_Entity ) const
		{
			return m_Registry.get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns a reference to the component of type T associated with the specified entity,
		// or creates and adds it if it doesn't exist.
		template<typename T, typename... _Args>
		[[nodiscard]] decltype( auto ) GetOrEmplace( const EntityID a_Entity, _Args&&... a_Args )
		{
			return m_Registry.get_or_emplace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns pointers to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) TryGet( const EntityID a_Entity )
		{
			return m_Registry.try_get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns const pointers to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) TryGet( const EntityID a_Entity ) const
		{
			return m_Registry.try_get<T...>( a_Entity );
		}

		//=============================================================================================
		// Clears the whole registry or specific component storages.
		template<typename... T>
		void Clear()
		{
			m_Registry.clear<T...>();
		}

		//=============================================================================================
		// Returns a view of entities with components of types T...,
		// excluding those with components of types _Exclude...
		template<typename T, typename... _Other, typename... _Exclude>
		[[nodiscard]] auto View( EntityExcludeType<_Exclude...> = EntityExcludeType{} )
		{
			return EntityView{ m_Registry.view<T, _Other...>( entt::exclude<_Exclude...> ) };
		}

		//=============================================================================================
		// Access the underlying EnTT registry.
		auto& Underlying() { return m_Registry; }
		const auto& Underlying() const { return m_Registry; }

		//=============================================================================================
		// Returns an iterable view of all storages in the registry.
		auto Storage() { return m_Registry.storage(); }
		auto Storage() const { return m_Registry.storage(); }

	private:

		//=============================================================================================
		entt::registry m_Registry;

	};

	//=================================================================================================
	// Entity Component System Interface: Base class for interacting with entity component registries.
	//=================================================================================================
	class IEntityComponentSystem
	{

	};

		//================================================================
	// Entity Component System (ECS)
	//  The ECS is a system for managing entities and their components.
	//  This class is a wrapper around the EnTT registry.
	//  Use this class for creating custom entity systems.
	//================================================================
	class OldEntityComponentSystem
	{
	public:
		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		void Clear() { m_Registry.clear(); }

		EntityID CreateEntity()
		{
			return m_Registry.create();
		}

		// If the requested entity isn't in use, the suggested identifier is used.
		// Otherwise, a new identifier is generated.
		EntityID CreateEntity( EntityID a_Hint )
		{
			return m_Registry.create( a_Hint );
		}

		bool IsValidEntity( EntityID a_Entity ) const
		{
			return m_Registry.valid( a_Entity );
		}

		void DestroyEntity( EntityID a_Entity )
		{
			m_Registry.destroy( a_Entity );
		}

		template <typename T, typename... Args>
		T& AddComponentToEntity( EntityID a_Entity, Args&&... args )
		{
			ASSERT( !m_Registry.any_of<T>( a_Entity ), "Entity already has this component!" );
			return m_Registry.emplace<T>( a_Entity, std::forward<Args>( args )... );
		}

		template <typename T>
		T& GetComponentFromEntity( EntityID a_Entity )
		{
			ENSURE( m_Registry.any_of<T>( a_Entity ), "Entity does not have this component!" );
			return m_Registry.get<T>( a_Entity );
		}

		template <typename T>
		T* TryGetComponentFromEntity( EntityID a_Entity )
		{
			return m_Registry.try_get<T>( a_Entity );
		}

		template <typename T>
		bool EntityHasComponent( EntityID a_Entity ) const
		{
			return m_Registry.any_of<T>( a_Entity );
		}

		template <typename T>
		void RemoveComponentFromEntity( EntityID a_Entity )
		{
			ASSERT( m_Registry.any_of<T>( a_Entity ), "Entity does not have this component!" );
			m_Registry.remove<T>( a_Entity );
		}

		template <typename T>
		void TryRemoveComponentFromEntity( EntityID a_Entity )
		{
			if ( m_Registry.any_of<T>( a_Entity ) )
			{
				m_Registry.remove<T>( a_Entity );
			}
		}

		template<typename... _Types>
		auto View()
		{
			return m_Registry.view<_Types...>();
		}

		template<typename... _Types>
		auto View() const
		{
			return m_Registry.view<_Types...>();
		}

		auto Storage() { return m_Registry.storage(); }
		auto Storage() const { return m_Registry.storage(); }

	private:
		entt::registry m_Registry;
	};

} // namespace Tridium