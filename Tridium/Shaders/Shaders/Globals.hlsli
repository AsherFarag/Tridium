#pragma once
#include "Platform.hlsli"
#include "Platform/DirectX12/RootSig.hlsli"
#include "ShaderInterop.h"

#if PLATFORM_SUPPORTS_STATIC_SAMPLERS

// Default Static samplers that do not need to be bound
// Must match the static samplers in the root signature defined in D3D12CommandList.cpp
SamplerState StaticPointWrappedSampler       : register(s0, space1000);
SamplerState StaticPointClampedSampler       : register(s1, space1000);
SamplerState StaticBilinearWrappedSampler    : register(s2, space1000);
SamplerState StaticBilinearClampedSampler    : register(s3, space1000);
SamplerState StaticTrilinearWrappedSampler   : register(s4, space1000);
SamplerState StaticTrilinearClampedSampler   : register(s5, space1000);

#define GetStaticSampler( _Filter, _WrapMode ) Static##_Filter##_WrapMode##Sampler

#endif // PLATFORM_SUPPORTS_STATIC_SAMPLERS