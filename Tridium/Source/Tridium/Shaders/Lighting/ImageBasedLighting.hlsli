#ifndef ImageBasedLighting_HLSLI
#define ImageBasedLighting_HLSLI

#include "Core.hlsli"
#include "BRDF.hlsli"

float2 ApproxBRDF(float NdotV, float roughness)
{
    // Approximation of the integrated BRDF used by UE4 (no LUT)
    const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
    const float4 c1 = float4(1.0, 0.0425, 1.04, -0.04);
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    return float2(-1.04, 1.04) * a004 + r.zw;
}

float3 EvaluateIBL(float3 Albedo, float Roughness, float Metallic, float3 N, float3 V,
                   COMBINED_SAMPLER_PARAM(IrradianceMap, TextureCube),
                   COMBINED_SAMPLER_PARAM(RadianceMap, TextureCube))
{
    uint mipLevel; uint width; uint height; uint mipCount;
    RadianceMap.GetDimensions(mipLevel, width, height, mipCount);

    const float3 F0 = FresnelSchlickRoughness(saturate(dot(N, V)), lerp(float3(0.04, 0.04, 0.04), Albedo, Metallic), Roughness);
    const float NdotV = saturate(dot(N, V));

    // --- Diffuse IBL ---
    const float3 irradiance = SampleTexture(IrradianceMap, N).rgb;
    const float3 kD = (1.0 - F0) * (1.0 - Metallic);
    // Lambertian diffuse
    const float3 diffuseIBL = kD * irradiance * (Albedo / PI);

    // --- Specular IBL ---
    const float3 R = reflect(-V, N);
    const float roughnessLevel = Roughness * Roughness * mipCount;
    const float3 prefiltered = SampleTextureLod(RadianceMap, R, roughnessLevel).rgb;

    // TODO: Replace with BRDF LUT sampling
    const float2 brdf = ApproxBRDF(NdotV, Roughness);
    const float3 specularIBL = prefiltered * (F0 * brdf.x + brdf.y);

    // --- Final contribution ---
    return diffuseIBL + specularIBL;
}

#endif // ImageBasedLighting_HLSLI