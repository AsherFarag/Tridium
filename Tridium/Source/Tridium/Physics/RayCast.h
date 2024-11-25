#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Physics/PhysicsBody.h>

namespace Tridium {

	// Forward declarations
	class GameObject;
	class RigidBodyComponent;
	// -------------------

	struct RayCastResult
	{
		bool Hit; // Did the ray hit anything?
		Vector3 Position; // The position of the hit
		Vector3 Normal; // The normal of the hit
		float Distance; // The distance from the ray start to the hit
		Vector3 RayStart; // The start of the ray
		Vector3 RayEnd; // The end of the ray
		PhysicsBodyID HitBodyID; // The ID of the body that was hit

		// Helper function to get the GameObject that was hit, 
		// by finding the GameObject with the matching PhysicsBodyID
		GameObject GetHitGameObject() const;
		// Helper function to get the RigidBodyComponent that was hit,
		// by finding the RigidBodyComponent with the matching PhysicsBodyID
		RigidBodyComponent* GetHitRigidBody() const;
	};

} // namespace Tridium