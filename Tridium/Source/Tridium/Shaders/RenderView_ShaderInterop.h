#ifndef RenderView_ShaderInterop_H
#define RenderView_ShaderInterop_H

#include "ShaderInterop.h"

BEGIN_TRIDIUM_NAMESPACE

struct CPP_ALIGN_TO_HLSL RenderViewConstants
{
	float4x4 ViewMatrix           DEFAULT_VALUE( 1.0f );
	float4x4 ProjectionMatrix     DEFAULT_VALUE( 1.0f );
	float4x4 ViewProjectionMatrix DEFAULT_VALUE( 1.0f );
	float4   ViewPosition         DEFAULT_VALUE( 0.0f, 0.0f, 0.0f, 1.0f );
	float2   ViewportSize         DEFAULT_VALUE( 0.0f, 0.0f );
	float    FarPlane             DEFAULT_VALUE( 1000.0f );
	float    NearPlane            DEFAULT_VALUE( 0.1f );
};

END_TRIDIUM_NAMESPACE

#endif // RenderView_ShaderInterop_H