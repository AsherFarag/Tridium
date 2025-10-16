#pragma once
#include <Tridium/Containers/Variant.h>
#include <Tridium/Math/Math.h>

namespace Tridium {

	//=============================================================================================
	struct Sphere;
	struct AABB;
	struct OBB;
	struct Frustum;

	//=============================================================================================
	// Sphere: A bounding volume defined by a center point and a radius.
	//=============================================================================================
	struct Sphere
	{
		Vector3 Center{ 0.0f };
		float Radius = 0.0f;

		bool Overlaps( const Sphere& a_Sphere, float a_Epsilon = Math::Epsilon() ) const;

		void Expand( const Sphere& a_Other );
	};

	//=============================================================================================
	// AABB: Axis-Aligned Bounding Box, a bounding volume defined by minimum and maximum points.
	//=============================================================================================
	struct AABB
	{
		Vector3 Min{ 0.0f };
		Vector3 Max{ 0.0f };

		bool Overlaps( const Sphere& a_Sphere, float a_Epsilon = Math::Epsilon() ) const;
		bool Overlaps( const AABB& a_Box, float a_Epsilon = Math::Epsilon() ) const;

		void Expand( const Sphere& a_Sphere );
		void Expand( const AABB& a_Box );
	};

	//=============================================================================================
	// OBB: Oriented Bounding Box, a bounding volume defined by an orientation matrix and half extents.
	//=============================================================================================
	struct OBB
	{
		Matrix4 Orientation{ 1.0f };
		Vector3 HalfExtents{ 0.0f };

		bool Overlaps( const Sphere& a_Sphere, float a_Epsilon = Math::Epsilon() ) const;
		bool Overlaps( const AABB& a_Box, float a_Epsilon = Math::Epsilon() ) const;
		bool Overlaps( const OBB& a_Box, float a_Epsilon = Math::Epsilon() ) const;

		void Expand( const Sphere& a_Sphere );
		void Expand( const AABB& a_Box );
		void Expand( const OBB& a_Box );
	};

	struct Frustum
	{

	};

	//=============================================================================================
	// Bounding Volume: A variant type that can hold any bounding volume type (Sphere, AABB, OBB, etc).
	//=============================================================================================
	struct BoundingVolume
	{
		Variant<Sphere, AABB, OBB> Data;

		template<typename T>
		constexpr bool Is() const noexcept { return std::holds_alternative<T>( Data ); }

		template<typename T>
		constexpr T& As() { return std::get<T>( Data ); }

		template<typename T>
		constexpr const T& As() const { return std::get<T>( Data ); }

		template<typename T> 
		constexpr bool Overlaps( const T& a_Other ) const
		{
			if constexpr ( Concepts::SameAs<T, BoundingVolume> )
			{
				return std::visit( []( const auto& a_Self, const auto& a_Other )
					{
						if constexpr ( requires { a_Self.Overlaps( a_Other ); } )
							return a_Self.Overlaps( a_Other );
						else if constexpr ( requires { a_Other.Overlaps( a_Self ); } )
							return a_Other.Overlaps( a_Self );
						else
							static_assert( std::_Always_false<T>, "Overlap test not implemented for these bounding volume types." );

					}, Data, a_Other.Data
				);
			}

			return std::visit( [&]( const auto& a_Self )
				{
					if constexpr ( requires { a_Self.Overlaps( a_Other ); } )
						return a_Self.Overlaps( a_Other );
					else if constexpr ( requires { a_Other.Overlaps( a_Self ); } )
						return a_Other.Overlaps( a_Self );
					else
						static_assert( std::_Always_false<T>, "Overlap test not implemented for these bounding volume types." );
				}, Data
			);
		}

		template<typename T>
		constexpr void Expand( const T& a_Other )
		{
			if constexpr ( Concepts::SameAs<T, BoundingVolume> )
			{
				std::visit( [&]( auto& a_Self, const auto& a_Other )
					{
						if constexpr ( requires { a_Self.Expand( a_Other ); } )
							a_Self.Expand( a_Other );
						else if constexpr ( requires { a_Other.Expand( a_Self ); } )
							a_Self = a_Other, a_Self.Expand( a_Self );
						else
							static_assert( std::_Always_false<T>, "Expand not implemented for these bounding volume types." );
					}, Data, a_Other.Data
				);
			}
			else
			{
				std::visit( [&]( auto& a_Self )
					{
						if constexpr ( requires { a_Self.Expand( a_Other ); } )
							a_Self.Expand( a_Other );
						else if constexpr ( requires { a_Other.Expand( a_Self ); } )
							a_Self = a_Other, a_Self.Expand( a_Self );
						else
							static_assert( std::_Always_false<T>, "Expand not implemented for these bounding volume types." );
					}, Data
				);
			}
		}
	};

	//=============================================================================================
	// Sphere Implementations
	//=============================================================================================

	inline bool Sphere::Overlaps( const Sphere& a_Sphere, float a_Epsilon ) const
	{
		const float radiusSum = Radius + a_Sphere.Radius;
		return ( a_Sphere.Center - Center ).LengthSqr() <= ( radiusSum * radiusSum ) + a_Epsilon;
	}

	//=============================================================================================
	// AABB Implementations
	//=============================================================================================

	inline bool AABB::Overlaps( const Sphere& a_Sphere, float a_Epsilon ) const
	{
		// Find the point on the AABB closest to the sphere center
		Vector3 closestPoint = Math::Clamp( a_Sphere.Center, Min, Max );
		// Check if the distance from the closest point to the sphere center is less than or equal to the radius
		return ( closestPoint - a_Sphere.Center ).LengthSqr() <= ( a_Sphere.Radius * a_Sphere.Radius ) + a_Epsilon;
	}

	inline bool AABB::Overlaps( const AABB& a_Box, float a_Epsilon ) const
	{
		return ( Min.X <= a_Box.Max.X + a_Epsilon && Max.X >= a_Box.Min.X - a_Epsilon ) &&
			   ( Min.Y <= a_Box.Max.Y + a_Epsilon && Max.Y >= a_Box.Min.Y - a_Epsilon ) &&
			   ( Min.Z <= a_Box.Max.Z + a_Epsilon && Max.Z >= a_Box.Min.Z - a_Epsilon );
	}

	//=============================================================================================
	// OBB Implementations
	//=============================================================================================

	inline bool OBB::Overlaps( const Sphere& a_Sphere, float a_Epsilon ) const
	{
		// Transform the sphere center into the OBB's local space
		Matrix4 invOrientation = Math::Inverse( Orientation );
		Vector3 localCenter = Vector3( invOrientation * Vector4( a_Sphere.Center, 1.0f ) );
		// Find the point on the OBB closest to the sphere center in local space
		Vector3 closestPoint = Math::Clamp( localCenter, -HalfExtents, HalfExtents );
		// Check if the distance from the closest point to the sphere center is less than or equal to the radius
		return ( closestPoint - localCenter ).LengthSqr() <= ( a_Sphere.Radius * a_Sphere.Radius ) + a_Epsilon;
	}

	inline bool OBB::Overlaps( const AABB& a_Box, float a_Epsilon ) const
	{
		// Convert AABB to OBB in world space
		OBB boxAsOBB;
		boxAsOBB.Orientation = Matrix4( 1.0f );
		boxAsOBB.HalfExtents = ( a_Box.Max - a_Box.Min ) * 0.5f;
		boxAsOBB.Orientation[3] = Vector4( ( a_Box.Min + a_Box.Max ) * 0.5f, 1.0f );
		return Overlaps( boxAsOBB, a_Epsilon );
	}

	inline bool OBB::Overlaps( const OBB& a_Box, float a_Epsilon ) const
	{
		assert( false ); // OBB vs OBB overlap test not implemented
		return true;
	}

} // namespace Tridium