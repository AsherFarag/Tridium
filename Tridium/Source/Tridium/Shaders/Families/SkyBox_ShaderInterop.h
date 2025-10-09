#ifndef SKYBOX_SHADERINTEROP
#define SKYBOX_SHADERINTEROP

#include "ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

struct CPP_ALIGN_TO_HLSL InlinedConstants_SkyBox
{
	float4x4 Projection;
	float4x4 View;
};

END_TRIDIUM_NAMESPACE

#endif // !SKYBOX_SHADERINTEROP