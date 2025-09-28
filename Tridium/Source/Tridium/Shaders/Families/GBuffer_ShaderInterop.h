#ifndef GBUFFER_SHADERINTEROP
#define GBUFFER_SHADERINTEROP

#include "../ShaderInterop.h"

#ifdef __cplusplus
namespace Tridium {
#endif

struct CPP_ALIGN_TO_HLSL InlinedConstants_GBuffer
{
	float4x4 PVM;
	float4x4 Model;
};

#if __cplusplus
} // namespace Tridium
#endif // __cplusplus

#endif // !GBUFFER_SHADERINTEROP
