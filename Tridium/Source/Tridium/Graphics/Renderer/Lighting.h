#pragma once
#include <Tridium/Containers/Array.h>
#include <Tridium/Graphics/Renderer/RenderResource.h>
#include <Tridium/Shaders/Lighting/LightTypes_ShaderInterop.h>

namespace Tridium {

	//=============================================================================================
	// Light ID: A unique identifier for lights in the scene.
	//=============================================================================================
	using LightID = int32_t;

	enum class ELightType : uint8_t
	{
		Unknown = 0,
		Directional,
		Point,
		Spot
	};

	//=============================================================================================
	// Light Mobility:
	//=============================================================================================
	enum class ELightMobility
	{
		Static = 0,
		Dynamic,
	};

	struct LightEnvironment
	{
		static constexpr size_t MaxDirectionalLights = 4;

		InlineArray<DirectionalLight, MaxDirectionalLights> DirectionalLights;
		Array<PointLight> PointLights;
		Array<SpotLight> SpotLights;

		UnorderedMap<LightID, RHITextureRef> DirectionalLightShadowMaps;
		UnorderedMap<LightID, RHITextureRef> PointLightShadowMaps;
		UnorderedMap<LightID, RHITextureRef> SpotLightShadowMaps;

		struct
		{
			RenderResourceEnvironmentMap EnvironmentMap;
			float Exposure = 1.0f;
			float Gamma = 2.2f;
			float Blur = 0.0f;
			float Intensity = 1.0f;
		} Sky;

		LightID AddDirectionalLight( const DirectionalLight& a_Light, RHITextureRef a_ShadowMap = nullptr )
		{
			if ( DirectionalLights.Size() >= MaxDirectionalLights )
				return -1;

			DirectionalLights.PushBack( a_Light );
			DirectionalLightShadowMaps[static_cast<LightID>(DirectionalLights.Size() - 1)] = a_ShadowMap;
			return static_cast<LightID>( DirectionalLights.Size() - 1 );
		}
	};

} // namespace Tridium