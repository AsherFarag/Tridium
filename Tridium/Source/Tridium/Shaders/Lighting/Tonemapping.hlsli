#ifndef Tonemapping_HLSLI
#define Tonemapping_HLSLI

#include "Core.hlsli"

float3 Tonemap_Reinhard(float3 a_Color)
{
    return a_Color / (a_Color + 1.0);
}

float3 Tonemap_ACESFilm(float3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// From: https://github.com/blender/blender/blob/fc08f7491e7eba994d86b610e5ec757f9c62ac81/release/datafiles/colormanagement/config.ocio#L358
static const float3x3 AgXInsetMatrix = float3x3(
    0.856627153315983, 0.0951212405381588, 0.0482516061458583,
    0.137318972929847, 0.761241990602591, 0.101439036467562,
    0.11189821299995, 0.0767994186031903, 0.811302368396859
);


// Converted to column major and inverted from https://github.com/EaryChow/AgX_LUT_Gen/blob/ab7415eca3cbeb14fd55deb1de6d7b2d699a1bb9/AgXBaseRec2020.py#L25
// https://github.com/google/filament/blob/bac8e58ee7009db4d348875d274daf4dd78a3bd1/filament/src/ToneMapper.cpp#L273-L278
static const float3x3 AgXOutsetMatrix = float3x3(
    0.899796955911611, 0.0871996192028351, 0.013003424885555,
    0.11142098895748, 0.875575586156966, 0.0130034248855548,
    0.11142098895748, 0.0871996192028349, 0.801379391839686
);

//static const float AgxMinEv = -12.47393;
//static const float AgxMaxEv = 4.026069;

// Biased towards identity matrix to reduce color shifts
//static const float3x3 AgXOutsetMatrix = float3x3(
//    0.95, 0.05, 0.00,
//    0.05, 0.95, 0.00,
//    0.05, 0.05, 0.90
//);

static const float AgxMinEv = -8.5;
static const float AgxMaxEv = 3.5;

float3 AGXCd1( float3 a_Color, float3 a_Slope, float3 a_Offset, float3 a_Power, float a_Saturation )
{
    // 1. agx
    // Input transform (inset)
    a_Color = mul(AgXInsetMatrix, a_Color);
    
    // From Filament: Avoid 0 or negative values for log2
    a_Color = max(a_Color, float3(1e-10, 1e-10, 1e-10));

    // Logarithmic compression
    a_Color = clamp(log2(a_Color), AgxMinEv, AgxMaxEv);
    a_Color = (a_Color - AgxMinEv) / (AgxMaxEv - AgxMinEv);

     // From Filament: Avoid 0 or negative values for pow
    a_Color = clamp(a_Color, 0.0, 1.0);
    
    // Apply sigmoid function approximation
    const float3 color2 = a_Color * a_Color;
    const float3 color4 = color2 * color2;
    a_Color = + 15.5   * color4 * color2
              - 40.14  * color4 * a_Color
              + 31.96  * color4
              - 6.868  * color2 * a_Color
              + 0.4298 * color2
              + 0.1191 * a_Color
              - 0.00232;
    
    // 2. slope, offset, power
    a_Color = pow(a_Color * a_Slope + a_Offset, a_Power);
    const float3 lumaw = float3(0.2126, 0.7152, 0.0722);
    float luma = dot(a_Color, lumaw);
    a_Color = luma + a_Saturation * (a_Color - luma);
    
    // 3. outset
    // Output transform (outset)
    a_Color = mul(AgXOutsetMatrix, a_Color);
    
    // Gamut clamp
    a_Color = clamp(a_Color, 0.0, 1.0);

    return a_Color;
}

float3 Tonemap_AGX(float3 a_Color)
{
    const float3 Slope = float3(1.0, 1.0, 1.0);
    const float3 Offset = float3(0.0, 0.0, 0.0);
    const float3 Power = float3(1.0, 1.0, 1.0);
    const float Saturation = 1.3;

    return AGXCd1(a_Color, Slope, Offset, Power, Saturation);
    //return Tonemap_Reinhard(a_Color);
}

#endif // Tonemapping_HLSLI