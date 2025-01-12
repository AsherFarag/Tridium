#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Physics/PhysicsBody.h>
#include <Tridium/ECS/ECS.h>

namespace Tridium {

	struct RayCastResult
	{
		bool Hit; // Did the ray hit anything?
		Vector3 Position; // The position of the hit.
		Vector3 Normal; // The normal of the hit.
		float Distance; // The distance from the ray start to the hit.
		Vector3 RayStart; // The start of the ray.
		Vector3 RayEnd; // The end of the ray.
		PhysicsBodyID HitBodyID; // The ID of the body that was hit.
		EntityID HitGameObject; // The GameObject that was hit.
	};

} // namespace Tridium