#ifndef Core_HLSLI
#define Core_HLSLI

#include "Platform.hlsli"
#include "Platform/DirectX12/RootSig.hlsli"
#include "ShaderInterop.h"

static const float PI = 3.14159265f;

#define CONSTANT_BUFFER( _Name, _Type, _Slot ) ConstantBuffer< _Type > _Name : register( b##_Slot )

#if defined( __PSSL__ )
	// Defined elsewhere
#elif defined( __spirv__ )
	#define INLINED_CONSTANTS( _Name, _Type ) ConstantBuffer< _Type > _Name : register( b0, INLINED_CONSTANTS_SPACE )
#else
	#define INLINED_CONSTANTS( _Name, _Type ) ConstantBuffer< _Type > _Name : register( b0, INLINED_CONSTANTS_SPACE )
#endif

#define STRUCTURED_BUFFER( _Name, _Type, _Slot ) StructuredBuffer< _Type > _Name : register( t##_Slot )

#define COMBINED_SAMPLER( _Name, _Type, _Slot ) \
	_Type _Name : register( t##_Slot ); \
	SamplerState _Name##_Sampler : register( s##_Slot )

#define GetCombinedSampler( _Texture ) _Texture##_Sampler

// Use for sampling combined samplers
#define SampleTexture( _Texture, _UV ) _Texture.Sample( GetCombinedSampler( _Texture ), _UV )
#define SampleTextureLod( _Texture, _UV, _Lod ) _Texture.SampleLevel( GetCombinedSampler( _Texture ), _UV, _Lod )

#endif // Core_HLSLI