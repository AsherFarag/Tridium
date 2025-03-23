#pragma once

#ifdef __cplusplus
#include <Tridium/Math/Math.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	static const String s_ShaderPath = ( FilePath::CurrentPath() / "../Tridium/Shaders" ).ToString();

	using int2 = i32Vector2;
	using int3 = i32Vector3;
	using int4 = i32Vector4;

	using uint = uint32_t;
	using uint2 = u32Vector2;
	using uint3 = u32Vector3;
	using uint4 = u32Vector4;

	using float2 = f32Vector2;
	using float3 = f32Vector3;
	using float4 = f32Vector4;

	using float2x2 = f32Matrix2;
	using float3x3 = f32Matrix3;
	using float4x4 = f32Matrix4;

}

#else

#define CONCAT_DETAIL(x, y) x##y
#define CONCAT(x, y) CONCATENATE_DETAIL(x, y)

#define CONSTANT_BUFFER( _Name, _Type, _Slot ) ConstantBuffer< _Type > _Name : register( b##_Slot )

#if defined( __PSSL__ )
	// Defined elsewhere
#elif defined( __spirv__ )
	#define INLINED_CONSTANT( _Name, _Type ) [[vk::push_constant]] _Type _Name
#else
	#define INLINED_CONSTANT( _Name, _Type ) ConstantBuffer< _Type > _Name : register( b0 )
#endif

#define COMBINED_SAMPLER( _Name, _Type, _Slot ) \
	_Type _Name : register( t##_Slot ); \
	SamplerState _Name##_Sampler : register( s##_Slot )

#endif // __cplusplus