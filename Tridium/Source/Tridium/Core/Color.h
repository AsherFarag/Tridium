#pragma once
#include <Tridium/Math/Math.h>

namespace Tridium {

	struct Color : public Vector4
	{
		Color() : Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) {}
		Color( float r, float g, float b, float a = 1.0f ) : Vector4( r, g, b, a ) {}
	};

}