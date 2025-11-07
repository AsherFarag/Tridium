#pragma once
#include <Tridium/ECS/Entity.h>

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

namespace Tridium {

	//=================================================================================================
	// Entity View: A wrapper around an EnTT view to provide additional functionality.
	// Views are a lightweight, non-owning way to iterate over entities with specific components.
	// Single type views are as fast as can be, as they just iterate over the sparse set of that component.
	// Multi-type views iterate over the smallest component's sparse set 
	// and check for the presence of other components.
	// Consider using groups for better performance with multiple component types.
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
		[[nodiscard]] auto RBegin() noexcept { return m_View.rbegin(); }
		[[nodiscard]] auto REnd() noexcept { return m_View.rend(); }
		[[nodiscard]] auto RBegin() const noexcept { return m_View.rbegin(); }
		[[nodiscard]] auto REnd() const noexcept { return m_View.rend(); }

		//=============================================================================================
		// Access component(s) of type T... for the specified entity.
		template<typename... T>
		decltype( auto ) Get( const Entity a_Entity ) const
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
	// Entity Group: A wrapper around an EnTT group to provide additional functionality.
	// A group is a tightly packed, optionally owning view of components 
	// that improves iteration speed and data locality.
	// Owning groups reorganize the storage so that the components are tightly packed together.
	// Non-owning groups do not modify storage layout but still provide fast iteration when iterating
	// the same combination of components frequently.
	// Example:
	// auto group = registry.Group<Transform>(EntityGet<RigidBody, Collider>);
	// 
	// - Transform becomes the owning component.
	// - The registry ensures that all entities that have (Transform, RigidBody, Collider)
	//   are packed together inside the Transform storage.
	// - The RigidBody and Collider components are not moved in their own storages,
	//   the registry just maintains matching indices so iteration is contiguous.
	// 
	// Entity layout example:
	// Transform storage:
	// [Entity1][Entity2][Entity3] <- tightly packed group entities
	// [Entity4]                   <- Transform-only (outside the group)
	// 
	// RigidBody storage:
	// [Entity1][Entity3][Entity2] <- may be in different order
	// 
	// Collider storage:
	// [Entity3][Entity1][Entity2]
	// 
	//=================================================================================================
	template<typename _UnderlyingType>
	class EntityGroup
	{
	public:

		//=============================================================================================
		using UnderlyingType = _UnderlyingType;

		//=============================================================================================
		EntityGroup() = default;
		explicit EntityGroup( UnderlyingType&& a_Group ) : m_Group( std::move( a_Group ) ) {}

		//=============================================================================================
		// Iterator support
		[[nodiscard]] auto Begin() noexcept { return m_Group.begin(); }
		[[nodiscard]] auto End() noexcept { return m_Group.end(); }
		[[nodiscard]] auto Begin() const noexcept { return m_Group.begin(); }
		[[nodiscard]] auto End() const noexcept { return m_Group.end(); }
		[[nodiscard]] auto RBegin() noexcept { return m_Group.rbegin(); }
		[[nodiscard]] auto REnd() noexcept { return m_Group.rend(); }
		[[nodiscard]] auto RBegin() const noexcept { return m_Group.rbegin(); }
		[[nodiscard]] auto REnd() const noexcept { return m_Group.rend(); }

		//=============================================================================================
		// Access component(s) of type T... for the specified entity.
		template<typename... T>
		decltype( auto ) Get( const Entity a_Entity ) const
		{
			return m_Group.template get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns an iterable object to iterate over each entity and its components.
		// E.g. , for ( auto [ entity, comp1, comp2 ] : view.Each() ) { ... }
		[[nodiscard]] decltype( auto ) Each() const noexcept
		{
			return m_Group.each();
		}

		//=============================================================================================
		// Applies the provided function to each entity and its components.
		// E.g., view.Each( []( auto entity, auto& comp1, auto& comp2 ) { ... } );
		template<typename _Func>
		void Each( _Func&& a_Func ) const
		{
			m_Group.each( std::forward<_Func>( a_Func ) );
		}

		//=============================================================================================
		UnderlyingType& Underlying() { return m_Group; }
		const UnderlyingType& Underlying() const { return m_Group; }

	private:

		//=============================================================================================
		UnderlyingType m_Group;

	};

	//=============================================================================================
	// begin/end for EntityGroup
	template<typename _UnderlyingType> auto begin( EntityGroup<_UnderlyingType>& a_Group ) { return a_Group.Begin(); }
	template<typename _UnderlyingType> auto end( EntityGroup<_UnderlyingType>& a_Group ) { return a_Group.End(); }
	template<typename _UnderlyingType> auto begin( const EntityGroup<_UnderlyingType>& a_Group ) { return a_Group.Begin(); }
	template<typename _UnderlyingType> auto end( const EntityGroup<_UnderlyingType>& a_Group ) { return a_Group.End(); }

	//=================================================================================================
	// Entity Event Handle: A scoped handle which manages the lifetime of an entity event conntection.
	//=================================================================================================
	class EntityEventHandle
	{
	public:

		//=============================================================================================
		// Helper struct for function aliasing.
		// For internal use only.
		template<typename T, auto _Method>
		struct alignas( std::remove_cvref_t<T> ) Wrapper
		{
			void Invoke( entt::registry& a_Registry, entt::entity a_Entity )
			{
				T* instance = reinterpret_cast<T*>( this );
				auto& registry = reinterpret_cast<class EntityComponentRegistry&>( a_Registry );
				Entity entity = a_Entity;
				( instance->*_Method )( registry, entity );
			}
		};


		//=============================================================================================
		EntityEventHandle() = default;
		explicit EntityEventHandle( entt::scoped_connection&& a_Connection ) : m_Connection( std::move( a_Connection ) ) {}


		//=============================================================================================
		operator bool() const
		{
			return Valid();
		}

		//=============================================================================================
		bool Valid() const
		{
			return static_cast<bool>( m_Connection );
		}

		//=============================================================================================
		void Release()
		{
			m_Connection.release();
		}

	private:

		//=============================================================================================
		entt::scoped_connection m_Connection;

	};

	//=================================================================================================
	// Entity Include Type: A type list representing components to include in views or groups.
	template<typename... _Include>
	using EntityIncludeType = entt::type_list<_Include...>;
	template<typename... _Include>
	static constexpr EntityIncludeType<_Include...> EntityInclude{};

	//=================================================================================================
	template<typename... _Exclude>
	using EntityExcludeType = entt::exclude_t<_Exclude...>;
	template<typename... _Exclude>
	static constexpr EntityExcludeType<_Exclude...> EntityExclude{};

	template<typename T>
	class ComponentStorage : public T
	{
	public:
		using allocator_type = typename T::allocator_type;
		using element_type = typename T::element_type;

		explicit ComponentStorage( const allocator_type& a_Allocator ) : T( a_Allocator )
		{
			using namespace entt::literals;
			entt::meta_factory<element_type>{}
			// cross registry, same type
			.template func<entt::overload<entt::storage_for_t<element_type, entt::entity>& ( const entt::id_type )>( &entt::basic_registry<entt::entity>::storage<element_type> ), entt::as_ref_t>( "storage"_hs );
		}
	};

	template<typename T, typename _Entity>
	struct entt::storage_type<T, _Entity>
	{
		using type = sigh_mixin<Tridium::ComponentStorage<basic_storage<T, _Entity>>>;
	};

	//=================================================================================================
	// Entity Component Registry (ECR): Stores and manages entities and their components,
	// in an Entity Component System (ECS) architecture.
	//=================================================================================================
	class EntityComponentRegistry
	{
	public:

		//=============================================================================================
		using UnderlyingType = entt::registry;

		//=============================================================================================
		// Checks if the entity is valid (i.e., currently in use).
		[[nodiscard]] bool Valid( const Entity a_Entity ) const
		{
			return m_Registry.valid( a_Entity );
		}

		//=============================================================================================
		// Creates a new entity or recycles an old one and returns its ID.
		Entity Create()
		{
			return m_Registry.create();
		}

		//=============================================================================================
		// If the requested entity isn't in use, the suggested identifier is used.
		// Otherwise, a new identifier is generated.
		Entity Create( const Entity a_Hint )
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
		EntityVersion Destroy( const Entity a_Entity )
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
		decltype( auto ) Emplace( const Entity a_Entity, _Args&&... a_Args )
		{
			return m_Registry.emplace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Adds or replaces a component of type T on the specified entity with the given arguments.
		// Returns a reference to the added or replaced component.
		template<typename T, typename... _Args>
		decltype( auto ) EmplaceOrReplace( const Entity a_Entity, _Args&&... a_Args )
		{
			return m_Registry.emplace_or_replace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Removes components of types T... from the specified entity.
		// Returns the number of components removed.
		template<typename... T>
		size_t Remove( const Entity a_Entity )
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
		[[nodiscard]] bool AllOf( const Entity a_Entity ) const
		{
			return m_Registry.all_of<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns true if the specified entity has any component of types T...
		template<typename... T>
		[[nodiscard]] bool AnyOf( const Entity a_Entity ) const
		{
			return m_Registry.any_of<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get( const Entity a_Entity )
		{
			return m_Registry.get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns const references to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) Get( const Entity a_Entity ) const
		{
			return m_Registry.get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns a reference to the component of type T associated with the specified entity,
		// or creates and adds it if it doesn't exist.
		template<typename T, typename... _Args>
		[[nodiscard]] decltype( auto ) GetOrEmplace( const Entity a_Entity, _Args&&... a_Args )
		{
			return m_Registry.get_or_emplace<T>( a_Entity, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns pointers to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) TryGet( const Entity a_Entity )
		{
			return m_Registry.try_get<T...>( a_Entity );
		}

		//=============================================================================================
		// Returns const pointers to the components of types T... associated with the specified entity.
		template<typename... T>
		[[nodiscard]] decltype( auto ) TryGet( const Entity a_Entity ) const
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
		// Returns a const view of entities with components of types T...,
		// excluding those with components of types _Exclude...
		template<typename T, typename... _Other, typename... _Exclude>
		[[nodiscard]] auto View( EntityExcludeType<_Exclude...> = EntityExcludeType{} ) const
		{
			return EntityView{ m_Registry.view<T, _Other...>( entt::exclude<_Exclude...> ) };
		}

		//=============================================================================================
		template<typename... _Owning, typename... _Include, typename... _Exclude>
		[[nodiscard]] auto Group( EntityIncludeType<_Include...> = EntityIncludeType{}, EntityExcludeType<_Exclude...> = EntityExcludeType{} )
		{
			return EntityGroup{ m_Registry.group<_Owning...>( entt::get<_Include...>, entt::exclude<_Exclude...> ) };
		}

		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to 
		// component construction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Func>
		[[nodiscard]] EntityEventHandle OnConstruct()
		{
			// Wrap the user's callback into EnTT's expected signature.
			return EntityEventHandle{ m_Registry
				.template on_construct<_Component>()
				.connect(
					[]( auto& a_Registry, auto a_Entity )
					{
						_Func( ReinterpretCast<EntityComponentRegistry&>( a_Registry ), Cast<Entity>( a_Entity ) );
					} ) };
		}

		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to
		// component construction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Method, typename T>
		[[nodiscard]] EntityEventHandle OnConstruct( T& a_Instance )
		{
			using Wrapper = EntityEventHandle::Wrapper<T, _Method>;

			return EntityEventHandle{ m_Registry
				.template on_construct<_Component>()
				.template connect<&Wrapper::Invoke>( ReinterpretCast<Wrapper&>( a_Instance ) ) };
		}


		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to
		// component construction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Method, typename T>
		[[nodiscard]] EntityEventHandle OnConstruct( T* a_Instance )
		{
			using Wrapper = EntityEventHandle::Wrapper<T, _Method>;

			return EntityEventHandle{ m_Registry
				.template on_construct<_Component>()
				.template connect<&Wrapper::Invoke>( ReinterpretCast<Wrapper*>( a_Instance ) ) };
		}

		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to
		// component destruction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Func>
		[[nodiscard]] EntityEventHandle OnDestruct()
		{
			// Wrap the user's callback into EnTT's expected signature.
			return EntityEventHandle{ m_Registry
				.template on_destroy<_Component>()
				.connect(
					[]( auto& a_Registry, auto a_Entity )
					{
						_Func( ReinterpretCast<EntityComponentRegistry&>( a_Registry ), Cast<Entity>( a_Entity ) );
					} ) };
		}

		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to
		// component destruction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Method, typename T>
		[[nodiscard]] EntityEventHandle OnDestruct( T& a_Instance )
		{
			using Wrapper = EntityEventHandle::Wrapper<T, _Method>;

			return EntityEventHandle{ m_Registry
				.template on_destroy<_Component>()
				.template connect<&Wrapper::Invoke>( ReinterpretCast<Wrapper&>( a_Instance ) ) };
		}


		//=============================================================================================
		// Returns a Scoped Delegate Handle object for listening to
		// component destruction events of type _Component.
		// NOTE: The returned handle must be kept alive to maintain the connection.
		template<typename _Component, auto _Method, typename T>
		[[nodiscard]] EntityEventHandle OnDestruct( T* a_Instance )
		{
			using Wrapper = EntityEventHandle::Wrapper<T, _Method>;

			return EntityEventHandle{ m_Registry
				.template on_destroy<_Component>()
				.template connect<&Wrapper::Invoke>( ReinterpretCast<Wrapper*>( a_Instance ) ) };
		}

		//=============================================================================================
		// Access the underlying EnTT registry.
		auto& Underlying() { return m_Registry; }
		const auto& Underlying() const { return m_Registry; }

		//=============================================================================================
		// Returns an iterable view of all storages in the registry.
		auto Storage() { return m_Registry.storage(); }
		auto Storage() const { return m_Registry.storage(); }

		//=============================================================================================
		auto Storage( const entt::id_type a_TypeID ) { return m_Registry.storage( a_TypeID ); }
		auto Storage( const entt::id_type a_TypeID ) const { return m_Registry.storage( a_TypeID ); }

		//=============================================================================================
		EntityComponentRegistry Clone() const
		{
			EntityComponentRegistry dstRegistry;

			// Copy entities
			auto entities = View<Entity>();
			for ( auto it = entities.RBegin(); it != entities.REnd(); ++it )
			{
				dstRegistry.Create( *it );
			}

			for ( const auto& [id, srcStorage] : Storage() )
			{
				auto dstStorage = dstRegistry.Storage( id );

				if ( !dstStorage )
				{
					using namespace entt::literals;
					entt::resolve( srcStorage.info() ).invoke( "storage"_hs, {}, entt::forward_as_meta( dstRegistry.Underlying() ), id );
					dstStorage = dstRegistry.Storage( id );
				}

				// Copy components
				for ( const auto entity : srcStorage )
				{
					dstStorage->push( entity, srcStorage.value( entity ) );
				}
			}

			return dstRegistry;
		}


	private:

		//=============================================================================================
		entt::registry m_Registry;

	};

} // namespace Tridium