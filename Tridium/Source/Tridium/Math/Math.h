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
	void DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Vector3& o_Rot, Vector3& o_Scale );
	void DecomposeTransform( const Matrix4& a_Mat, Vector3& o_Pos, Quaternion& o_Rot, Vector3& o_Scale );
}