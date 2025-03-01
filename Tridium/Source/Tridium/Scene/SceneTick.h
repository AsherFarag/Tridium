#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {

	//====================================================================================
	// Tick Group
	//  A tick group is a way to categorize when logic should run in a frame.
	//  The order of the tick groups is as follows:
	//  - PrePhysics
	//  - DuringPhysics
	//  - PostPhysics
	//  - Last
	enum class ETickGroup : uint8_t
	{
		// Runs at the start of a frame, before physics simulation has begun.
		// Useful for things intended to interact with the physics simulation.
		// Note: Physics data, such as velocities, is from the previous frame.
		PrePhysics,

		// Runs at the same time as the physics simulation (If multithreaded physics is enabled).
		// The physics simulation is guaranteed to have begun by this point.
		// Note: Physics data can be in an invalid state during this tick group.
		// Use this tick group only for logic that does not depend on physics data.
		DuringPhysics,

		// Runs after the physics simulation has finished.
		// Useful for physics data to be at it's most up-to-date state.
		PostPhysics,

		// Runs after all other tick groups have completed.
		// Useful for logic that is intended to run after all other logic.
		Last
	};

} // namespace Tridium