#pragma once
#include <Tridium/Core/Assert.h>
#include <Tridium/ECS/ECSFwd.h>
#include <Tridium/Utils/Concepts.h>

#include <entt/entity/registry.hpp>

namespace Tridium {

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
				T* instance = ReinterpretCast<T*>( this );
				auto& registry = ReinterpretCast<class EntityComponentRegistry&>( a_Registry );
				EntityID entity = ReinterpretCast<EntityID>( a_Entity );
				( instance->*_Method )( registry, entity );
			}

			//void Invoke( entt::registry& a_Registry, entt::entity a_Entity ) const
			//{
			//	const T* instance = ReinterpretCast<T*>( this );
			//	auto& registry = ReinterpretCast<class EntityComponentRegistry&>( a_Registry );
			//	EntityID entity = ReinterpretCast<EntityID>( a_Entity );
			//	( instance->*_Method )( registry, entity );
			//}

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
			return Cast<bool>( m_Connection );
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
						_Func( ReinterpretCast<EntityComponentRegistry&>( a_Registry ), Cast<EntityID>( a_Entity ) );
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
						_Func( ReinterpretCast<EntityComponentRegistry&>( a_Registry ), Cast<EntityID>( a_Entity ) );
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

	private:

		//=============================================================================================
		entt::registry m_Registry;

	};

} // namespace Tridium