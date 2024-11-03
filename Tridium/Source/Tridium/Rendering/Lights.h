#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Core/Color.h>

namespace Tridium {

	// Temp?
	class Framebuffer;

	constexpr uint32_t MAX_POINT_LIGHTS = 4;

	struct PointLight
	{
		Vector3 Position;
		Color Color;
		float Intensity;
		float FalloffExponent;
		float AttenuationRadius;

		// Temp?
		SharedPtr<Framebuffer> ShadowMap;
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

	struct DirectionalLight
	{
		Vector3 Direction;
		Color Color;
		float Intensity;
		bool CastsShadows;

		// Temp?
		SharedPtr<Framebuffer> ShadowMap;
	};
}