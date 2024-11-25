#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Tridium {
	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;

	using iVector2 = glm::ivec2;
	using iVector3 = glm::ivec3;
	using iVector4 = glm::ivec4;

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