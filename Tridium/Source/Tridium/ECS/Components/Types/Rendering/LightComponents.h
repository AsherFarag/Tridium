#pragma once
#include <Tridium/ECS/Components/NativeScriptComponent.h>
#include <Tridium/Graphics/oldRendering/Lights.h>

namespace Tridium {

	class Framebuffer;
	class CubeMap;

	class OldPointLightComponent : public NativeScriptComponent
	{
		REFLECT( OldPointLightComponent );
	public:
		OldPointLightComponent() = default;
		OldPointLightComponent( const OldPointLightComponent& a_Other );
		OldPointLightComponent& operator=( const OldPointLightComponent& a_Other )
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

		Color4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float FalloffExponent = 2.0f;
		float AttenuationRadius = 10.0f;
		bool CastsShadows = true;

		// Temp?
		uint32_t ShadowMapSize = 256;
		SharedPtr<Framebuffer> ShadowMap;
	};

	class OldSpotLightComponent : public NativeScriptComponent
	{
		REFLECT( OldSpotLightComponent );
	public:
		OldSpotLightComponent() = default;
		OldSpotLightComponent( const OldSpotLightComponent& a_Other )
			: LightColor( a_Other.LightColor ), Intensity( a_Other.Intensity ), FalloffExponent( a_Other.FalloffExponent ),
			AttenuationRadius( a_Other.AttenuationRadius ), InnerConeAngle( a_Other.InnerConeAngle ), OuterConeAngle( a_Other.OuterConeAngle ),
			CastsShadows( a_Other.CastsShadows ), ShadowMapSize( a_Other.ShadowMapSize ), ShadowMap( nullptr ) {}
		OldSpotLightComponent& operator=( const OldSpotLightComponent& a_Other )
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

		Color4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
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

	class OldDirectionalLightComponent : public NativeScriptComponent
	{
		REFLECT( OldDirectionalLightComponent );
	public:
		OldDirectionalLightComponent() = default;
		OldDirectionalLightComponent( const OldDirectionalLightComponent& a_Other )
			: LightColor( a_Other.LightColor ), Intensity( a_Other.Intensity ), CastsShadows( a_Other.CastsShadows ),
			ShadowMapSize( a_Other.ShadowMapSize ), ShadowMap( nullptr ) {}
		OldDirectionalLightComponent& operator=( const OldDirectionalLightComponent& a_Other )
		{
			LightColor = a_Other.LightColor;
			Intensity = a_Other.Intensity;
			CastsShadows = a_Other.CastsShadows;
			ShadowMapSize = a_Other.ShadowMapSize;
			ShadowMap = nullptr;
			return *this;
		}

		Color4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		bool CastsShadows = true;

		// Temp?
		iVector2 ShadowMapSize{ 1024 * 8, 1024 * 8 };
		SharedPtr<Framebuffer> ShadowMap;
	};

}