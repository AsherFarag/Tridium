#pragma once
#include "Math.h"

namespace Tridium {

	struct Rotator
	{
		// Avoid modifying directly, use SetFromEuler.
		Vector3 Euler;
		// Avoid modifying directly, use SetFromQuaternion.
		Quaternion Quat;

		Rotator()
		{
			SetFromEuler( Vector3( 0.0f, 0.0f, 0.0f ) );
		}

		Rotator( float a_XRad, float a_YRad, float a_ZRad )
		{
			SetFromEuler( Vector3( a_XRad, a_YRad, a_ZRad ) );
		}

		Rotator( const Vector3& a_EulerRad )
		{
			SetFromEuler( a_EulerRad );
		}

		Rotator( const Quaternion& a_Quat )
		{
			SetFromQuaternion( a_Quat );
		}

		Vector3 GetEuler() const
		{
			return Euler;
		}

		Quaternion GetQuaternion() const
		{
			return Quat;
		}

		Vector3 GetForward() const
		{
			return glm::normalize( glm::rotate( Quat, Cast<const Vector3::GLMType&>( Vector3::Forward() * -1 ) ) );
		}

		Vector3 GetRight() const
		{
			return glm::normalize( glm::rotate( Quat, Cast<const Vector3::GLMType&>( Vector3::Right() ) ) );
		}

		Vector3 GetUp() const
		{
			return glm::normalize( glm::rotate( Quat, Cast<const Vector3::GLMType&>( Vector3::Up() ) ) );
		}

		void SetFromEuler( const Vector3& a_Euler )
		{
			Euler = a_Euler;
			Quat = glm::quat( a_Euler );
		}

		void SetFromQuaternion( const Quaternion& a_Quat )
		{
			Quat = a_Quat;
			Euler = glm::eulerAngles( a_Quat );
		}

		void SetFromAxisAngle( const Vector3& a_Axis, float a_Angle )
		{
			Quat = Math::AngleAxis( a_Angle, a_Axis );
			Euler = Math::EulerAngles( Quat );
		}

		void SetFromLookAt( const Vector3& a_Position, const Vector3& a_Target, const Vector3& a_Up )
		{
			Quat = glm::quatLookAt( Cast<Vector3::GLMType>( Vector3( a_Target - a_Position ).Normalized() ),
				Cast<Vector3::GLMType>( a_Up.Normalized() ) );
			Euler = glm::eulerAngles( Quat );
		}

		void SetFromToRotation( const Vector3& a_From, const Vector3& a_To )
		{
			Quat = glm::rotation( Cast<Vector3::GLMType>( a_From.Normalized() ),
				Cast<Vector3::GLMType>( a_To.Normalized() ) );
			Euler = glm::eulerAngles( Quat );
		}

		void SetFromRotationMatrix( const Matrix4& a_Mat )
		{
			Quat = glm::quat_cast( a_Mat );
			Euler = glm::eulerAngles( Quat );
		}
		
	};

}