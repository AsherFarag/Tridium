#ifndef SHADERINTEROP_HLSLI
#define SHADERINTEROP_HLSLI

#ifdef __cplusplus

#define BEGIN_TRIDIUM_NAMESPACE namespace Tridium {
#define END_TRIDIUM_NAMESPACE }

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

#define BEGIN_TRIDIUM_NAMESPACE
#define END_TRIDIUM_NAMESPACE

#define CPP_ALIGN_TO_HLSL
#define INLINED_CONSTANTS_SPACE space9999
#define DEFAULT_VALUE( ... )

#endif // __cplusplus

struct CPP_ALIGN_TO_HLSL InlinedConstants_SceneData
{
	float4x4 PVM;
	float4x4 Model;
	float4x4 NormalMatrix;
	float3 CameraPosition;
};

#endif // SHADERINTEROP_HLSLI