#pragma once
#include <glm/glm.hpp>
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MathConstants.h"

namespace Tridium::Math {

	// Returns the machine epsilon, that is,
	// the difference between 1.0 and the next value representable by the floating-point type _Gen.
	// ( From cppreference on std::numeric_limits )
	template<Concepts::Arithmetic _Gen>
	constexpr inline _Gen Epsilon() { return std::numeric_limits<_Gen>::epsilon(); }

	// Returns true if two values are approximately equal within a given epsilon.
	// This is useful for comparing floating point values.
	template<Concepts::Arithmetic _Gen>
	constexpr inline bool Approx( 
		const _Gen& a_A,
		const _Gen& a_B,
		const _Gen& a_Epsilon = _Gen( 0.000001 ) )
	{
		return glm::epsilonEqual( a_A, a_B, a_Epsilon );
	}

	// Returns true if two values are approximately equal within a given epsilon.
	// This is useful for comparing floating point values.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline bool Approx( 
		const Vector<_Count, _Gen>& a_A,
		const Vector<_Count, _Gen>& a_B,
		const _Gen& a_Epsilon = _Gen( 0.000001 ) )
	{
		return glm::epsilonEqual( a_A, a_B, a_Epsilon );
	}

	// Converts a value (in degrees) to radians.
	template<Concepts::Arithmetic _Gen>
	constexpr inline _Gen Radians( const _Gen& a_Degrees )
	{
		return glm::radians( a_Degrees );
	}

	// Converts a vector (in degrees) to radians.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Radians( const Vector<_Count, _Gen>& a_Degrees )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::radians( Cast<const GLMType>( a_Degrees ) );
	}

	// Converts a value (in radians) to degrees.
	template<Concepts::Arithmetic _Gen>
	constexpr inline _Gen Degrees( const _Gen& a_Radians )
	{
		return glm::degrees( a_Radians );
	}

	// Converts a vector (in radians) to degrees.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Degrees( const Vector<_Count, _Gen>& a_Radians )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::degrees( Cast<const GLMType>( a_Radians ) );
	}

	// Returns the minimum of two values.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Min( const _Gen& a_A, const _Gen& a_B ) { return glm::min( a_A, a_B ); }

	// Returns the minimum of two vectors.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Min( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::min( Cast<const GLMType>( a_A ), Cast<const GLMType>( a_B ) );
	}

	// Returns the maximum of two values.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Max( const _Gen& a_A, const _Gen& a_B ) { return glm::max( a_A, a_B ); }

	// Returns the maximum of two vectors.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Max( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::max( Cast<const GLMType>( a_A ), Cast<const GLMType>( a_B ) );
	}

	// Clamps a value between a minimum and maximum value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Clamp( const _Gen& a_Value, const _Gen& a_Min, const _Gen& a_Max ) { return glm::clamp( a_Value, a_Min, a_Max ); }

	// Clamps a vector between a minimum and maximum value.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Clamp( const Vector<_Count, _Gen>& a_Value, const Vector<_Count, _Gen>& a_Min, const Vector<_Count, _Gen>& a_Max )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::clamp( Cast<const GLMType>( a_Value ), Cast<const GLMType>( a_Min ), Cast<const GLMType>( a_Max ) );
	}

	// Linear interpolates between a and b by the interpolation factor.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Lerp( const _Gen& a_A, const _Gen& a_B, const _Gen& a_InterpolationFactor ) { return glm::mix( a_A, a_B, a_InterpolationFactor ); }

	// Linear interpolation between a and b by the interpolation factor.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline Vector<_Count, _Gen> Lerp( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B, const _Gen& a_InterpolationFactor )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::mix( Cast<const GLMType>( a_A ), Cast<const GLMType>( a_B ), a_InterpolationFactor );
	}

	// Clamped linear interpolation between a and b by the interpolation factor ( 0 - 1 ).
	// i.e. the result will always be between a and b.
	template<typename T, Concepts::Arithmetic _Interp>
	constexpr inline T ClampedLerp( const T& a_A, const T& a_B, const _Interp& a_InterpolationFactor )
	{ 
		return Lerp( a_A, a_B, Clamp( a_InterpolationFactor, Cast<_Interp>( 0 ), Cast<_Interp>( 1 ) ) );
	}

	// Returns the absolute value of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Abs( const _Gen& a_Value ) { return glm::abs( a_Value ); }

	// Returns the sign of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Sign( const _Gen& a_Value ) { return glm::sign( a_Value ); }

	// Returns the square root of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Sqrt( const _Gen& a_Value ) { return glm::sqrt( a_Value ); }

	// Returns the square of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Sqr( const _Gen& a_Value ) { return a_Value * a_Value; }

	// Returns the sine of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Sin( const _Gen& a_Value ) { return glm::sin( a_Value ); }

	// Returns the cosine of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Cos( const _Gen& a_Value ) { return glm::cos( a_Value ); }

	// Returns the tangent of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Tan( const _Gen& a_Value ) { return glm::tan( a_Value ); }

	// Returns the arc sine of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto ASin( const _Gen& a_Value ) { return glm::asin( a_Value ); }

	// Returns the arc cosine of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto ACos( const _Gen& a_Value ) { return glm::acos( a_Value ); }

	// Returns the arc tangent of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto ATan( const _Gen& a_Value ) { return glm::atan( a_Value ); }

	// Returns the arc tangent of y/x.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto ATan( const _Gen& a_Y, const _Gen& a_X ) { return glm::atan( a_Y, a_X ); }

	// Returns the power of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Pow( _Gen a_Base, _Gen a_Exponent ) { return glm::pow( a_Base, a_Exponent ); }

	// Returns the natural logarithm of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Log( _Gen a_Value ) { return glm::log( a_Value ); }

	// Returns the base 2 logarithm of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Log2( _Gen a_Value ) { return glm::log2( a_Value ); }

	// Returns the smallest integer value not less than a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Ceil( _Gen a_Value ) { return glm::ceil( a_Value ); }

	// Returns the largest integer value not greater than a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Floor( _Gen a_Value ) { return glm::floor( a_Value ); }

	// Returns the nearest integer value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Round( _Gen a_Value ) { return glm::round( a_Value ); }

	// Returns the fractional part of a value.
	template<Concepts::Arithmetic _Gen>
	constexpr inline auto Fract( _Gen a_Value ) { return glm::fract( a_Value ); }

#pragma region Vector

	// Returns the cross product of two vectors.
	template<size_t _Count, typename _Gen>
	constexpr inline Vector<_Count, _Gen> Cross( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::cross( Cast<const GLMType&>( a_A ), Cast<const GLMType&>( a_B ) );
	}

	// Returns the dot product of two vectors.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline _Gen Dot( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::dot( Cast<const GLMType&>( a_A ), Cast<const GLMType&>( a_B ) );
	}

	// Returns the distance between two vectors.
	template<size_t _Count, Concepts::Arithmetic _Gen>
	constexpr inline _Gen Distance( const Vector<_Count, _Gen>& a_A, const Vector<_Count, _Gen>& a_B )
	{
		using GLMType = typename Vector<_Count, _Gen>::GLMType;
		return glm::distance( Cast<const GLMType&>( a_A ), Cast<const GLMType&>( a_B ) );
	}


#pragma endregion // Vector

#pragma region Quaternion

	// Returns a quaternion from an axis and an angle.
	template<Concepts::Arithmetic _Gen>
	inline TQuaternion<_Gen> AngleAxis( _Gen a_Angle, const Vector<3, _Gen>& a_Axis )
	{
		using GLMType = typename Vector<3, _Gen>::GLMType;
		return glm::angleAxis( a_Angle, Cast<const GLMType&>( a_Axis ) );
	}

	// Returns a quaternion from Euler angles. ( Radians )
	template<Concepts::Arithmetic _Gen>
	constexpr inline TQuaternion<_Gen> QuatEuler( _Gen a_Yaw, _Gen a_Pitch, _Gen a_Roll )
	{
		return TQuaternion<_Gen>( Vector<3, _Gen>( a_Yaw, a_Pitch, a_Roll ) );
	}

	// Returns a Vector3 in euler angles from a quaternion. ( Radians )
	template<Concepts::Arithmetic _Gen>
	inline Vector<3, _Gen> EulerAngles( const TQuaternion<_Gen>& a_Quat )
	{
		return glm::eulerAngles( a_Quat );
	}

	template<Concepts::Arithmetic _Gen>
	inline Vector<3, _Gen> Rotate( const TQuaternion<_Gen>& a_Quat, const Vector<3, _Gen>& a_Vector )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return Cast<Vector<3, _Gen>>( glm::rotate( a_Quat, Cast<const GLMVecType&>( a_Vector ) ) );
	}

	// Returns a lookat quaternion from a direction vector and an up vector.
	template<Concepts::Arithmetic _Gen>
	inline TQuaternion<_Gen> QuatLookAt( const Vector<3, _Gen>& a_Direction, const Vector<3, _Gen>& a_Up )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::quatLookAt( Cast<const GLMVecType&>( a_Direction ), Cast<const GLMVecType&>( a_Up ) );
	}

#pragma endregion // Quaternion

#pragma region Matrix

	// Translates a matrix by a given translation vector.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Translate( const Vector<3, _Gen>& a_Translation )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::translate( Matrix4x4<_Gen>( Cast<_Gen>( 1 ) ), Cast<const GLMVecType&>( a_Translation ) );
	}

	// Translates a matrix by a given translation vector.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Translate( const Matrix4x4<_Gen> a_Matrix, const Vector<3, _Gen>& a_Translation )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::translate( a_Matrix, Cast<const GLMVecType&>( a_Translation ) );
	}

	// Rotates a matrix by a given angle around a given axis.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Rotate( const _Gen& a_Angle, const Vector<3, _Gen>& a_Axis )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::rotate( Matrix4x4<_Gen>( Cast<_Gen>( 1 ) ), a_Angle, Cast<const GLMVecType&>( a_Axis ) );
	}

	// Rotates a matrix by a given angle around a given axis.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Rotate( const Matrix4x4<_Gen> a_Matrix, const _Gen& a_Angle, const Vector<3, _Gen>& a_Axis )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::rotate( a_Matrix, a_Angle, Cast<const GLMVecType&>( a_Axis ) );
	}

	// Scales a matrix by a given scale vector.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Scale( const Vector<3, _Gen>& a_Scale )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::scale( Matrix4x4<_Gen>( Cast<_Gen>( 1 ) ), Cast<const GLMVecType&>( a_Scale ) );
	}

	// Scales a matrix by a given scale vector.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Scale( const Matrix4x4<_Gen> a_Matrix, const Vector<3, _Gen>& a_Scale )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::scale( a_Matrix, Cast<const GLMVecType&>( a_Scale ) );
	}

	// Creates a look-at matrix from a position, target, and up vector.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> LookAt( const Vector<3, _Gen>& a_Position, const Vector<3, _Gen>& a_Target, const Vector<3, _Gen>& a_Up )
	{
		using GLMVecType = typename Vector<3, _Gen>::GLMType;
		return glm::lookAt( Cast<const GLMVecType&>( a_Position ), Cast<const GLMVecType&>( a_Target ), Cast<const GLMVecType&>( a_Up ) );
	}

	// Creates a perspective projection matrix.
	template<Concepts::Arithmetic _Gen>
	constexpr inline Matrix4x4<_Gen> Perspective( const _Gen& a_FovY, const _Gen& a_Aspect, const _Gen& a_ZNear, const _Gen& a_ZFar )
	{
		return glm::perspective( a_FovY, a_Aspect, a_ZNear, a_ZFar );
	}

	// Inverts a matrix.
	template<size_t _CountX, size_t _CountY, typename _Gen>
	constexpr inline Matrix<_CountX, _CountY, _Gen> Inverse( const Matrix<_CountX, _CountY, _Gen>& a_Matrix )
	{
		return glm::inverse( a_Matrix );
	}

	// Creates a matrix from a quaternion.
	template<Concepts::Arithmetic _Gen>
	inline Matrix4x4<_Gen> ToMat4( const TQuaternion<_Gen>& a_Quat )
	{
		return glm::toMat4( a_Quat );
	}

	// Decomposes a transformation matrix into position, rotation, and scale.
	template<Concepts::Arithmetic _Gen>
	inline bool DecomposeTransform( const Matrix4x4<_Gen>& a_Mat, Vector<3, _Gen>& o_Pos, TQuaternion<_Gen>& o_Rot, Vector<3, _Gen>& o_Scale )
	{
		glm::vec<3, _Gen> skew;
		glm::vec<4, _Gen> perspective;
		return glm::decompose( a_Mat, Cast<glm::vec<3, _Gen>&>( o_Scale ), o_Rot, Cast<glm::vec<3, _Gen>&>( o_Pos ), skew, perspective );
	}

	// Decomposes a transformation matrix into position, rotation and scale.
	template<Concepts::Arithmetic _Gen>
	inline bool DecomposeTransform( const Matrix4x4<_Gen>& a_Mat, Vector<3, _Gen>& o_Pos, Vector<3, _Gen>& o_Rot, Vector<3, _Gen>& o_Scale )
	{
		glm::vec<3, _Gen> skew;
		glm::vec<4, _Gen> perspective;
		glm::qua<_Gen> rot;
		bool result = glm::decompose( a_Mat, Cast<glm::vec<3, _Gen>&>( o_Scale ), rot, Cast<glm::vec<3, _Gen>&>( o_Pos ), skew, perspective );
		o_Rot = glm::eulerAngles( rot );
		return result;
	}

	// Decomposes a transformation matrix into position, rotation, scale, skew, and perspective.
	template<Concepts::Arithmetic _Gen>
	inline bool DecomposeTransform( 
		const Matrix4x4<_Gen>& a_Mat,
		Vector<3, _Gen>& o_Pos, TQuaternion<_Gen>& o_Rot, Vector<3, _Gen>& o_Scale,
		Vector<3, _Gen>& o_Skew, Vector<3, _Gen>& o_Perspective )
	{
		return glm::decompose(
			a_Mat, Cast<glm::vec<3, _Gen>&>( o_Scale ), o_Rot, Cast<glm::vec<3, _Gen>&>( o_Pos ),
			Cast<glm::vec<3, _Gen>&>( o_Skew ), Cast<glm::vec<4, _Gen>&>( o_Perspective )
		);
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	template<Concepts::Arithmetic _Gen>
	inline Matrix4 RecomposeTransform( const Vector<3, _Gen>& a_Pos, const TQuaternion<_Gen>& a_Rot, const Vector<3, _Gen>& a_Scale )
	{
		return glm::recompose( 
			a_Scale, a_Rot, Cast<const glm::vec<3, _Gen>&>( a_Pos ), glm::vec<3, _Gen>( 0 ), glm::vec<4, _Gen>( 0 ) 
		);
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	template<Concepts::Arithmetic _Gen>
	inline Matrix4 RecomposeTransform( const Vector<3, _Gen>& a_Pos, const Vector<3, _Gen>& a_Rot, const Vector<3, _Gen>& a_Scale )
	{
		return glm::recompose( 
			a_Scale, glm::qua<_Gen>( a_Rot ), Cast<const glm::vec<3, _Gen>&>( a_Pos ), glm::vec<3, _Gen>( 0 ), glm::vec<4, _Gen>( 0 ) 
		);
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	template<Concepts::Arithmetic _Gen>
	inline Matrix4 RecomposeTransform(
		const Vector<3, _Gen>& a_Pos, const TQuaternion<_Gen>& a_Rot, const Vector<3, _Gen>& a_Scale,
		const Vector<3, _Gen>& a_Skew, const Vector<3, _Gen>& a_Perspective )
	{
		return glm::recompose(
			a_Scale, a_Rot, Cast<const glm::vec<3, _Gen>&>( a_Pos ),
			Cast<const glm::vec<3, _Gen>&>( a_Skew ), Cast<const glm::vec<4, _Gen>&>( a_Perspective )
		);
	}

#pragma endregion // Matrix

} // namespace Tridium::Math