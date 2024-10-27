#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Core/Color.h>

namespace Tridium {
	constexpr uint32_t MAX_POINT_LIGHTS = 4;

	struct PointLight
	{
		Vector3 Position;
		Color Color;
		float Intensity;
		float FalloffExponent;
		float AttenuationRadius;
	};

	constexpr uint32_t MAX_SPOT_LIGHTS = 4;

	struct SpotLight
	{
		Vector3 Position;
		Vector3 Direction;
		Color Color;
		float Intensity;
		float FalloffExponent;
		float AttenuationRadius;
		float InnerConeAngle;
		float OuterConeAngle;
	};

	constexpr uint32_t MAX_DIRECTIONAL_LIGHTS = 1;

	// Temp?
	class Framebuffer;

	struct DirectionalLight
	{
		Vector3 Direction;
		Color Color;
		float Intensity;

		// Temp?
		SharedPtr<Framebuffer> ShadowMap;
	};
}