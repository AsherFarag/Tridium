#pragma once
#include "../OpenGL4.h"
#include "../../../RHICommon.h"

namespace Tridium::OpenGL {

	inline GLint Translate( ERHIComparison a_Comparison )
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
			default: ASSERT_LOG( false, "Invalid comparison" ); return GL_NEVER;
		}
	}

	inline GLint Translate( ERHIBlendOp a_Factor )
	{
		switch ( a_Factor )
		{
			using enum ERHIBlendOp;
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

	inline GLint Translate( ERHIBlendEq a_Equation )
	{
		switch ( a_Equation )
		{
			using enum ERHIBlendEq;
			case Add: return GL_FUNC_ADD;
			case Subtract: return GL_FUNC_SUBTRACT;
			case ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case Min: return GL_MIN;
			case Max: return GL_MAX;
			default: return GL_FUNC_ADD;
		}
	}

	inline GLint Translate( ERHIDepthOp a_DepthOp )
	{
		switch ( a_DepthOp )
		{
			using enum ERHIDepthOp;
			case Keep: return GL_KEEP;
			case Replace: return GL_REPLACE;
			default: ASSERT_LOG( false, "Invalid depth operation" ); return GL_KEEP;
		}
	}

	inline GLint Translate( ERHIStencilOp a_StencilOp )
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
			default: ASSERT_LOG( false, "Invalid stencil operation" ); return GL_KEEP;
		}
	}


} // namespace Tridium::OpenGL