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

#ifndef __cplusplus

CONSTANT_BUFFER(u_MaterialProps, MaterialProperties, 0);

#endif // __cplusplus

#endif // MATERIAL_HLSLI