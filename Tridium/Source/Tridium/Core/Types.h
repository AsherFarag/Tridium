#pragma once
#include <stdint.h>

namespace Tridium {

	using Byte = unsigned char;
	using GameObjectID = uint32_t;
	constexpr GameObjectID INVALID_GAMEOBJECT_ID = 0xFFFFFFFF;
}