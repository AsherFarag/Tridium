#ifndef LightTypes_ShaderInterop_H
#define LightTypes_ShaderInterop_H

#include "ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

//=================================================================================================
// Directional Light: A light that has a direction but no specific position, simulating sunlight.
//=================================================================================================
struct CPP_ALIGN_TO_HLSL DirectionalLight
{
	// The direction the light is pointing in (should be normalized).
	float3 Direction    DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The intensity of the light, affecting its brightness.
	float Intensity     DEFAULT_VALUE( 0.0f );
	// The color of the light, represented as RGB values.
	float3 Color        DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// A scale factor for specular highlights produced by the light.
	// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
	float SpecularScale DEFAULT_VALUE( 1.0f );
};

//=================================================================================================
// Point Light: A light that emits light uniformly in all directions from a single point in space.
//=================================================================================================
struct CPP_ALIGN_TO_HLSL PointLight
{
	// The position of the light in 3D space.
	float3 Position		DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The intensity of the light, affecting its brightness.
	float Intensity		DEFAULT_VALUE( 0.0f );
	// The color of the light, represented as RGB values.
	float3 Color		DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The minimum radius for light attenuation calculations.
	float MinRadius		DEFAULT_VALUE( 0.001f );
	// The maximum radius for light attenuation calculations.
	float Radius		DEFAULT_VALUE( 25.0f );
	// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
	float Falloff		DEFAULT_VALUE( 1.f );
	// The size of the light source, affecting soft shadow calculations.
	float SourceSize	DEFAULT_VALUE( 0.1f );
	// A scale factor for specular highlights produced by the light.
	// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
	float SpecularScale DEFAULT_VALUE( 1.0f );
};

//=================================================================================================
// Spot Light: A light that emits light in a specific direction within a cone,
// simulating a flashlight or spotlight.
//=================================================================================================
struct CPP_ALIGN_TO_HLSL SpotLight
{
	// The position of the light in 3D space.
	float3 Position        DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The intensity of the light, affecting its brightness.
	float Intensity        DEFAULT_VALUE( 0.0f );
	// The direction the light is pointing in (should be normalized).
	float3 Direction       DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The attenuation factor based on the angle between the light's direction and the point being lit.
	float AngleAttenuation DEFAULT_VALUE( 0.0f );
	// The color of the light, represented as RGB values.
	float3 Color           DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
	// The minimum radius for light attenuation calculations.
	float Range            DEFAULT_VALUE( 0.1f );
	// The angle (in radians) defining the cone of the spotlight.
	float Angle            DEFAULT_VALUE( 0.0f );
	// The falloff exponent for light attenuation, controlling how quickly the light diminishes with distance.
	float Falloff          DEFAULT_VALUE( 1.0f );
	// A scale factor for specular highlights produced by the light.
	// NOTE: Any value other than 1.0f will break energy conservation and is not physically accurate.
	float SpecularScale    DEFAULT_VALUE( 1.0f );

	float _Padding;
};

END_TRIDIUM_NAMESPACE

#endif // LightTypes_ShaderInterop_H