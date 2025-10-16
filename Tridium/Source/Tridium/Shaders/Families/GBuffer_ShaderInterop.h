#ifndef GBUFFER_SHADERINTEROP
#define GBUFFER_SHADERINTEROP

#include "ShaderInterop.h"
#include "RenderView_ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

struct CPP_ALIGN_TO_HLSL InlinedConstants_GBuffer
{
	float4x4 PVM;
	float4x4 Model;
};

END_TRIDIUM_NAMESPACE

#endif // !GBUFFER_SHADERINTEROP
