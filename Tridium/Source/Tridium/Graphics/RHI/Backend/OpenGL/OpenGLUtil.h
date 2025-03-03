#pragma once
#include "OpenGL4.h"
#include <Tridium/Graphics/RHI/RHICommon.h>

namespace Tridium {

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
				case SrcColour:         return GL_SRC_COLOR;
				case OneMinusSrcColour: return GL_ONE_MINUS_SRC_COLOR;
				case SrcAlpha:          return GL_SRC_ALPHA;
				case OneMinusSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
				case DstColour:         return GL_DST_COLOR;
				case OneMinusDstColour: return GL_ONE_MINUS_DST_COLOR;
				case DstAlpha:          return GL_DST_ALPHA;
				case OneMinusDstAlpha:  return GL_ONE_MINUS_DST_ALPHA;
				case SrcAlphaSaturate:  return GL_SRC_ALPHA_SATURATE;
				default: return GL_ZERO;
			}
		}

	} // namespace ToOpenGL

} // namespace Tridium