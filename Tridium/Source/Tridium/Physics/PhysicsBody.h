#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {
	using PhysicsBodyID = uint32_t;
	constexpr PhysicsBodyID NullPhysicsBodyID = 0xffffffff;
	constexpr PhysicsBodyID INVALID_PHYSICS_BODY_ID = 0xffffffff;
}