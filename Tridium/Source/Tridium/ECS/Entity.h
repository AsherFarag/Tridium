#pragma once
#include <entt/entity/entity.hpp>

//=================================================================================================
// Entity ID Type: The underlying type used for entity indices.
//=================================================================================================
using EntityID = uint32_t;

//=================================================================================================
// Entity Version: The underlying type used for entity versions.
//=================================================================================================
using EntityVersion = uint16_t;

//=================================================================================================
using EntityUnderlyingType = std::underlying_type_t<entt::entity>;

//=================================================================================================
// Entity: A unique identifier for an entity.
// It contains both an index and a version to ensure uniqueness.
//=================================================================================================
using Entity = entt::entity;
constexpr Entity NullEntity = entt::null;

//=================================================================================================
inline constexpr EntityID GetEntityID( const Entity a_Entity )
{
	return entt::entt_traits<Entity>::to_entity( a_Entity );
}

//=================================================================================================
inline constexpr EntityVersion GetEntityVersion( const Entity a_Entity )
{
	return entt::entt_traits<Entity>::to_version( a_Entity );
}