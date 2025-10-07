#ifndef SHADERINTEROP_HLSLI
#define SHADERINTEROP_HLSLI

#ifdef __cplusplus

#include <Tridium/Math/Math.h>
#include <Tridium/IO/FilePath.h>

#define CPP_ALIGN_TO_HLSL alignas(16)
#define INLINED_CONSTANTS_SPACE 9999
#define DEFAULT_VALUE( ... ) = ( __VA_ARGS__ )

namespace Tridium {

	static const String s_ShaderPath = ( FilePath::CurrentPath() / "../Tridium/Shaders" ).ToString();

}

using int2 = Tridium::Vector<2, Tridium::int32_t>;
using int3 = Tridium::Vector<3, Tridium::int32_t>;
using int4 = Tridium::Vector<4, Tridium::int32_t>;

using uint = Tridium::uint32_t;
using uint2 = Tridium::Vector<2, Tridium::uint32_t>;
using uint3 = Tridium::Vector<3, Tridium::uint32_t>;
using uint4 = Tridium::Vector<4, Tridium::uint32_t>;

using float2 = Tridium::Vector<2, Tridium::float32_t>;
using float3 = Tridium::Vector<3, Tridium::float32_t>;
using float4 = Tridium::Vector<4, Tridium::float32_t>;

using float2x2 = Tridium::Matrix<2, 2, Tridium::float32_t>;
using float3x3 = Tridium::Matrix<3, 3, Tridium::float32_t>;
using float4x4 = Tridium::Matrix<4, 4, Tridium::float32_t>;

#else

static const float PI = 3.14159265f;

#define CPP_ALIGN_TO_HLSL
#define INLINED_CONSTANTS_SPACE space9999
#define DEFAULT_VALUE( ... )

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

#endif // __cplusplus

struct CPP_ALIGN_TO_HLSL InlinedConstants_SceneData
{
	float4x4 PVM;
	float4x4 Model;
	float4x4 NormalMatrix;
	float3 CameraPosition;
};

#endif // SHADERINTEROP_HLSLI