#pragma once
#include <Tridium/Core/Core.h>
#include <Jolt/Math/Vector.h>
#include <Tridium/Physics/MotionType.h>
#include <Tridium/Physics/PhysicsLayer.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Tridium::Util {

	inline JPH::Vec3 ToJoltVec3( const Vector3& a_Vector )
	{
		return JPH::Vec3( a_Vector.x, a_Vector.y, a_Vector.z );
	}

	inline JPH::Quat ToJoltQuat( const Quaternion& a_Quat )
	{
		return JPH::Quat( a_Quat.x, a_Quat.y, a_Quat.z, a_Quat.w );
	}

	inline JPH::Mat44 ToJoltMat44( const Matrix4& a_Matrix )
	{
		JPH::Mat44 matrix;

		// JPH Mat44 stores its values as rows; glm mat4 expects column-major format.
		for ( int row = 0; row < 4; ++row )
		{
			matrix( 0, row ) = a_Matrix[0][row]; // Column 0
			matrix( 1, row ) = a_Matrix[1][row]; // Column 1
			matrix( 2, row ) = a_Matrix[2][row]; // Column 2
			matrix( 3, row ) = a_Matrix[3][row]; // Column 3
		}

		return matrix;

	}

	inline Vector3 ToTridiumVec3( const JPH::Vec3& a_Vector )
	{
		return Vector3( a_Vector.GetX(), a_Vector.GetY(), a_Vector.GetZ() );
	}

	inline Quaternion ToTridiumQuat( const JPH::Quat& a_Quat )
	{
		return Quaternion( a_Quat.GetW(), a_Quat.GetX(), a_Quat.GetY(), a_Quat.GetZ() );
	}

	inline Matrix4 ToTridiumMat4( const JPH::Mat44& a_Matrix )
	{
		Matrix4 matrix;

		// JPH::Mat44 stores values in row-major order, while glm::mat4 expects column-major order.
		for ( int row = 0; row < 4; ++row )
		{
			for ( int col = 0; col < 4; ++col )
			{
				matrix[col][row] = a_Matrix( row, col ); // Transpose row-major to column-major
			}
		}

		return matrix;
	}

	inline JPH::ObjectLayer ToJoltObjectLayer( EPhysicsLayer a_PhysicsLayer )
	{
		return static_cast<JPH::ObjectLayer>( a_PhysicsLayer );
	}

	inline EPhysicsLayer ToTridiumObjectLayer( JPH::ObjectLayer a_ObjectLayer )
	{
		return static_cast<EPhysicsLayer>( a_ObjectLayer );
	}

	inline JPH::EMotionType ToJoltMotionType( EMotionType a_MotionType )
	{
		switch ( a_MotionType )
		{
			case EMotionType::Static: return JPH::EMotionType::Static;
			case EMotionType::Dynamic: return JPH::EMotionType::Dynamic;
			case EMotionType::Kinematic: return JPH::EMotionType::Kinematic;
		}

		ASSERT_LOG( false, "Unknown motion type" );
		return JPH::EMotionType::Static;
	}

	inline EMotionType ToTridiumMotionType( JPH::EMotionType a_MotionType )
	{
		switch ( a_MotionType )
		{
			case JPH::EMotionType::Static: return EMotionType::Static;
			case JPH::EMotionType::Dynamic: return EMotionType::Dynamic;
			case JPH::EMotionType::Kinematic: return EMotionType::Kinematic;
		}

		ASSERT_LOG( false, "Unknown motion type" );
		return EMotionType::Static;
	}

}