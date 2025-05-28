#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include "Vector.h"
#include <Tridium/Utils/Concepts.h>

namespace Tridium {

	template<Concepts::Arithmetic T>
	using TQuaternion = glm::qua<T>;

	using Quaternion = TQuaternion<float>;

	// Vector & Quaternion Operators

	template<Concepts::Arithmetic T>
	constexpr Vector<3, T> operator*( const TQuaternion<T>& a_Quat, const Vector<3, T>& a_Vector ) noexcept
	{
		using GLMVecType = typename Vector<3, T>::GLMType;
		return glm::rotate( a_Quat, Cast<const GLMVecType&>( a_Vector ) );
	}

	template<Concepts::Arithmetic T>
	constexpr Vector<3, T> operator*( const Vector<3, T>& a_Vector, const TQuaternion<T>& a_Quat ) noexcept
	{
		using GLMVecType = typename Vector<3, T>::GLMType;
		return glm::rotate( a_Quat, Cast<const GLMVecType&>( a_Vector ) );
	}
}