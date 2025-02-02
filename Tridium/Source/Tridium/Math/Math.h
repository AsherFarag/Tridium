#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Tridium {

	using f32Vector2 = glm::vec<2, float>;
	using f32Vector3 = glm::vec<3, float>;
	using f32Vector4 = glm::vec<4, float>;

	using i32Vector2 = glm::vec<2, int32_t>;
	using i32Vector3 = glm::vec<3, int32_t>;
	using i32Vector4 = glm::vec<4, int32_t>;

	using u32Vector2 = glm::vec<2, uint32_t>;
	using u32Vector3 = glm::vec<3, uint32_t>;
	using u32Vector4 = glm::vec<4, uint32_t>;

	using Vector2 = f32Vector2;
	using Vector3 = f32Vector3;
	using Vector4 = f32Vector4;

	using iVector2 = i32Vector2;
	using iVector3 = i32Vector3;
	using iVector4 = i32Vector4;

	using uVector2 = u32Vector2;
	using uVector3 = u32Vector3;
	using uVector4 = u32Vector4;

	using Matrix2 = glm::mat2;
	using Matrix3 = glm::mat3;
	using Matrix4 = glm::mat4;

	using Quaternion = glm::quat;
}

namespace Tridium::Math
{
	template<typename T>
	constexpr T Min( T a, T b )
	{
		return a < b ? a : b;
	}

	template<typename T>
	constexpr T Max( T a, T b )
	{
		return a > b ? a : b;
	}

	template<typename T>
	constexpr T Clamp( T value, T min, T max )
	{
		return value < min ? min : value > max ? max : value;
	}

	template<typename T>
	constexpr T Lerp( T a, T b, float t )
	{
		return a + ( b - a ) * t;
	}

	template<typename T>
	constexpr T ClampedLerp( T a, T b, float t )
	{
		return Lerp( a, b, Clamp( t, 0.0f, 1.0f ) );
	}

	void DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Vector3& o_Rot, Vector3& o_Scale );
	void DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Quaternion& o_Rot, Vector3& o_Scale );
}