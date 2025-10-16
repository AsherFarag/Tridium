#ifndef DebugLightCaster_ShaderInterop_H
#define DebugLightCaster_ShaderInterop_H

#include "ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

struct CPP_ALIGN_TO_HLSL InlinedConstants_DebugLightCaster
{
	float4x4 PVM;
	float4 Color;
};

END_TRIDIUM_NAMESPACE

#endif // DebugLightCaster_ShaderInterop_H