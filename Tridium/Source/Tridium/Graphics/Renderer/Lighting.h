#pragma once
#include <Tridium/Containers/Array.h>
#include <Tridium/Graphics/Renderer/RenderResource.h>
#include <Tridium/Shaders/Lighting/LightTypes_ShaderInterop.h>

namespace Tridium {

	constexpr size_t c_MaxDirectionLights = 4;

	enum class ELightType : uint8_t
	{
		Unknown = 0,
		Directional,
		Point,
		Spot
	};

	struct LightEnvironment
	{
		InlineArray<DirectionalLight, c_MaxDirectionLights> DirectionalLights;
		Array<PointLight> PointLights;
		Array<SpotLight> SpotLights;

		struct
		{
			RenderResourceEnvironmentMap EnvironmentMap;
		} Sky;
	};

}