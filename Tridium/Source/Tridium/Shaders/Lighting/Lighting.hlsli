#ifndef Lighting_HLSLI
#define Lighting_HLSLI

#include "Core.hlsli"
#include "LightTypes_ShaderInterop.h"

// Computes a radial light attenuation within a given 'a_Radius', returning 0 if outside the radius.
// The attenuation follows a smooth falloff curve defined by the formula:
//     attenuation = a_MaxIntensity * (1 - (d/r)^2)^2 / (1 + a_Falloff * (d/r))
// where 'd' is the distance from the light source, 'r' is the radius, and 'a_Falloff' controls the steepness of the falloff.
float AttenuateCusp(float a_Distance, float a_Radius, float a_MaxIntensity, float a_Falloff)
{
    float s = a_Distance / a_Radius;
    
    // If the distance is greater than or equal to the radius, return 0 intensity
    if (s >= 1.0f)
        return 0.0f;

    // Use saturate to clamp the value between 0 and 1
    float t = saturate(1.0f - s * s);
    
    // Prevent division by zero by ensuring the denominator is at least a small positive value
    float denom = max(1e-4f, 1.0f + a_Falloff * s);
    
    // Calculate and return the attenuated intensity
    return a_MaxIntensity * (t * t) / denom;
}

float SpotLightFactor(float3 L, float3 SpotDirection, float SpotInnerCos, float SpotOuterCos)
{
    float cosTheta = dot(L, -SpotDirection);
    return 1 - saturate(smoothstep(SpotInnerCos, SpotOuterCos, cosTheta));
}

float SampleDirectionalShadowMap(
    COMBINED_COMP_SAMPLER_PARAM(ShadowMap, Texture2D),
    float4 LightSpacePos, float3 Normal, float3 LightDirection
)
{
    // Transform from clip space to texture space
    float3 proj = LightSpacePos.xyz / LightSpacePos.w;
    float2 uv = proj.xy * 0.5f + 0.5f;

    // No Z remap here — you already fixed it on the CPU
    float currentDepth = proj.z;

    // Outside of shadow map bounds
    if (any(uv < 0.0f) || any(uv > 1.0f))
        return 1.0f;

    // Bias to prevent shadow acne
    float bias = max(0.0005f * (1.0f - dot(Normal, LightDirection)), 0.0005f);

    // Hardware PCF sample
    float shadow = ShadowMap.SampleCmpLevelZero(
        GetCombinedSampler(ShadowMap),
        uv,
        currentDepth - bias
    );

    return shadow; // 1 = lit, 0 = shadowed
}



#endif // Lighting_HLSLI