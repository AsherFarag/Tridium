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

	struct OldFrustum
	{
		Plane Top;
		Plane Bottom;

		Plane Right;
		Plane Left;

		Plane Far;
		Plane Near;

		bool Intersects( const AABBOld& a_AABB ) const
		{
			const Vector3& min = a_AABB.Min;
			const Vector3& max = a_AABB.Max;

			for ( const Plane& plane : { Top, Bottom, Right, Left, Far, Near } ) 
			{
				Vector3 positive = {
					plane.Normal.X > 0 ? max.X : min.X,
					plane.Normal.Y > 0 ? max.Y : min.Y,
					plane.Normal.Z > 0 ? max.Z : min.Z
				};

				// If the extreme point is outside the plane, the AABB is not intersecting
				if ( ( Math::Dot( positive, plane.Normal ) + plane.Distance ) < 0.0f )
					return false;
			}

			return true;
		}
	};

}