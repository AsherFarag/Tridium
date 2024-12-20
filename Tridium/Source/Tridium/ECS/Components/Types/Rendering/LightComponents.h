#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Lights.h>

namespace Tridium {

	class Framebuffer;
	class CubeMap;

	class PointLightComponent : public Component
	{
		REFLECT( PointLightComponent );
	public:
		PointLightComponent() = default;
		PointLightComponent( const PointLightComponent& a_Other );
		PointLightComponent& operator=( const PointLightComponent& a_Other )
		{
			LightColor = a_Other.LightColor;
			Intensity = a_Other.Intensity;
			FalloffExponent = a_Other.FalloffExponent;
			AttenuationRadius = a_Other.AttenuationRadius;
			CastsShadows = a_Other.CastsShadows;
			ShadowMapSize = a_Other.ShadowMapSize;
			ShadowMap = nullptr;
			return *this;
		}

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float FalloffExponent = 2.0f;
		float AttenuationRadius = 10.0f;
		bool CastsShadows = true;

		// Temp?
		uint32_t ShadowMapSize = 256;
		SharedPtr<Framebuffer> ShadowMap;
	};

	class SpotLightComponent : public Component
	{
		REFLECT( SpotLightComponent );
	public:
		SpotLightComponent() = default;
		SpotLightComponent( const SpotLightComponent& a_Other )
			: LightColor( a_Other.LightColor ), Intensity( a_Other.Intensity ), FalloffExponent( a_Other.FalloffExponent ),
			AttenuationRadius( a_Other.AttenuationRadius ), InnerConeAngle( a_Other.InnerConeAngle ), OuterConeAngle( a_Other.OuterConeAngle ),
			CastsShadows( a_Other.CastsShadows ), ShadowMapSize( a_Other.ShadowMapSize ), ShadowMap( nullptr ) {}
		SpotLightComponent& operator=( const SpotLightComponent& a_Other )
		{
			LightColor = a_Other.LightColor;
			Intensity = a_Other.Intensity;
			FalloffExponent = a_Other.FalloffExponent;
			AttenuationRadius = a_Other.AttenuationRadius;
			InnerConeAngle = a_Other.InnerConeAngle;
			OuterConeAngle = a_Other.OuterConeAngle;
			CastsShadows = a_Other.CastsShadows;
			ShadowMapSize = a_Other.ShadowMapSize;
			ShadowMap = nullptr;
			return *this;
		}

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float FalloffExponent = 2.0f;
		float AttenuationRadius = 10.0f;
		float InnerConeAngle = 30.0f;
		float OuterConeAngle = 45.0f;
		bool CastsShadows = true;

		// Temp?
		iVector2 ShadowMapSize{ 1024, 1024 };
		SharedPtr<Framebuffer> ShadowMap;
	};

	class DirectionalLightComponent : public Component
	{
		REFLECT( DirectionalLightComponent );
	public:
		DirectionalLightComponent() = default;
		DirectionalLightComponent( const DirectionalLightComponent& a_Other )
			: LightColor( a_Other.LightColor ), Intensity( a_Other.Intensity ), CastsShadows( a_Other.CastsShadows ),
			ShadowMapSize( a_Other.ShadowMapSize ), ShadowMap( nullptr ) {}
		DirectionalLightComponent& operator=( const DirectionalLightComponent& a_Other )
		{
			LightColor = a_Other.LightColor;
			Intensity = a_Other.Intensity;
			CastsShadows = a_Other.CastsShadows;
			ShadowMapSize = a_Other.ShadowMapSize;
			ShadowMap = nullptr;
			return *this;
		}

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		bool CastsShadows = true;

		// Temp?
		iVector2 ShadowMapSize{ 1024 * 8, 1024 * 8 };
		SharedPtr<Framebuffer> ShadowMap;
	};

}