#pragma once
#include "OpenGL4.h"
#include "OpenGLState.h"
#include <Tridium/Graphics/RHI/RHICommon.h>

namespace Tridium::OpenGL {

	template<typename T>
	void SetUniform( GLint a_Location, const T& a_Value );

	template<typename T>
	void SetUniformArray( GLint a_Location, const T* a_Values, uint32_t a_Count );

	template<typename T>
	inline void SetUniform(GLint a_Location, const T & a_Value)
	{
		if constexpr ( std::is_same_v<T, int> )
		{
			OpenGL3::Uniform1i( a_Location, a_Value );
		}
		else if constexpr ( std::is_same_v<T, int2> )
		{
			OpenGL3::Uniform2i( a_Location, a_Value.x, a_Value.y );
		}
		else if constexpr ( std::is_same_v<T, int3> )
		{
			OpenGL3::Uniform3i( a_Location, a_Value.x, a_Value.y, a_Value.z );
		}
		else if constexpr ( std::is_same_v<T, int4> )
		{
			OpenGL3::Uniform4i( a_Location, a_Value.x, a_Value.y, a_Value.z, a_Value.w );
		}
		else if constexpr ( std::is_same_v<T, uint> )
		{
			OpenGL3::Uniform1ui( a_Location, a_Value );
		}
		else if constexpr ( std::is_same_v<T, uint2> )
		{
			OpenGL3::Uniform2ui( a_Location, a_Value.x, a_Value.y );
		}
		else if constexpr ( std::is_same_v<T, uint3> )
		{
			OpenGL3::Uniform3ui( a_Location, a_Value.x, a_Value.y, a_Value.z );
		}
		else if constexpr ( std::is_same_v<T, uint4> )
		{
			OpenGL3::Uniform4ui( a_Location, a_Value.x, a_Value.y, a_Value.z, a_Value.w );
		}
		else if constexpr ( std::is_same_v<T, float> )
		{
			OpenGL3::Uniform1f( a_Location, a_Value );
		}
		else if constexpr ( std::is_same_v<T, float2> )
		{
			OpenGL3::Uniform2f( a_Location, a_Value.x, a_Value.y );
		}
		else if constexpr ( std::is_same_v<T, float3> )
		{
			OpenGL3::Uniform3f( a_Location, a_Value.x, a_Value.y, a_Value.z );
		}
		else if constexpr ( std::is_same_v<T, float4> )
		{
			OpenGL3::Uniform4f( a_Location, a_Value.x, a_Value.y, a_Value.z, a_Value.w );
		}
		else if constexpr ( std::is_same_v<T, float2x2> )
		{
			OpenGL3::UniformMatrix2fv( a_Location, 1, GL_FALSE, &a_Value[0][0] );
		}
		else if constexpr ( std::is_same_v<T, float3x3> )
		{
			OpenGL3::UniformMatrix3fv( a_Location, 1, GL_FALSE, &a_Value[0][0] );
		}
		else if constexpr ( std::is_same_v<T, float4x4> )
		{
			OpenGL3::UniformMatrix4fv( a_Location, 1, GL_FALSE, &a_Value[0][0] );
		}
		else if constexpr ( Concepts::IsStruct<T> )
		{
			//OpenGL3::UniformBlockBinding( a_Location, a_Value );
		}
		else
		{
			//static_assert( false, "Unsupported uniform type" );
		}
	}

	template<typename T>
	void SetUniformArray( GLint a_Location, const T* a_Values, uint32_t a_Count )
	{
		if constexpr ( std::is_same_v<T, int> )
		{
			OpenGL3::Uniform1iv( a_Location, a_Count, a_Values );
		}
		else if constexpr ( std::is_same_v<T, int2> )
		{
			OpenGL3::Uniform2iv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, int3> )
		{
			OpenGL3::Uniform3iv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, int4> )
		{
			OpenGL3::Uniform4iv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, uint> )
		{
			OpenGL3::Uniform1uiv( a_Location, a_Count, a_Values );
		}
		else if constexpr ( std::is_same_v<T, uint2> )
		{
			OpenGL3::Uniform2uiv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, uint3> )
		{
			OpenGL3::Uniform3uiv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, uint4> )
		{
			OpenGL3::Uniform4uiv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, float> )
		{
			OpenGL3::Uniform1fv( a_Location, a_Count, a_Values );
		}
		else if constexpr ( std::is_same_v<T, float2> )
		{
			OpenGL3::Uniform2fv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, float3> )
		{
			OpenGL3::Uniform3fv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, float4> )
		{
			OpenGL3::Uniform4fv( a_Location, a_Count, &a_Values->x );
		}
		else if constexpr ( std::is_same_v<T, float2x2> )
		{
			OpenGL3::UniformMatrix2fv( a_Location, a_Count, GL_FALSE, &a_Values->x.x );
		}
		else if constexpr ( std::is_same_v<T, float3x3> )
		{
			OpenGL3::UniformMatrix3fv( a_Location, a_Count, GL_FALSE, &a_Values->x.x );
		}
		else if constexpr ( std::is_same_v<T, float4x4> )
		{
			OpenGL3::UniformMatrix4fv( a_Location, a_Count, GL_FALSE, &a_Values->x.x );
		}
		else
		{
			//static_assert( false, "Unsupported uniform type" );
		}
	}

} // namespace Tridium