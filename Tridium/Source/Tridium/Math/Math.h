#pragma once

#include "glm/glm.hpp"
#include "glm/fwd.hpp"

typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;

typedef glm::mat2 Matrix2;
typedef glm::mat3 Matrix3;
typedef glm::mat4 Matrix4;

typedef glm::quat Quaternion;

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

namespace Math {}