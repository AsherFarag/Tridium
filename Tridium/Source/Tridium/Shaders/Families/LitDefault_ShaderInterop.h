#ifndef LITDEFAULT_SHADERINTEROP
#define LITDEFAULT_SHADERINTEROP

#include "../ShaderInterop.h"

#ifdef __cplusplus
namespace Tridium {
#endif

	struct CPP_ALIGN_TO_HLSL InlinedConstants_LitDefault
	{
		float3 CameraPosition;
		uint NumPointLights;
	};

	struct CPP_ALIGN_TO_HLSL DirectionalLight
	{
		float3 Direction    DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float3 Radiance     DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float Intensity     DEFAULT_VALUE( 0.0f );
		float ShadowAmount  DEFAULT_VALUE( 1.0f );
	};

	struct CPP_ALIGN_TO_HLSL PointLight
	{
		float3 Position		DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float Intensity		DEFAULT_VALUE( 0.0f );
		float3 Radiance		DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float MinRadius		DEFAULT_VALUE( 0.001f );
		float Radius		DEFAULT_VALUE( 25.0f );
		float Falloff		DEFAULT_VALUE( 1.f );
		float SourceSize	DEFAULT_VALUE( 0.1f );
		float _Padding		DEFAULT_VALUE( 0.0f );
	};

	struct CPP_ALIGN_TO_HLSL SpotLight
	{
		float3 Position        DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float Intensity        DEFAULT_VALUE( 0.0f );
		float3 Direction       DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float AngleAttenuation DEFAULT_VALUE( 0.0f );
		float3 Radiance        DEFAULT_VALUE( 0.0f, 0.0f, 0.0f );
		float Range            DEFAULT_VALUE( 0.1f );
		float Angle            DEFAULT_VALUE( 0.0f );
		float Falloff          DEFAULT_VALUE( 1.0f );
	};

#if __cplusplus
} // namespace Tridium
#endif // __cplusplus

#endif // !LITDEFAULT_SHADERINTEROP