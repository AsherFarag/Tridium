#pragma once
#include <Tridium/Math/Math.h>
#include <glm/gtx/string_cast.hpp>

namespace Tridium {

	struct Color : public Vector4
	{
		constexpr Color() : Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) {}
		constexpr Color( float a_Value ) : Vector4( a_Value, a_Value, a_Value, 1.0f ) {}
		constexpr Color( float a_Red, float a_Green, float a_Blue, float a_Alpha = 1.0f ) : Vector4( a_Red, a_Green, a_Blue, a_Alpha ) {}
	};

}

// Hot fix for glm::to_string not being defined in the glm namespace
namespace glm {
	template<>
	GLM_FUNC_QUALIFIER std::string to_string( Tridium::Color const& x )
	{
		return glm::to_string( static_cast<glm::vec4>( x ) );
	}
}