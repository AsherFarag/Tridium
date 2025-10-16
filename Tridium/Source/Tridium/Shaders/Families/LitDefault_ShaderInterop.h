#ifndef LITDEFAULT_SHADERINTEROP
#define LITDEFAULT_SHADERINTEROP

#include "ShaderInterop.h"
#include "RenderView_ShaderInterop.h"
#include "Lighting/LightTypes_ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

	struct CPP_ALIGN_TO_HLSL InlinedConstants_LitDefault
	{
		uint NumPointLights;
		uint NumSpotLights;
		DirectionalLight DirectionalLight;
	};

END_TRIDIUM_NAMESPACE

#ifndef __cplusplus

CONSTANT_BUFFER( u_RenderView, RenderViewConstants, 0 );

#endif // __cplusplus

#endif // !LITDEFAULT_SHADERINTEROP