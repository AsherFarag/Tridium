#pragma once
#include <Jolt/Math/Vector.h>

#include <Tridium/Physics/MotionType.h>

namespace Tridium::Util {

	JPH::Vec3 ToJoltVec3( const Vector3& a_Vector )
	{
		return JPH::Vec3( a_Vector.x, a_Vector.y, a_Vector.z );
	}

	JPH::Quat ToJoltQuat( const Quaternion& a_Quat )
	{
		return JPH::Quat( a_Quat.x, a_Quat.y, a_Quat.z, a_Quat.w );
	}

	Vector3 ToTridiumVec3( const JPH::Vec3& a_Vector )
	{
		return Vector3( a_Vector.GetX(), a_Vector.GetY(), a_Vector.GetZ() );
	}

	Quaternion ToTridiumQuat( const JPH::Quat& a_Quat )
	{
		return Quaternion( a_Quat.GetW(), a_Quat.GetX(), a_Quat.GetY(), a_Quat.GetZ() );
	}

	JPH::EMotionType ToJoltMotionType( EMotionType a_MotionType )
	{
		switch ( a_MotionType )
		{
			case EMotionType::Static: return JPH::EMotionType::Static;
			case EMotionType::Dynamic: return JPH::EMotionType::Dynamic;
			case EMotionType::Kinematic: return JPH::EMotionType::Kinematic;
		}

		TE_CORE_ASSERT( false, "Unknown motion type" );
		return JPH::EMotionType::Static;
	}

	EMotionType ToTridiumMotionType( JPH::EMotionType a_MotionType )
	{
		switch ( a_MotionType )
		{
			case JPH::EMotionType::Static: return EMotionType::Static;
			case JPH::EMotionType::Dynamic: return EMotionType::Dynamic;
			case JPH::EMotionType::Kinematic: return EMotionType::Kinematic;
		}

		TE_CORE_ASSERT( false, "Unknown motion type" );
		return EMotionType::Static;
	}

}