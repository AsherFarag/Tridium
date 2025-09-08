#pragma once
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include "OpenGL4.h"

namespace Tridium::OpenGL {

	struct VertexElementType
	{
		GLenum Type = 0;
		uint16_t Count = 0;
		uint8_t ComponentSize = 0;
		bool Normalized = false;

		constexpr bool Valid() const { return Type != 0 || Count != 0 || ComponentSize != 0; }

		static constexpr VertexElementType From( ERHIFormat format )
		{
			switch ( format )
			{
			case RHIVertexElementFormats::Half1: return { GL_HALF_FLOAT, 1, 2 };
			case RHIVertexElementFormats::Half2: return { GL_HALF_FLOAT, 2, 2 };
			//case RHIVertexElementFormats::Half3: return { GL_HALF_FLOAT, 3, 2 };
			case RHIVertexElementFormats::Half4: return { GL_HALF_FLOAT, 4, 2 };
			case RHIVertexElementFormats::Float1: return { GL_FLOAT, 1, 4 };
			case RHIVertexElementFormats::Float2: return { GL_FLOAT, 2, 4 };
			case RHIVertexElementFormats::Float3: return { GL_FLOAT, 3, 4 };
			case RHIVertexElementFormats::Float4: return { GL_FLOAT, 4, 4 };
			case RHIVertexElementFormats::Short1: return { GL_SHORT, 1, 2 };
			case RHIVertexElementFormats::Short2: return { GL_SHORT, 2, 2 };
			//case RHIVertexElementFormats::Short3: return { GL_SHORT, 3, 2 };
			case RHIVertexElementFormats::Short4: return { GL_SHORT, 4, 2 };
			case RHIVertexElementFormats::UShort1: return { GL_UNSIGNED_SHORT, 1, 2 };
			case RHIVertexElementFormats::UShort2: return { GL_UNSIGNED_SHORT, 2, 2 };
			//case RHIVertexElementFormats::UShort3: return { GL_UNSIGNED_SHORT, 3, 2 };
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
	};

	inline constexpr GLint Translate( ERHIComparison a_Comparison )
	{
		switch ( a_Comparison )
		{
			using enum ERHIComparison;
			case Never: return GL_NEVER;
			case Less: return GL_LESS;
			case Equal: return GL_EQUAL;
			case LessEqual: return GL_LEQUAL;
			case Greater: return GL_GREATER;
			case NotEqual: return GL_NOTEQUAL;
			case GreaterEqual: return GL_GEQUAL;
			case Always: return GL_ALWAYS;
			default: ASSERT( false, "Invalid comparison" ); return GL_NEVER;
		}
	}

	inline constexpr GLint Translate( ERHIBlendFactor a_Factor )
	{
		switch ( a_Factor )
		{
			using enum ERHIBlendFactor;
			case Zero: return GL_ZERO;
			case One: return GL_ONE;
			case SrcColor: return GL_SRC_COLOR;
			case OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
			case SrcAlpha: return GL_SRC_ALPHA;
			case OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case DstColor: return GL_DST_COLOR;
			case OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
			case DstAlpha: return GL_DST_ALPHA;
			case OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
			default: return GL_ZERO;
		}
	}

	inline constexpr GLint Translate( ERHIBlendOp a_Equation )
	{
		switch ( a_Equation )
		{
			using enum ERHIBlendOp;
			case Add: return GL_FUNC_ADD;
			case Subtract: return GL_FUNC_SUBTRACT;
			case ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case Min: return GL_MIN;
			case Max: return GL_MAX;
			default: return GL_FUNC_ADD;
		}
	}

	inline constexpr GLint Translate( ERHIStencilOp a_StencilOp )
	{
		switch ( a_StencilOp )
		{
			using enum ERHIStencilOp;
			case Keep: return GL_KEEP;
			case Zero: return GL_ZERO;
			case Replace: return GL_REPLACE;
			case Increment: return GL_INCR;
			//case IncrementWrap: return GL_INCR_WRAP;
			case Decrement: return GL_DECR;
			//case DecrementWrap: return GL_DECR_WRAP;
			case Invert: return GL_INVERT;
			default: ASSERT( false, "Invalid stencil operation" ); return GL_KEEP;
		}
	}

	inline void Translate( ERHISamplerFilter a_Filter, GLenum& o_MinFilter, GLenum& o_MagFilter, GLenum& o_MipFilter, bool& o_IsComparison, bool& o_IsAnisotropic )
	{
		// Default to false
		o_IsComparison = false;
		o_IsAnisotropic = false;

		using enum ERHISamplerFilter;
		switch ( a_Filter )
		{
		case Unknown:
			RHI_DEV_CHECK( false, "Unknown sampler filter type!" );
			o_MinFilter = o_MagFilter = o_MipFilter = GL_NEAREST;
			break;

		case MinMagMipPoint:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_NEAREST;
			break;

		case MinMagPointMipLinear:
			o_MinFilter = o_MagFilter = GL_NEAREST;
			o_MipFilter = GL_LINEAR;
			break;

		case MinPointMagLinearMipPoint:
			o_MinFilter = GL_NEAREST;
			o_MagFilter = GL_LINEAR;
			o_MipFilter = GL_NEAREST;
			break;

		case MinPointMagMipLinear:
			o_MinFilter = GL_NEAREST;
			o_MagFilter = o_MipFilter = GL_LINEAR;
			break;

		case MinLinearMagMipPoint:
			o_MinFilter = GL_LINEAR;
			o_MagFilter = o_MipFilter = GL_NEAREST;
			break;

		case MinLinearMagPointMipLinear:
			o_MinFilter = GL_LINEAR;
			o_MagFilter = GL_NEAREST;
			o_MipFilter = GL_LINEAR;
			break;

		case MinMagLinearMipPoint:
			o_MinFilter = o_MagFilter = GL_LINEAR;
			o_MipFilter = GL_NEAREST;
			break;

		case MinMagMipLinear:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_LINEAR;
			break;

		case Anisotropic:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_LINEAR;
			o_IsAnisotropic = true;
			break;

			// = Comparison Filters =

		case ComparisonMinMagMipPoint:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_NEAREST;
			o_IsComparison = true;
			break;

		case ComparisonMinMagPointMipLinear:
			o_MinFilter = o_MagFilter = GL_NEAREST;
			o_MipFilter = GL_LINEAR;
			o_IsComparison = true;
			break;

		case ComparisonMinPointMagLinearMipPoint:
			o_MinFilter = GL_NEAREST;
			o_MagFilter = GL_LINEAR;
			o_MipFilter = GL_NEAREST;
			o_IsComparison = true;
			break;

		case ComparisonMinPointMagMipLinear:
			o_MinFilter = GL_NEAREST;
			o_MagFilter = o_MipFilter = GL_LINEAR;
			o_IsComparison = true;
			break;

		case ComparisonMinLinearMagMipPoint:
			o_MinFilter = GL_LINEAR;
			o_MagFilter = o_MipFilter = GL_NEAREST;
			o_IsComparison = true;
			break;

		case ComparisonMinLinearMagPointMipLinear:
			o_MinFilter = GL_LINEAR;
			o_MagFilter = GL_NEAREST;
			o_MipFilter = GL_LINEAR;
			o_IsComparison = true;
			break;

		case ComparisonMinMagLinearMipPoint:
			o_MinFilter = o_MagFilter = GL_LINEAR;
			o_MipFilter = GL_NEAREST;
			o_IsComparison = true;
			break;	

		case ComparisonMinMagMipLinear:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_LINEAR;
			o_IsComparison = true;
			break;

		case ComparisonAnisotropic:
			o_MinFilter = o_MagFilter = o_MipFilter = GL_LINEAR;
			o_IsAnisotropic = true;
			o_IsComparison = true;
			break;

		default:
			RHI_DEV_CHECK( false, "Unknown sampler filter type!" );
			o_MinFilter = o_MagFilter = o_MipFilter = GL_NEAREST;
			break;
		}
	}

	inline constexpr GLenum Translate( ERHISamplerAddressMode a_Mode )
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

	inline constexpr GLenum Translate( ERHIShaderVisibility a_ShaderVisibility )
	{
		using enum ERHIShaderVisibility;
		switch ( a_ShaderVisibility )
		{
		case Vertex:      return GL_VERTEX_SHADER_BIT;
		case Hull:        return GL_TESS_CONTROL_SHADER_BIT;
		case Domain:      return GL_TESS_EVALUATION_SHADER_BIT;
		case Geometry:    return GL_GEOMETRY_SHADER_BIT;
		case Pixel:       return GL_FRAGMENT_SHADER_BIT;
		case All:        return GL_ALL_SHADER_BITS;
		default:        return GL_ALL_SHADER_BITS;
		}
	}

	inline constexpr GLenum Translate( ERHIShaderType a_Type )
	{
		using enum ERHIShaderType;
		switch ( a_Type )
		{
		case Vertex:   return GL_VERTEX_SHADER;
		case Geometry: return GL_GEOMETRY_SHADER;
		case Hull:     return GL_TESS_CONTROL_SHADER;
		case Domain:   return GL_TESS_EVALUATION_SHADER;
		case Pixel:    return GL_FRAGMENT_SHADER;
		case Compute:  return GL_COMPUTE_SHADER;
		default:       return 0;
		}
	}

	inline constexpr GLenum Translate( ERHITopology a_Topology )
	{
		using enum ERHITopology;
		switch ( a_Topology )
		{
		case Point:     return GL_POINTS;
		case Line:      return GL_LINES;
		case LineStrip: return GL_LINE_STRIP;
		case Triangle:  return GL_TRIANGLES;
		case TriangleStrip: return GL_TRIANGLE_STRIP;
		default:        return GL_TRIANGLES;
		}
	}

	inline constexpr GLenum Translate( ERHIUsage a_Usage )
	{
		using enum ERHIUsage;
		switch ( a_Usage )
		{
		case Default: return GL_STATIC_DRAW;
		case Static:  return GL_STATIC_DRAW;
		case Dynamic: return GL_DYNAMIC_DRAW;
		default:      return GL_STATIC_DRAW;
		}
	}

	inline constexpr GLenum Translate( ERHILogicOp a_Op )
	{
		using enum ERHILogicOp;
		switch ( a_Op )
		{
			case Clear:         return GL_CLEAR;
			case Set:           return GL_SET;
			case Copy:          return GL_COPY;
			case CopyInverted:  return GL_COPY_INVERTED;
			case NoOp:          return GL_NOOP;
			case Invert:        return GL_INVERT;
			case And:           return GL_AND;
			case Nand:          return GL_NAND;
			case Or:            return GL_OR;
			case Nor:           return GL_NOR;
			case Xor:           return GL_XOR;
			case Eqv:           return GL_EQUIV;
			case AndReverse:    return GL_AND_REVERSE;
			case AndInverted:   return GL_AND_INVERTED;
			case OrReverse:     return GL_OR_REVERSE;
			case OrInverted:    return GL_OR_INVERTED;
			default:            return GL_NOOP;
		}
	}

} // namespace Tridium::OpenGL