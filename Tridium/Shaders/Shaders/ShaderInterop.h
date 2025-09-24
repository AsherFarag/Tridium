#ifndef SHADERINTEROP_HLSLI
#define SHADERINTEROP_HLSLI

#ifdef __cplusplus

#include <Tridium/Math/Math.h>
#include <Tridium/IO/FilePath.h>

#define INLINED_CONSTANTS_SPACE 9999

namespace Tridium {

	static const String s_ShaderPath = ( FilePath::CurrentPath() / "../Tridium/Shaders" ).ToString();

	using int2 = Vector<2, int32_t>;
	using int3 = Vector<3, int32_t>;
	using int4 = Vector<4, int32_t>;

	using uint = uint32_t;
	using uint2 = Vector<2, uint32_t>;
	using uint3 = Vector<3, uint32_t>;
	using uint4 = Vector<4, uint32_t>;

	using float2 = Vector<2, float32_t>;
	using float3 = Vector<3, float32_t>;
	using float4 = Vector<4, float32_t>;

	using float2x2 = Matrix<2, 2, float32_t>;
	using float3x3 = Matrix<3, 3, float32_t>;
	using float4x4 = Matrix<4, 4, float32_t>;

}

#else

#define INLINED_CONSTANTS_SPACE space9999

#define CONCAT_DETAIL(x, y) x##y
#define CONCAT(x, y) CONCATENATE_DETAIL(x, y)

#define CONSTANT_BUFFER( _Name, _Type, _Slot ) ConstantBuffer< _Type > _Name : register( b##_Slot )

#if defined( __PSSL__ )
	// Defined elsewhere
#elif defined( __spirv_opengl__ )
	#define INLINED_CONSTANTS( _Name, _Type ) ConstantBuffer< _Type > _Name## : register( b0, INLINED_CONSTANTS_SPACE )
#elif defined( __spirv__ )
	#define INLINED_CONSTANTS( _Name, _Type ) ConstantBuffer< _Type > _Name : register( b0, INLINED_CONSTANTS_SPACE )
#else
	#define INLINED_CONSTANTS( _Name, _Type ) ConstantBuffer< _Type > _Name : register( b0, INLINED_CONSTANTS_SPACE )
#endif

#define COMBINED_SAMPLER( _Name, _Type, _Slot ) \
	_Type _Name : register( t##_Slot ); \
	SamplerState _Name##_Sampler : register( s##_Slot )

#define GetCombinedSampler( _Texture ) _Texture##_Sampler

// Use for sampling combined samplers
#define SampleTexture( _Texture, _UV ) _Texture.Sample( GetCombinedSampler( _Texture ), _UV )

#endif // __cplusplus

#endif // SHADERINTEROP_HLSLI