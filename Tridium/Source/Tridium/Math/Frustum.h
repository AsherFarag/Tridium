#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Math/AABB.h>

namespace Tridium {

	struct Plane
	{
		// Unit normal vector
		Vector3 Normal;
		// Distance from the origin.
		float Distance;
	};

	struct Frustum
	{
		Plane Top;
		Plane Bottom;

		Plane Right;
		Plane Left;

		Plane Far;
		Plane Near;

		bool Intersects( const AABB& a_AABB ) const
		{
			const Vector3& min = a_AABB.Min;
			const Vector3& max = a_AABB.Max;

			for ( const Plane& plane : { Top, Bottom, Right, Left, Far, Near } ) {
				Vector3 positive = {
					plane.Normal.x > 0 ? max.x : min.x,
					plane.Normal.y > 0 ? max.y : min.y,
					plane.Normal.z > 0 ? max.z : min.z
				};

				// If the extreme point is outside the plane, the AABB is not intersecting
				if ( glm::dot( positive, plane.Normal ) + plane.Distance < 0.0f ) {
					return false;
				}
			}

			return true;
		}
	};

}