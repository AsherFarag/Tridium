#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Graphics/Color.h>
#include <Tridium/Core/Memory.h>

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
		bool CastsShadows;
		Matrix4 LightSpaceMatrices[6];

		// Temp?
		uint32_t ShadowMapSize;
		SharedPtr<Framebuffer> ShadowMap;
	};

	constexpr uint32_t MAX_SPOT_LIGHTS = 32;

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
		bool CastsShadows;
		Matrix4 LightSpaceMatrix;

		// Temp?
		iVector2 ShadowMapSize;
		SharedPtr<Framebuffer> ShadowMap;
	};

	constexpr uint32_t MAX_DIRECTIONAL_LIGHTS = 1;

	struct DirectionalLight
	{
		Vector3 Direction;
		Color Color;
		float Intensity;
		bool CastsShadows;
		Matrix4 LightSpaceMatrix;

		// Temp?
		iVector2 ShadowMapSize;
		SharedPtr<Framebuffer> ShadowMap;
	};
}