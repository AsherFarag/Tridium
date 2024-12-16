#pragma once
#include "Math.h"

namespace Tridium {

	// 3D Axis Aligned Bounding Box
	struct AABB
	{
		static const AABB s_MaxAABB;
		Vector3 Min{0.0f};
		Vector3 Max{0.0f};

		AABB Transform( const Matrix4& a_Transform ) const
		{
			Vector3 corners[8] = {
				Vector3( Min.x, Min.y, Min.z ),
				Vector3( Min.x, Min.y, Max.z ),
				Vector3( Min.x, Max.y, Min.z ),
				Vector3( Min.x, Max.y, Max.z ),
				Vector3( Max.x, Min.y, Min.z ),
				Vector3( Max.x, Min.y, Max.z ),
				Vector3( Max.x, Max.y, Min.z ),
				Vector3( Max.x, Max.y, Max.z )
			};

			Vector3 transformedCorner = a_Transform * Vector4( corners[0], 1.0f );
			Vector3 min = transformedCorner;
			Vector3 max = transformedCorner;

			for ( uint32_t i = 1; i < 8; ++i )
			{
				transformedCorner = a_Transform * Vector4( corners[i], 1.0f );
				min = glm::min( min, transformedCorner );
				max = glm::max( max, transformedCorner );
			}

			return { min, max };
		}

		bool Intersects( const AABB& a_Other ) const
		{
			return ( Min.x <= a_Other.Max.x && Max.x >= a_Other.Min.x ) &&
				   ( Min.y <= a_Other.Max.y && Max.y >= a_Other.Min.y ) &&
				   ( Min.z <= a_Other.Max.z && Max.z >= a_Other.Min.z );
		}

		bool Contains( const Vector3& a_Point ) const
		{
			return ( a_Point.x >= Min.x && a_Point.x <= Max.x ) &&
				   ( a_Point.y >= Min.y && a_Point.y <= Max.y ) &&
				   ( a_Point.z >= Min.z && a_Point.z <= Max.z );
		}

		void Expand( const Vector3& a_Point )
		{
			Min = Vector3( Min.x < a_Point.x ? Min.x : a_Point.x,
				           Min.y < a_Point.y ? Min.y : a_Point.y,
				           Min.z < a_Point.z ? Min.z : a_Point.z );

			Max = Vector3( Max.x > a_Point.x ? Max.x : a_Point.x,
						   Max.y > a_Point.y ? Max.y : a_Point.y,
						   Max.z > a_Point.z ? Max.z : a_Point.z );
		}

		void Expand( const AABB& a_Other )
		{
			Min = Vector3( Min.x < a_Other.Min.x ? Min.x : a_Other.Min.x,
				           Min.y < a_Other.Min.y ? Min.y : a_Other.Min.y,
				           Min.z < a_Other.Min.z ? Min.z : a_Other.Min.z );

			Max = Vector3( Max.x > a_Other.Max.x ? Max.x : a_Other.Max.x,
						   Max.y > a_Other.Max.y ? Max.y : a_Other.Max.y,
						   Max.z > a_Other.Max.z ? Max.z : a_Other.Max.z );
		}

		void Scale( float a_Scale )
		{
			Vector3 center = GetCenter();
			Vector3 size = GetSize() * a_Scale * 0.5f;
			Min = center - size;
			Max = center + size;
		}

		void Scale( const Vector3& a_Scale )
		{
			Vector3 center = GetCenter();
			Vector3 size = GetSize() * a_Scale * 0.5f;
			Min = center - size;
			Max = center + size;
		}

		Vector3 GetCenter() const
		{
			return ( Min + Max ) * 0.5f;
		}

		Vector3 GetSize() const
		{
			return Max - Min;
		}
	};

	// 2D Axis Aligned Bounding Box
	struct Rect
	{
		static const Rect s_MaxRect;
		Vector2 Min{ 0.0f };
		Vector2 Max{ 0.0f };

		bool Intersects( const Rect& a_Other ) const
		{
			return ( Min.x <= a_Other.Max.x && Max.x >= a_Other.Min.x ) &&
				   ( Min.y <= a_Other.Max.y && Max.y >= a_Other.Min.y );
		}

		bool Contains( const Vector2& a_Point ) const
		{
			return ( a_Point.x >= Min.x && a_Point.x <= Max.x ) &&
				   ( a_Point.y >= Min.y && a_Point.y <= Max.y );
		}

		void Expand( const Vector2& a_Point )
		{
			Min = Vector2( Min.x < a_Point.x ? Min.x : a_Point.x,
						   Min.y < a_Point.y ? Min.y : a_Point.y );

			Max = Vector2( Max.x > a_Point.x ? Max.x : a_Point.x,
						   Max.y > a_Point.y ? Max.y : a_Point.y );
		}

		void Expand( const Rect& a_Other )
		{
			Min = Vector2( Min.x < a_Other.Min.x ? Min.x : a_Other.Min.x,
						   Min.y < a_Other.Min.y ? Min.y : a_Other.Min.y );

			Max = Vector2( Max.x > a_Other.Max.x ? Max.x : a_Other.Max.x,
						   Max.y > a_Other.Max.y ? Max.y : a_Other.Max.y );
		}

		void Scale( float a_Scale )
		{
			Vector2 center = GetCenter();
			Vector2 size = GetSize() * a_Scale * 0.5f;
			Min = center - size;
			Max = center + size;
		}

		void Scale( const Vector2& a_Scale )
		{
			Vector2 center = GetCenter();
			Vector2 size = GetSize() * a_Scale * 0.5f;
			Min = center - size;
			Max = center + size;
		}

		Vector2 GetCenter() const
		{
			return ( Min + Max ) * 0.5f;
		}

		Vector2 GetSize() const
		{
			return Max - Min;
		}
	};

}