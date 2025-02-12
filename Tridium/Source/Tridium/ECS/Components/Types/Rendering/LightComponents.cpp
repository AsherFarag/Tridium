#include "tripch.h"
#include "LightComponents.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( PointLightComponent, Scriptable )
		BASE( NativeScriptComponent )
		PROPERTY( LightColor, Serialize | EditAnywhere )
		PROPERTY( Intensity, Serialize | EditAnywhere )
		PROPERTY( FalloffExponent, Serialize | EditAnywhere )
		PROPERTY( AttenuationRadius, Serialize | EditAnywhere )
		PROPERTY( CastsShadows, Serialize | EditAnywhere )
		PROPERTY( ShadowMapSize, Serialize | EditAnywhere )
		PROPERTY( ShadowMap, VisibleAnywhere )
	END_REFLECT_COMPONENT( PointLightComponent );

	
	BEGIN_REFLECT_COMPONENT( SpotLightComponent, Scriptable )
		BASE( NativeScriptComponent )
		PROPERTY( LightColor,       Serialize | EditAnywhere )
		PROPERTY( Intensity,        Serialize | EditAnywhere )
		PROPERTY( FalloffExponent,  Serialize | EditAnywhere )
		PROPERTY( AttenuationRadius,Serialize | EditAnywhere )
		PROPERTY( InnerConeAngle,   Serialize | EditAnywhere )
		PROPERTY( OuterConeAngle,   Serialize | EditAnywhere )
		PROPERTY( CastsShadows,		Serialize | EditAnywhere )
		PROPERTY( ShadowMapSize,	Serialize | EditAnywhere )
		PROPERTY( ShadowMap,		VisibleAnywhere )
	END_REFLECT_COMPONENT( SpotLightComponent );

	BEGIN_REFLECT_COMPONENT( DirectionalLightComponent, Scriptable )
		BASE( NativeScriptComponent )
		PROPERTY( LightColor,    Serialize | EditAnywhere )
		PROPERTY( Intensity,     Serialize | EditAnywhere )
		PROPERTY( CastsShadows,  Serialize | EditAnywhere )
		PROPERTY( ShadowMapSize, Serialize | EditAnywhere )
		PROPERTY( ShadowMap,     VisibleAnywhere )
	END_REFLECT_COMPONENT( DirectionalLightComponent );

	PointLightComponent::PointLightComponent( const PointLightComponent& a_Other )
		: LightColor( a_Other.LightColor ), Intensity( a_Other.Intensity ), FalloffExponent( a_Other.FalloffExponent ),
		AttenuationRadius( a_Other.AttenuationRadius ), CastsShadows( a_Other.CastsShadows ), ShadowMapSize( a_Other.ShadowMapSize ), ShadowMap( nullptr ) {}
}
