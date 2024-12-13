#pragma once
#include <Tridium/ECS/Components/Component.h>

// Temp?
#include <Tridium/Rendering/FrameBuffer.h>

namespace Tridium {

	class PointLightComponent : public Component
	{
		REFLECT( PointLightComponent );
	public:
		PointLightComponent() = default;

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float FalloffExponent = 2.0f;
		float AttenuationRadius = 10.0f;
		bool CastsShadows = true;

		// Temp?
		SharedPtr<Framebuffer> ShadowMap;
	};

	BEGIN_REFLECT_COMPONENT( PointLightComponent )
		PROPERTY( LightColor,        FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Intensity,         FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( FalloffExponent,   FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( AttenuationRadius, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( CastsShadows,      FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( ShadowMap,		 FLAGS( VisibleAnywhere ) )
	END_REFLECT( PointLightComponent );

	class SpotLightComponent : public Component
	{
		REFLECT( SpotLightComponent );
	public:
		SpotLightComponent() = default;

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		float FalloffExponent = 2.0f;
		float AttenuationRadius = 10.0f;
		float InnerConeAngle = 30.0f;
		float OuterConeAngle = 45.0f;
	};

	BEGIN_REFLECT_COMPONENT( SpotLightComponent )
		PROPERTY( LightColor,        FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Intensity,         FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( FalloffExponent,   FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( AttenuationRadius, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( InnerConeAngle,    FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( OuterConeAngle,    FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( SpotLightComponent );

	class DirectionalLightComponent : public Component
	{
		REFLECT( DirectionalLightComponent );
	public:
		DirectionalLightComponent() = default;

		Color LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
		bool CastsShadows = true;

		// Temp?
		SharedPtr<Framebuffer> ShadowMap;
	};

	BEGIN_REFLECT_COMPONENT( DirectionalLightComponent )
		PROPERTY( LightColor,   FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Intensity,    FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( CastsShadows, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( ShadowMap,    FLAGS( VisibleAnywhere ) )
	END_REFLECT( DirectionalLightComponent );

}