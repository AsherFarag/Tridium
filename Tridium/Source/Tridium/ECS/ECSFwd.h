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

} // namespace Tridium