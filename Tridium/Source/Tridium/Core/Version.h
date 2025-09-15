#pragma once
#include <Tridium/Core/Types.h>

#define TRIDIUM_MAKE_VERSION(major, minor, patch) ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define TRIDIUM_VERSION_MAJOR 1
#define TRIDIUM_VERSION_MINOR 0
#define TRIDIUM_VERSION_PATCH 0

#define TRIDIUM_VERSION TRIDIUM_MAKE_VERSION( TRIDIUM_VERSION_MAJOR, TRIDIUM_VERSION_MINOR, TRIDIUM_VERSION_PATCH )

namespace Tridium {

	using VersionID = uint32_t;
	inline constexpr VersionID UnknownVersionID = ( VersionID )-1;

}