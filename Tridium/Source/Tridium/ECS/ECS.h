#pragma once
#include "entt.hpp"
#include <Tridium/Core/Assert.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	using EntityIDType = uint32_t;
	using EntityID = entt::entity;
	constexpr EntityID NullEntity = entt::null;

	//================================================================
	// Entity Component System (ECS)
	//  The ECS is a system for managing entities and their components.
	//  This class is a wrapper around the EnTT registry.
	//  Use this class for creating custom entity systems.
	//================================================================
	class EntityComponentSystem
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
			ASSERT_LOG( !m_Registry.any_of<T>( a_Entity ), "Entity already has this component!" );
			return m_Registry.emplace<T>( a_Entity, std::forward<Args>( args )... );
		}

		template <typename T>
		T& GetComponentFromEntity( EntityID a_Entity )
		{
			ENSURE_LOG( m_Registry.any_of<T>( a_Entity ), "Entity does not have this component!" );
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
			ASSERT_LOG( m_Registry.any_of<T>( a_Entity ), "Entity does not have this component!" );
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

}