#ifndef GGX_HLSLI
#define GGX_HLSLI

#include "Core.hlsli"

//======================================================================
// GGX area light approximation from Horizon Zero Dawn
//======================================================================

static const float GGX_POINT_LIGHT_RADIUS_TAN = 0.001;                     // Tangent of the angular radius of a point light (radius / distance to surface point)
static const float GGX_SPOT_LIGHT_RADIUS_TAN = GGX_POINT_LIGHT_RADIUS_TAN; // Tangent of the angular radius of a spot light (radius / distance to surface point)
static const float GGX_DIR_LIGHT_RADIUS_TAN = 0.0092;                      // Tangent of the angular radius of a directional light (radius / distance to surface point)

// Estimates how much the half-angle vector H between the view direction V and light direction L
// @param a_RadiusTan: tangent of the light's angular radius (radius / distance to surface point)
// @param a_NoL: dot(N, L) where L is the (unblurred) light direction (cosine between normal and light direction)
// @param a_NoV: dot(N, V) where V is the view direction (cosine between normal and view direction)
// @param a_VoL: dot(V, L) (cosine between view and light direction)
// @return: The result is an approximation of (N·H)^2 clamped to [0,1]. 
float GetNoHSquared(float a_RadiusTan, float a_NoL, float a_NoV, float a_VoL)
{
    // Handle the case where the light is a point light (or very small)
    const float radiusCos = 1.0 / sqrt(1.0 + a_RadiusTan * a_RadiusTan);
    const float RoL = 2.0 * a_NoL * a_NoV - a_VoL;
    if (RoL >= radiusCos)
        return 1.0; // Full specular highlight
    
    // Handle the case where the light is very large (or very close)
    const float rOverLengthT = radiusCos * a_RadiusTan / sqrt(1.0 - RoL * RoL);
    float NoTr = rOverLengthT * (a_NoV - RoL * a_NoL);
    float VoTr = rOverLengthT * (2.0 * a_NoV * a_NoV - 1.0 - RoL * a_VoL);

    const float triple = sqrt(clamp(1.0 - a_NoL * a_NoL - a_NoV * a_NoV - a_VoL * a_VoL + 2.0 * a_NoL * a_NoV * a_VoL, 0.0, 1.0));

    const float NoBr = rOverLengthT * triple, VoBr = rOverLengthT * (2.0 * triple * a_NoV);
    const float NoLVTr = a_NoL * radiusCos + a_NoV + NoTr, VoLVTr = a_VoL * radiusCos + 1.0 + VoTr;
    const float p = NoBr * VoLVTr, q = NoLVTr * VoLVTr, s = VoBr * NoLVTr;
    const float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
    const float xDenom = p * p + s * ((s - 2.0 * p)) + NoLVTr * ((a_NoL * radiusCos + a_NoV) * VoLVTr * VoLVTr +
                   q * (-0.5 * (VoLVTr + a_VoL * radiusCos) - 0.5));
    const float twoX1 = 2.0 * xNum / (xDenom * xDenom + xNum * xNum);
    
    const float sinTheta = twoX1 * xDenom;
    const float cosTheta = 1.0 - twoX1 * xNum;
    NoTr = cosTheta * NoTr + sinTheta * NoBr;
    VoTr = cosTheta * VoTr + sinTheta * VoBr;

    const float newNoL = a_NoL * radiusCos + NoTr;
    const float newVoL = a_VoL * radiusCos + VoTr;
    const float NoH = a_NoV + newNoL;
    const float HoH = 2.0 * newVoL + 2.0;
    
    return clamp(NoH * NoH / HoH, 0.0, 1.0);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Specular reflection using the GGX microfacet model with an area light approximation.
// Note: This function assumes that the input vectors are normalized and that the normal is not facing away from the light.
// @param a_LightRadiusTan: tangent of the light's angular radius (radius / distance to surface point)
// @param a_NormalM: surface normal in model space (should be normalized)
// @param a_ViewPos: view direction in model space (should be normalized and point towards the camera)
// @param a_LightVec: light direction in model space (should be normalized and point towards the light)
// @param a_NdotLmax0: sqrt(max(dot(N, L), 0)) where L is the (unblurred) light direction (cosine between normal and light direction)
// @param a_SmoothnessG: smoothness value in [0,1] where 1 is perfectly smooth
// @return: specular reflection intensity
float3 LightGGX(float3 albedo, float roughness, float metallic, float3 N, float3 V, float3 L, float NdotLmax0, float VoL, float radiusTan)
{
    // Clamp roughness to avoid singularities
    roughness = max(roughness, 0.05);

    // Half-vector
    float3 H = normalize(V + L);
    float NdotH = max(dot(N, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);
    float NdotV = max(dot(N, V), 0.001);
    float NdotL = max(dot(N, L), 0.001);

    // Area-light approximation
#ifdef GGX_HIGH_QUALITY
    float NoH2 = GetNoHSquared(radiusTan, NdotLmax0, NdotV, VoL);
#else
    float NoH2 = NdotH * NdotH; // standard GGX
#endif

    // GGX Normal Distribution Function
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NoH2 * (alpha2 - 1.0) + 1.0;
    float D = alpha2 / (PI * denom * denom);

    // Geometry (Smith GGX)
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    float Gv = NdotV / (NdotV * (1.0 - k) + k);
    float Gl = NdotL / (NdotL * (1.0 - k) + k);
    float G = Gv * Gl;

    // Fresnel
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float3 F = FresnelSchlick(VdotH, F0);

    // Specular term
    float3 spec = (D * G * F) / max(4.0 * NdotV * NdotL, 0.001);

    // Optional energy compensation (to match UE / Unity style)
    spec = spec / (0.125 * spec + 1.0);

    return spec;
}

float3 PointLightGGX(float3 albedo, float roughness, float metallic, float3 N, float3 V, float3 L, float NdotLmax0, float VoL)
{
    return LightGGX(albedo, roughness, metallic, N, V, L, NdotLmax0, VoL, GGX_POINT_LIGHT_RADIUS_TAN);
}

float3 SpotLightGGX(float3 albedo, float roughness, float metallic, float3 N, float3 V, float3 L, float NdotLmax0, float VoL)
{
    return LightGGX(albedo, roughness, metallic, N, V, L, NdotLmax0, VoL, GGX_SPOT_LIGHT_RADIUS_TAN);
}

float3 DirLightGGX(float3 albedo, float roughness, float metallic, float3 N, float3 V, float3 L, float NdotLmax0, float VoL)
{
    return LightGGX(albedo, roughness, metallic, N, V, L, NdotLmax0, VoL, GGX_DIR_LIGHT_RADIUS_TAN);
}

#endif // GGX_HLSLI