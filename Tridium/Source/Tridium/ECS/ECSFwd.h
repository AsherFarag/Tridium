#pragma once
#include <Tridium/Core/Types.h>
#include <entt/entity/entity.hpp>

namespace Tridium {

	using EntityIDType = uint32_t;
	using EntityID = entt::entity;
	using EntityVersion = entt::entt_traits<EntityID>::version_type;
	constexpr EntityID NullEntity = entt::null;

	template<typename... _Exclude>
	using EntityExcludeType = entt::exclude_t<_Exclude...>;
	template<typename... _Exclude>
	static constexpr EntityExcludeType<_Exclude...> EntityExclude{};

	inline constexpr EntityIDType ToEntityID( EntityID a_Entity ) noexcept
	{
		return static_cast<EntityIDType>( entt::to_entity( a_Entity ) );
	}

	inline constexpr EntityVersion ToEntityVersion( EntityID a_Entity ) noexcept
	{
		return entt::entt_traits<EntityID>::to_version( a_Entity );
	}

} // namespace Tridium