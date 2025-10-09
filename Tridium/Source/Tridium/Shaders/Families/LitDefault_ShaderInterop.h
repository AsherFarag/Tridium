#ifndef LITDEFAULT_SHADERINTEROP
#define LITDEFAULT_SHADERINTEROP

#include "ShaderInterop.h"
#include "Lighting/LightTypes_ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

	struct CPP_ALIGN_TO_HLSL InlinedConstants_LitDefault
	{
		float3 CameraPosition;
		uint NumPointLights;
		uint NumSpotLights;
		DirectionalLight DirectionalLight;
	};

END_TRIDIUM_NAMESPACE

#endif // !LITDEFAULT_SHADERINTEROP