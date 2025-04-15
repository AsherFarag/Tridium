#pragma once
#include "OpenGL4.h"
#include "OpenGLState.h"
#include "Utils/OpenGLToRHI.h"
#include "Utils/OpenGLWrappers.h"
#include <Tridium/Graphics/RHI/RHICommon.h>

namespace Tridium {

	struct OpenGLVertexElementType
	{
		GLenum Type = 0;
		uint16_t Count = 0;
		uint8_t ComponentSize = 0;
		bool Normalized = false;

		constexpr bool Valid() const { return Type != 0 || Count != 0 || ComponentSize != 0; }
	};

	namespace OpenGLUtil {

		template<typename T>
		void SetUniform( GLint a_Location, const T& a_Value );

		template<typename T>
		void SetUniformArray( GLint a_Location, const T* a_Values, uint32_t a_Count );

	}

	namespace ToOpenGL {

		inline constexpr GLenum GetFilter( ERHISamplerFilter a_Filter )
		{
			switch ( a_Filter )
			{
				using enum ERHISamplerFilter;
				case Point:               return GL_NEAREST;
				case Bilinear:            return GL_LINEAR;
				case Trilinear:           return GL_LINEAR_MIPMAP_LINEAR;
				case AnisotropicPoint:    return GL_NEAREST;
				case AnisotropicLinear:   return GL_LINEAR;
				default:                  return GL_NEAREST;
			}
		}

		inline constexpr GLenum GetAddressMode( ERHISamplerAddressMode a_Mode )
		{
			switch ( a_Mode )
			{
				using enum ERHISamplerAddressMode;
				case Repeat:  return GL_REPEAT;
				case Mirror:  return GL_MIRRORED_REPEAT;
				case Clamp:   return GL_CLAMP_TO_EDGE;
				case Border:  return GL_CLAMP_TO_BORDER;
				default:      return GL_REPEAT;
			}
		}

		inline constexpr GLenum GetComparisonFunc( ERHISamplerComparison a_Comparison )
		{
			switch ( a_Comparison )
			{
				using enum ERHISamplerComparison;
				case Never:        return GL_NEVER;
				case Less:         return GL_LESS;
				case Equal:        return GL_EQUAL;
				case LessEqual:    return GL_LEQUAL;
				case Greater:      return GL_GREATER;
				case NotEqual:     return GL_NOTEQUAL;
				case GreaterEqual: return GL_GEQUAL;
				case Always:       return GL_ALWAYS;
				default:           return GL_NEVER;
			}
		}

		inline constexpr GLenum GetShaderVisibility( ERHIShaderVisibility a_ShaderVisibility )
		{
			switch ( a_ShaderVisibility )
			{
				using enum ERHIShaderVisibility;
				case Vertex:      return GL_VERTEX_SHADER_BIT;
				case Hull:        return GL_TESS_CONTROL_SHADER_BIT;
				case Domain:      return GL_TESS_EVALUATION_SHADER_BIT;
				case Geometry:    return GL_GEOMETRY_SHADER_BIT;
				case Pixel:       return GL_FRAGMENT_SHADER_BIT;
				case All:        return GL_ALL_SHADER_BITS;
				default:        return GL_ALL_SHADER_BITS;
			}
		}

		inline constexpr GLenum GetBlendOp( ERHIBlendOp a_Factor )
		{
			switch ( a_Factor )
			{
				using enum ERHIBlendOp;
				case Zero:              return GL_ZERO;
				case One:               return GL_ONE;
				case SrcColor:         return GL_SRC_COLOR;
				case OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
				case SrcAlpha:          return GL_SRC_ALPHA;
				case OneMinusSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
				case DstColor:         return GL_DST_COLOR;
				case OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
				case DstAlpha:          return GL_DST_ALPHA;
				case OneMinusDstAlpha:  return GL_ONE_MINUS_DST_ALPHA;
				case SrcAlphaSaturate:  return GL_SRC_ALPHA_SATURATE;
				default: return GL_ZERO;
			}
		}

		inline constexpr GLenum GetShaderType( ERHIShaderType a_Type )
		{
			switch ( a_Type )
			{
				using enum ERHIShaderType;
				case Vertex:   return GL_VERTEX_SHADER;
				case Geometry: return GL_GEOMETRY_SHADER;
				case Hull:     return GL_TESS_CONTROL_SHADER;
				case Domain:   return GL_TESS_EVALUATION_SHADER;
				case Pixel:    return GL_FRAGMENT_SHADER;
				case Compute:  return GL_COMPUTE_SHADER;
				default:       return 0;
			}
		}

		inline constexpr GLenum GetTopology( ERHITopology a_Topology )
		{
			switch ( a_Topology )
			{
				using enum ERHITopology;
				case Point:     return GL_POINTS;
				case Line:      return GL_LINES;
				case LineStrip: return GL_LINE_STRIP;
				case Triangle:  return GL_TRIANGLES;
				case TriangleStrip: return GL_TRIANGLE_STRIP;
				default:        return GL_TRIANGLES;
			}
		}

		inline constexpr OpenGLVertexElementType GetVertexElementType( ERHIFormat format )
		{
			switch ( format )
			{
			case RHIVertexElementFormats::Half1: return { GL_HALF_FLOAT, 1, 2 };
			case RHIVertexElementFormats::Half2: return { GL_HALF_FLOAT, 2, 2 };
			case RHIVertexElementFormats::Half3: return { GL_HALF_FLOAT, 3, 2 };
			case RHIVertexElementFormats::Half4: return { GL_HALF_FLOAT, 4, 2 };
			case RHIVertexElementFormats::Float1: return { GL_FLOAT, 1, 4 };
			case RHIVertexElementFormats::Float2: return { GL_FLOAT, 2, 4 };
			case RHIVertexElementFormats::Float3: return { GL_FLOAT, 3, 4 };
			case RHIVertexElementFormats::Float4: return { GL_FLOAT, 4, 4 };
			case RHIVertexElementFormats::Short1: return { GL_SHORT, 1, 2 };
			case RHIVertexElementFormats::Short2: return { GL_SHORT, 2, 2 };
			case RHIVertexElementFormats::Short3: return { GL_SHORT, 3, 2 };
			case RHIVertexElementFormats::Short4: return { GL_SHORT, 4, 2 };
			case RHIVertexElementFormats::UShort1: return { GL_UNSIGNED_SHORT, 1, 2 };
			case RHIVertexElementFormats::UShort2: return { GL_UNSIGNED_SHORT, 2, 2 };
			case RHIVertexElementFormats::UShort3: return { GL_UNSIGNED_SHORT, 3, 2 };
			case RHIVertexElementFormats::UShort4: return { GL_UNSIGNED_SHORT, 4, 2 };
			case RHIVertexElementFormats::Int1: return { GL_INT, 1, 4 };
			case RHIVertexElementFormats::Int2: return { GL_INT, 2, 4 };
			case RHIVertexElementFormats::Int3: return { GL_INT, 3, 4 };
			case RHIVertexElementFormats::Int4: return { GL_INT, 4, 4 };
			case RHIVertexElementFormats::UInt1: return { GL_UNSIGNED_INT, 1, 4 };
			case RHIVertexElementFormats::UInt2: return { GL_UNSIGNED_INT, 2, 4 };
			case RHIVertexElementFormats::UInt3: return { GL_UNSIGNED_INT, 3, 4 };
			case RHIVertexElementFormats::UInt4: return { GL_UNSIGNED_INT, 4, 4 };
			case RHIVertexElementFormats::Color: return { GL_UNSIGNED_BYTE, 4, 1, true };
			case RHIVertexElementFormats::ColorSRGB: return { GL_UNSIGNED_BYTE, 4, 1, true };
			default: return { 0, 0, 0 };
			}
		}


	} // namespace ToOpenGL

	template<typename T>
	inline void OpenGLUtil::SetUniform(GLint a_Location, const T & a_Value)
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
	void OpenGLUtil::SetUniformArray( GLint a_Location, const T* a_Values, uint32_t a_Count )
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