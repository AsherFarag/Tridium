#ifndef GGX_HLSLI
#define GGX_HLSLI

#include "Core.hlsli"

//======================================================================
// GGX area light approximation from Horizon Zero Dawn
//======================================================================

static const float GGX_POINT_LIGHT_RADIUS_TAN = 0.001;                     // Tangent of the angular radius of a point light (radius / distance to surface point)
static const float GGX_SPOT_LIGHT_RADIUS_TAN = GGX_POINT_LIGHT_RADIUS_TAN; // Tangent of the angular radius of a spot light (radius / distance to surface point)
static const float GGX_DIR_LIGHT_RADIUS_TAN = 0.0092;                      // Tangent of the angular radius of a directional light (radius / distance to surface point)

// Estimates how much the half-angle vector H between the view direction a_V and light direction a_L
// @param a_RadiusTan: tangent of the light's angular radius (radius / distance to surface point)
// @param a_NoL: dot(a_N, a_L) where a_L is the (unblurred) light direction (cosine between normal and light direction)
// @param a_NoV: dot(a_N, a_V) where a_V is the view direction (cosine between normal and view direction)
// @param a_VoL: dot(a_V, a_L) (cosine between view and light direction)
// @return: The result is an approximation of (a_N·H)^2 clamped to [0,1]. 
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
    const float NoLVTr = a_NoL * radiusCos + a_NoV + NoTr, a_VoLVTr = a_VoL * radiusCos + 1.0 + VoTr;
    const float p = NoBr * a_VoLVTr, q = NoLVTr * a_VoLVTr, s = VoBr * NoLVTr;
    const float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
    const float xDenom = p * p + s * ((s - 2.0 * p)) + NoLVTr * ((a_NoL * radiusCos + a_NoV) * a_VoLVTr * a_VoLVTr +
                   q * (-0.5 * (a_VoLVTr + a_VoL * radiusCos) - 0.5));
    const float twoX1 = 2.0 * xNum / (xDenom * xDenom + xNum * xNum);
    
    const float sinTheta = twoX1 * xDenom;
    const float cosTheta = 1.0 - twoX1 * xNum;
    NoTr = cosTheta * NoTr + sinTheta * NoBr;
    VoTr = cosTheta * VoTr + sinTheta * VoBr;

    const float newNoL = a_NoL * radiusCos + NoTr;
    const float newa_VoL = a_VoL * radiusCos + VoTr;
    const float NoH = a_NoV + newNoL;
    const float HoH = 2.0 * newa_VoL + 2.0;
    
    return clamp(NoH * NoH / HoH, 0.0, 1.0);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX specular BRDF for different light types
// @param a_Albedo: base color of the material
// @param a_Roughness: surface a_Roughness [0,1]
// @param a_Metallic: a_Metallic factor [0,1]
// @param a_N: normal vector at the surface point
// @param a_V: view direction (towards the camera)
// @param a_L: light direction (towards the light source)
// @param a_NoL: max(dot(a_N, a_L), 0)
// @param a_VoL: dot(a_V, a_L)
// @param a_RadiusTan: tangent of the light's angular radius (radius / distance to surface point)
// @return: specular reflection color
float3 LightGGX(float3 a_Albedo, float a_Roughness, float a_Metallic, float3 a_N, float3 a_V, float3 a_L, float a_NoL, float a_VoL, float a_RadiusTan)
{
    // Clamp a_Roughness to avoid singularities
    a_Roughness = max(a_Roughness, 0.05);

    // Half-vector
    const float3 H = normalize(a_V + a_L);
    const float NoH = max(dot(a_N, H), 0.0);
    const float VoH = max(dot(a_V, H), 0.0);
    const float NoV = max(dot(a_N, a_V), 0.001);
    const float NoL = max(dot(a_N, a_L), 0.001);

    // Area-light approximation
#ifdef GGX_HIGH_QUALITY
    const float NoH2 = GetNoHSquared(a_RadiusTan, a_NoL, NoV, a_VoL);
#else
    const float NoH2 = NoH * NoH; // standard GGX
#endif

    // GGX Normal Distribution Function
    const float alpha = a_Roughness * a_Roughness;
    const float alpha2 = alpha * alpha;
    const float denom = NoH2 * (alpha2 - 1.0) + 1.0;
    const float D = alpha2 / (PI * denom * denom);

    // Geometry (Smith GGX)
    const float k = (a_Roughness + 1.0f) * (a_Roughness + 1.0f) / 8.0f;
    const float Gv = NoV / (NoV * (1.0 - k) + k);
    const float Gl = NoL / (NoL * (1.0 - k) + k);
    const float G = Gv * Gl;

    // Fresnel
    const float3 F0 = lerp(float3(0.04, 0.04, 0.04), a_Albedo, a_Metallic);
    const float3 F = FresnelSchlick(VoH, F0);

    // Specular term
    float3 spec = (D * G * F) / max(4.0 * NoV * NoL, 0.001);

    // Energy compensation (to match UE / Unity style)
    spec = spec / (0.125 * spec + 1.0);

    return spec;
}

// See 'LightGGX' for parameter descriptions
float3 PointLightGGX(float3 a_Albedo, float a_Roughness, float a_Metallic, float3 a_N, float3 a_V, float3 a_L, float a_NoL, float a_VoL)
{
    return LightGGX(a_Albedo, a_Roughness, a_Metallic, a_N, a_V, a_L, a_NoL, a_VoL, GGX_POINT_LIGHT_RADIUS_TAN);
}

// See 'LightGGX' for parameter descriptions
float3 SpotLightGGX(float3 a_Albedo, float a_Roughness, float a_Metallic, float3 a_N, float3 a_V, float3 a_L, float a_NoL, float a_VoL)
{
    return LightGGX(a_Albedo, a_Roughness, a_Metallic, a_N, a_V, a_L, a_NoL, a_VoL, GGX_SPOT_LIGHT_RADIUS_TAN);
}

// See 'LightGGX' for parameter descriptions
float3 DirLightGGX(float3 a_Albedo, float a_Roughness, float a_Metallic, float3 a_N, float3 a_V, float3 a_L, float a_NoL, float a_VoL)
{
    return LightGGX(a_Albedo, a_Roughness, a_Metallic, a_N, a_V, a_L, a_NoL, a_VoL, GGX_DIR_LIGHT_RADIUS_TAN);
}

#endif // GGX_HLSLI