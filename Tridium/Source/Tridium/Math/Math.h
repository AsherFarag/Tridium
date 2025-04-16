#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Tridium {

	// Vector types.

	template<typename T> using TVector2 = glm::vec<2, T>;
	template<typename T> using TVector3 = glm::vec<3, T>;
	template<typename T> using TVector4 = glm::vec<4, T>;

	using f32Vector2 = TVector2<float>;
	using f32Vector3 = TVector3<float>;
	using f32Vector4 = TVector4<float>;

	using i32Vector2 = TVector2<int32_t>;
	using i32Vector3 = TVector3<int32_t>;
	using i32Vector4 = TVector4<int32_t>;

	using u32Vector2 = TVector2<uint32_t>;
	using u32Vector3 = TVector3<uint32_t>;
	using u32Vector4 = TVector4<uint32_t>;

	using Vector2 = f32Vector2;
	using Vector3 = f32Vector3;
	using Vector4 = f32Vector4;

	using iVector2 = i32Vector2;
	using iVector3 = i32Vector3;
	using iVector4 = i32Vector4;

	using uVector2 = u32Vector2;
	using uVector3 = u32Vector3;
	using uVector4 = u32Vector4;

	// Matrix types.

	template<typename T> using TMatrix2 = glm::mat<2, 2, T>;
	template<typename T> using TMatrix3 = glm::mat<3, 3, T>;
	template<typename T> using TMatrix4 = glm::mat<4, 4, T>;

	using f32Matrix2 = TMatrix2<float>;
	using f32Matrix3 = TMatrix3<float>;
	using f32Matrix4 = TMatrix4<float>;

	using i32Matrix2 = TMatrix2<int32_t>;
	using i32Matrix3 = TMatrix3<int32_t>;
	using i32Matrix4 = TMatrix4<int32_t>;

	using u32Matrix2 = TMatrix2<uint32_t>;
	using u32Matrix3 = TMatrix3<uint32_t>;
	using u32Matrix4 = TMatrix4<uint32_t>;

	using Matrix2 = f32Matrix2;
	using Matrix3 = f32Matrix3;
	using Matrix4 = f32Matrix4;

	using iMatrix2 = i32Matrix2;
	using iMatrix3 = i32Matrix3;
	using iMatrix4 = i32Matrix4;

	using uMatrix2 = u32Matrix2;
	using uMatrix3 = u32Matrix3;
	using uMatrix4 = u32Matrix4;

	// Quaternion types.

	template<typename T> using TQuaternion = glm::qua<T>;
	using f32Quaternion = TQuaternion<float>;
	using Quaternion = f32Quaternion;
}

namespace Tridium::Math {

	// Returns the minimum of two values.
	template<typename _A, typename _B>
	constexpr inline auto Min( _A a, _B b ) { return glm::min( a, b ); }

	// Returns the maximum of two values.
	template<typename _A, typename _B>
	constexpr inline auto Max( _A a, _B b ) { return glm::max( a, b ); }

	// Clamps a value between a minimum and maximum value.
	template<typename _Value, typename _Gen>
	constexpr inline auto Clamp( _Value a_Value, _Gen a_Min, _Gen a_Max ) { return glm::clamp( a_Value, a_Min, a_Max ); }

	// Linear interpolates between a and b by the interpolation factor.
	template<typename _GenT, typename _GenU>
	constexpr inline auto Lerp( _GenT a_A, _GenT a_B, _GenU a_InterpolationFactor ) { return glm::mix( a_A, a_B, a_InterpolationFactor ); }

	// Clamped linear interpolation between a and b by the interpolation factor ( 0 - 1 ).
	// i.e. the result will always be between a and b.
	template<typename _GenT, typename _GenU>
	constexpr inline auto ClampedLerp( _GenT a_A, _GenT a_B, _GenU a_InterpolationFactor ) { return Lerp( a_A, a_B, Clamp( a_InterpolationFactor, 0.0f, 1.0f ) ); }

	// Returns the absolute value of a value.
	template<typename _Gen>
	constexpr inline auto Abs( _Gen a_Value ) { return glm::abs( a_Value ); }

	// Returns the sign of a value.
	template<typename _Gen>
	constexpr inline auto Sign( _Gen a_Value ) { return glm::sign( a_Value ); }

	// Returns the square root of a value.
	template<typename _Gen>
	constexpr inline auto Sqrt( _Gen a_Value ) { return glm::sqrt( a_Value ); }

	// Returns the square of a value.
	template<typename _Gen>
	constexpr inline auto Sqr( _Gen a_Value ) { return a_Value * a_Value; }

	// Returns the sine of a value.
	template<typename _Gen>
	constexpr inline auto Sin( _Gen a_Value ) { return glm::sin( a_Value ); }

	// Returns the cosine of a value.
	template<typename _Gen>
	constexpr inline auto Cos( _Gen a_Value ) { return glm::cos( a_Value ); }

	// Returns the tangent of a value.
	template<typename _Gen>
	constexpr inline auto Tan( _Gen a_Value ) { return glm::tan( a_Value ); }

	// Returns the arc sine of a value.
	template<typename _Gen>
	constexpr inline auto ASin( _Gen a_Value ) { return glm::asin( a_Value ); }

	// Returns the arc cosine of a value.
	template<typename _Gen>
	constexpr inline auto ACos( _Gen a_Value ) { return glm::acos( a_Value ); }

	// Returns the arc tangent of a value.
	template<typename _Gen>
	constexpr inline auto ATan( _Gen a_Value ) { return glm::atan( a_Value ); }

	// Returns the arc tangent of y/x.
	template<typename _Gen>
	constexpr inline auto ATan2( _Gen a_Y, _Gen a_X ) { return glm::atan( a_Y, a_X ); }

	// Returns the power of a value.
	template<typename _Gen>
	constexpr inline auto Pow( _Gen a_Base, _Gen a_Exponent ) { return glm::pow( a_Base, a_Exponent ); }

	// Returns the natural logarithm of a value.
	template<typename _Gen>
	constexpr inline auto Log( _Gen a_Value ) { return glm::log( a_Value ); }

	// Returns the base 2 logarithm of a value.
	template<typename _Gen>
	constexpr inline auto Log2( _Gen a_Value ) { return glm::log2( a_Value ); }

	// Returns the smallest integer value not less than a value.
	template<typename _Gen>
	constexpr inline auto Ceil( _Gen a_Value ) { return glm::ceil( a_Value ); }

	// Returns the largest integer value not greater than a value.
	template<typename _Gen>
	constexpr inline auto Floor( _Gen a_Value ) { return glm::floor( a_Value ); }

	// Returns the nearest integer value.
	template<typename _Gen>
	constexpr inline auto Round( _Gen a_Value ) { return glm::round( a_Value ); }

	// Returns the fractional part of a value.
	template<typename _Gen>
	constexpr inline auto Fract( _Gen a_Value ) { return glm::fract( a_Value ); }

	// Returns the value in radians
	template<typename _Gen>
	constexpr inline auto Radians( _Gen a_Degrees ) { return glm::radians( a_Degrees ); }

	// Returns the value in degrees
	template<typename _Gen>
	constexpr inline auto Degrees( _Gen a_Radians ) { return glm::degrees( a_Radians ); }

	// Returns a quaternion from an axis and an angle.
	template<typename _Gen>
	inline TQuaternion<_Gen> AngleAxis( _Gen a_Angle, const TVector3<_Gen>& a_Axis )
	{
		return glm::angleAxis( a_Angle, a_Axis );
	}

	// Returns a quaternion from an axis and an angle. ( Radians )
	template<> 
	inline Quaternion AngleAxis( float a_Angle, const Vector3& a_Axis ) { return glm::angleAxis( a_Angle, a_Axis ); }

	// Returns a quaternion from Euler angles. ( Radians )
	template<typename _Gen>
	constexpr inline TQuaternion<_Gen> QuatEuler( _Gen a_Yaw, _Gen a_Pitch, _Gen a_Roll )
	{
		return glm::quat( glm::vec3( a_Yaw, a_Pitch, a_Roll ) );
	}

	// Returns a quaternion from Euler angles. ( Radians )
	template<>
	constexpr inline Quaternion QuatEuler( float a_Yaw, float a_Pitch, float a_Roll )
	{
		return glm::quat( glm::vec3( a_Yaw, a_Pitch, a_Roll ) );
	}

	// Returns a Vector3 in euler angles from a quaternion. ( Radians )
	template<typename _Gen>
	inline TVector3<_Gen> EulerAngles( const TQuaternion<_Gen>& a_Quat )
	{
		return glm::eulerAngles( a_Quat );
	}

	// Returns a Vector3 in euler angles from a quaternion. ( Radians )
	template<>
	inline Vector3 EulerAngles( const Quaternion& a_Quat )
	{
		return glm::eulerAngles( a_Quat );
	}

	// Decomposes a transformation matrix into position, rotation, and scale.
	inline bool DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Quaternion& o_Rot, Vector3& o_Scale )
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		return glm::decompose( a_Mat, o_Scale, o_Rot, o_Pos, skew, perspective );
	}

	// Decomposes a transformation matrix into position, rotation and scale.
	inline bool DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Vector3& o_Rot, Vector3& o_Scale )
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::quat rot;
		bool result = glm::decompose( a_Mat, o_Scale, rot, o_Pos, skew, perspective );
		o_Rot = glm::eulerAngles( rot );
		return result;
	}

	// Decomposes a transformation matrix into position, rotation, scale, skew, and perspective.
	inline bool DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Quaternion& o_Rot, Vector3& o_Scale, Vector3& o_Skew, Vector4& o_Perspective )
	{
		return glm::decompose( a_Mat, o_Scale, o_Rot, o_Pos, o_Skew, o_Perspective );
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	inline Matrix4 RecomposeTransform( const Vector3& a_Pos, const Quaternion& a_Rot, const Vector3& a_Scale )
	{
		return glm::recompose( a_Scale, a_Rot, a_Pos, glm::vec3( 0.0f ), glm::vec4( 0.0f ) );
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	inline Matrix4 RecomposeTransform( const Vector3& a_Pos, const Vector3& a_Rot, const Vector3& a_Scale )
	{
		return glm::recompose( a_Scale, glm::quat( a_Rot ), a_Pos, glm::vec3( 0.0f ), glm::vec4( 0.0f ) );
	}

	// Recomposes a transformation matrix from a previously decomposed matrix.
	inline Matrix4 RecomposeTransform( const Vector3& a_Pos, const Quaternion& a_Rot, const Vector3& a_Scale, const Vector3& a_Skew, const Vector4& a_Perspective )
	{
		return glm::recompose( a_Scale, a_Rot, a_Pos, a_Skew, a_Perspective );
	}

} // namespace Tridium::Math