#pragma once
#include "Math.h"

namespace Tridium {

	// 3D Axis Aligned Bounding Box
	struct AABBOld
	{
		Vector3 Min{0.0f};
		Vector3 Max{0.0f};

		static constexpr AABBOld MaxAABB()
		{
			return AABBOld( Vector3( Math::NumericLimits<float>::min() ),
					     Vector3( Math::NumericLimits<float>::max() ) );
		}

		AABBOld Transform( const Matrix4& a_Transform ) const
		{
			Vector3 corners[8] = {
				Vector3( Min.X, Min.Y, Min.Z ),
				Vector3( Min.X, Min.Y, Max.Z ),
				Vector3( Min.X, Max.Y, Min.Z ),
				Vector3( Min.X, Max.Y, Max.Z ),
				Vector3( Max.X, Min.Y, Min.Z ),
				Vector3( Max.X, Min.Y, Max.Z ),
				Vector3( Max.X, Max.Y, Min.Z ),
				Vector3( Max.X, Max.Y, Max.Z )
			};

			Vector3 transformedCorner = a_Transform * Vector4( corners[0], 1.0f );
			Vector3 min = transformedCorner;
			Vector3 max = transformedCorner;

			for ( uint32_t i = 1; i < 8; ++i )
			{
				transformedCorner = a_Transform * Vector4( corners[i], 1.0f );
				min = Math::Min( min, transformedCorner );
				max = Math::Max( max, transformedCorner );
			}

			return { min, max };
		}

		bool Intersects( const AABBOld& a_Other ) const
		{
			return ( Min.X <= a_Other.Max.X && Max.X >= a_Other.Min.X ) &&
				   ( Min.Y <= a_Other.Max.Y && Max.Y >= a_Other.Min.Y ) &&
				   ( Min.Z <= a_Other.Max.Z && Max.Z >= a_Other.Min.Z );
		}

		bool Contains( const Vector3& a_Point ) const
		{
			return ( a_Point.X >= Min.X && a_Point.X <= Max.X ) &&
				   ( a_Point.Y >= Min.Y && a_Point.Y <= Max.Y ) &&
				   ( a_Point.Z >= Min.Z && a_Point.Z <= Max.Z );
		}

		void Expand( const Vector3& a_Point )
		{
			Min = Vector3( Min.X < a_Point.X ? Min.X : a_Point.X,
				           Min.Y < a_Point.Y ? Min.Y : a_Point.Y,
				           Min.Z < a_Point.Z ? Min.Z : a_Point.Z );

			Max = Vector3( Max.X > a_Point.X ? Max.X : a_Point.X,
						   Max.Y > a_Point.Y ? Max.Y : a_Point.Y,
						   Max.Z > a_Point.Z ? Max.Z : a_Point.Z );
		}

		void Expand( const AABBOld& a_Other )
		{
			Min = Vector3( Min.X < a_Other.Min.X ? Min.X : a_Other.Min.X,
				           Min.Y < a_Other.Min.Y ? Min.Y : a_Other.Min.Y,
				           Min.Z < a_Other.Min.Z ? Min.Z : a_Other.Min.Z );

			Max = Vector3( Max.X > a_Other.Max.X ? Max.X : a_Other.Max.X,
						   Max.Y > a_Other.Max.Y ? Max.Y : a_Other.Max.Y,
						   Max.Z > a_Other.Max.Z ? Max.Z : a_Other.Max.Z );
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

		void Translate( const Vector3& a_Translation )
		{
			Min += a_Translation;
			Max += a_Translation;
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
		Vector2 Min{ 0.0f };
		Vector2 Max{ 0.0f };

		static constexpr Rect MaxRect()
		{
			return Rect( Vector2( -std::numeric_limits<float>::max() ),
						 Vector2( std::numeric_limits<float>::max() ) );
		}

		bool Intersects( const Rect& a_Other ) const
		{
			return ( Min.X <= a_Other.Max.X && Max.X >= a_Other.Min.X ) &&
				   ( Min.Y <= a_Other.Max.Y && Max.Y >= a_Other.Min.Y );
		}

		bool Contains( const Vector2& a_Point ) const
		{
			return ( a_Point.X >= Min.X && a_Point.X <= Max.X ) &&
				   ( a_Point.Y >= Min.Y && a_Point.Y <= Max.Y );
		}

		void Expand( const Vector2& a_Point )
		{
			Min = Vector2( Min.X < a_Point.X ? Min.X : a_Point.X,
						   Min.Y < a_Point.Y ? Min.Y : a_Point.Y );

			Max = Vector2( Max.X > a_Point.X ? Max.X : a_Point.X,
						   Max.Y > a_Point.Y ? Max.Y : a_Point.Y );
		}

		void Expand( const Rect& a_Other )
		{
			Min = Vector2( Min.X < a_Other.Min.X ? Min.X : a_Other.Min.X,
						   Min.Y < a_Other.Min.Y ? Min.Y : a_Other.Min.Y );

			Max = Vector2( Max.X > a_Other.Max.X ? Max.X : a_Other.Max.X,
						   Max.Y > a_Other.Max.Y ? Max.Y : a_Other.Max.Y );
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