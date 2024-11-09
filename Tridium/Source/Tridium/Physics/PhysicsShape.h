#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {
	enum class EShapeType : uint8_t
	{
		Sphere,
		Box,
		Triangle,
		Capsule,
		TaperedCapsule,
		Cylinder,
		Mesh,
		HeightField,
		Plane,
		TaperedCylinder,
	};
}