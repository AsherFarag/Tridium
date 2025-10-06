#ifndef SKYBOX_SHADERINTEROP
#define SKYBOX_SHADERINTEROP

#include "../ShaderInterop.h"

#ifdef __cplusplus
namespace Tridium {
#endif

	struct CPP_ALIGN_TO_HLSL InlinedConstants_SkyBox
	{
		float4x4 Projection;
		float4x4 View;
	};

#if __cplusplus
} // namespace Tridium
#endif // __cplusplus

#endif // !SKYBOX_SHADERINTEROP