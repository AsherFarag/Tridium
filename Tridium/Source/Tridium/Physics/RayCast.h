#pragma once
#include <Tridium/ECS/ECS.h>
#include <Tridium/Debug/DebugDrawer.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Physics/PhysicsBody.h>
#include <Tridium/Physics/PhysicsFilter.h>
#include <Tridium/Physics/PhysicsLayer.h>

namespace Tridium {

	struct RayCastParams
	{
		const PhysicsBodyFilter& BodyFilter{};
		ERayCastChannel Channel;
		bool DrawDebug = false;

		struct
		{
			Debug::EDrawDuration DrawDurationType = Debug::EDrawDuration::OneFrame;
			float DrawDuration = 0.0f;
			Color4 LineColor = Color4::Red();
			Color4 HitColor = Color4::Green();
		} Debug;
	};

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