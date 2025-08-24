#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/matrix.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include <Tridium/Utils/Concepts.h>
#include <Tridium/Math/Vector.h>

namespace Tridium {

	template<size_t _CountX, size_t _CountY, Concepts::Arithmetic T>
	using Matrix = glm::mat<_CountX, _CountY, T>;

	template<Concepts::Arithmetic T>
	using Matrix2x2 = Matrix<2, 2, T>;
	template<Concepts::Arithmetic T>
	using Matrix2x3 = Matrix<2, 3, T>;
	template<Concepts::Arithmetic T>
	using Matrix2x4 = Matrix<2, 4, T>;
	template<Concepts::Arithmetic T>
	using Matrix3x2 = Matrix<3, 2, T>;
	template<Concepts::Arithmetic T>
	using Matrix3x3 = Matrix<3, 3, T>;
	template<Concepts::Arithmetic T>
	using Matrix3x4 = Matrix<3, 4, T>;
	template<Concepts::Arithmetic T>
	using Matrix4x2 = Matrix<4, 2, T>;
	template<Concepts::Arithmetic T>
	using Matrix4x3 = Matrix<4, 3, T>;
	template<Concepts::Arithmetic T>
	using Matrix4x4 = Matrix<4, 4, T>;

	using Matrix2 = Matrix2x2<float>;
	using Matrix3 = Matrix3x3<float>;
	using Matrix4 = Matrix4x4<float>;

}