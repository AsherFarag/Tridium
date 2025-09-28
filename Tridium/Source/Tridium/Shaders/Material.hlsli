#ifndef MATERIAL_HLSLI
#define MATERIAL_HLSLI

#include "Globals.hlsli"
#include "ShaderInterop.h"

struct MaterialProperties
{
    float4 AlbedoColor;
    float MetallicIntensity;
    float RoughnessIntensity;
    float EmissiveIntensity;
    float Padding;
};

#endif // MATERIAL_HLSLI