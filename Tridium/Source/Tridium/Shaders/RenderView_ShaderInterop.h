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

#ifdef __cplusplus

	RenderViewConstants() = default;
	RenderViewConstants( const float4x4& a_ViewMatrix, const float4x4& a_ProjectionMatrix, const float4& a_ViewPosition,
						 const float2& a_ViewportSize, float a_FarPlane = 1000.0f, float a_NearPlane = 0.1f ) 
		: ViewMatrix( a_ViewMatrix ),
		  ProjectionMatrix( a_ProjectionMatrix ),
		  ViewProjectionMatrix( a_ProjectionMatrix* a_ViewMatrix ),
		  ViewPosition( a_ViewPosition ),
		  ViewportSize( a_ViewportSize ),
		  FarPlane( a_FarPlane ),
		  NearPlane( a_NearPlane )
	{}

#endif // __cplusplus
};

END_TRIDIUM_NAMESPACE

#endif // RenderView_ShaderInterop_H