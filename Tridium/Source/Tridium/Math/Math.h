#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

#pragma region To String Functions

//std::string ToString( const Vector2& v )
//{
//	return { "x: [" + std::to_string( v.x ) + "] y : [" + std::to_string( v.y ) + "]" };
//}
//
//std::string ToString( const Vector3& v )
//{
//	return { "x: [" + std::to_string( v.x ) + "] y : [" + std::to_string( v.y ) + "] z: [" + std::to_string( v.z ) + "]" };
//}
//
//std::string ToString( const Vector4& v )
//{
//	return { "x: [" + std::to_string( v.x ) + "] y : [" + std::to_string( v.y ) + "] z: [" + std::to_string( v.z ) + "] w: [" + std::to_string( v.w ) + "]" };
//}

#pragma endregion

namespace Tridium::Math
{
	void DecomposeTransform( const Matrix4& mat, Vector3& outPos, Vector3& outRot, Vector3& outScale );
	void DecomposeTransform( const Matrix4& mat, Vector3& outPos, Quaternion& outRot, Vector3& outScale );
}