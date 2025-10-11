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

#endif // Lighting_HLSLI