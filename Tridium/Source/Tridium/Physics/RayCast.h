#pragma once
#include <Tridium/Math/Math.h>

namespace Tridium {

	class GameObject;

	struct RayCastResult
	{
		bool Hit; // Did the ray hit anything?
		Vector3 Position; // The position of the hit
		Vector3 Normal; // The normal of the hit
		float Distance; // The distance from the ray start to the hit
		Vector3 RayStart; // The start of the ray
		Vector3 RayEnd; // The end of the ray
		//GameObject HitGameObject; // The game object that was hit. Can be null if no game object was hit
	};

} // namespace Tridium