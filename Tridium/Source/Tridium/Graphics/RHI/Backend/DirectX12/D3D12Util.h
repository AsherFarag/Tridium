#pragma once
#include "DirectX12.h"
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	namespace ToD3D12 {

		static constexpr D3D12_BLEND GetBlend( ERHIBlendOp a_Factor )
		{
			switch ( a_Factor )
			{
				using enum ERHIBlendOp;
				case Zero:              return D3D12_BLEND_ZERO;
				case One:               return D3D12_BLEND_ONE;
				case SrcColour:         return D3D12_BLEND_SRC_COLOR;
				case OneMinusSrcColour: return D3D12_BLEND_INV_SRC_COLOR;
				case SrcAlpha:          return D3D12_BLEND_SRC_ALPHA;
				case OneMinusSrcAlpha:  return D3D12_BLEND_INV_SRC_ALPHA;
				case DstColour:         return D3D12_BLEND_DEST_COLOR;
				case OneMinusDstColour: return D3D12_BLEND_INV_DEST_COLOR;
				case DstAlpha:          return D3D12_BLEND_DEST_ALPHA;
				case OneMinusDstAlpha:  return D3D12_BLEND_INV_DEST_ALPHA;
				case SrcAlphaSaturate:  return D3D12_BLEND_SRC_ALPHA_SAT;
				default: return D3D12_BLEND_ZERO;
			}
		}

		static constexpr D3D12_BLEND_OP GetBlendOp( ERHIBlendEq a_Eq )
		{
			switch ( a_Eq )
			{
				using enum ERHIBlendEq;
				case Add:             return D3D12_BLEND_OP_ADD;
				case Subtract:        return D3D12_BLEND_OP_SUBTRACT;
				case ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
				case Min:             return D3D12_BLEND_OP_MIN;
				case Max:             return D3D12_BLEND_OP_MAX;
				default:              return D3D12_BLEND_OP_ADD;
			}
		}

		static constexpr D3D12_COMPARISON_FUNC GetComparisonFunc( ERHIComparison a_Comparison )
		{
			switch ( a_Comparison )
			{
				using enum ERHIComparison;
				case Never:        return D3D12_COMPARISON_FUNC_NEVER;
				case Less:         return D3D12_COMPARISON_FUNC_LESS;
				case Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
				case LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
				case Greater:      return D3D12_COMPARISON_FUNC_GREATER;
				case NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
				case GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
				case Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
				default:           return D3D12_COMPARISON_FUNC_NEVER;
			}
		}

		static constexpr D3D12_STENCIL_OP GetStencilOp( ERHIStencilOp a_Op )
		{
			switch ( a_Op )
			{
				using enum ERHIStencilOp;
				case Keep:    return D3D12_STENCIL_OP_KEEP;
				case Zero:    return D3D12_STENCIL_OP_ZERO;
				case Replace: return D3D12_STENCIL_OP_REPLACE;
				case Invert:  return D3D12_STENCIL_OP_INVERT;
				default:      return D3D12_STENCIL_OP_KEEP;
			}
		}

		static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType( ERHITopology a_Topology )
		{
			switch ( a_Topology )
			{
				using enum ERHITopology;
				case Line:          return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case Triangle:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				case LineStrip:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				default:            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			}
		}

        static constexpr DXGI_FORMAT GetFormat( ERHITextureFormat a_Format )
        {
            switch ( a_Format )
            {
				using enum ERHITextureFormat;
                case R8:      return DXGI_FORMAT_R8_UNORM;
                case RG8:     return DXGI_FORMAT_R8G8_UNORM;
                case RGBA8:   return DXGI_FORMAT_R8G8B8A8_UNORM;
                case SRGBA8:  return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                case R16:     return DXGI_FORMAT_R16_UNORM;
                case RG16:    return DXGI_FORMAT_R16G16_UNORM;
                case RGBA16:  return DXGI_FORMAT_R16G16B16A16_UNORM;
                case R16F:    return DXGI_FORMAT_R16_FLOAT;
                case RG16F:   return DXGI_FORMAT_R16G16_FLOAT;
                case RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
                case R32F:    return DXGI_FORMAT_R32_FLOAT;
                case RG32F:   return DXGI_FORMAT_R32G32_FLOAT;
                case RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case R16I:    return DXGI_FORMAT_R16_SINT;
                case RG16I:   return DXGI_FORMAT_R16G16_SINT;
                case RGBA16I: return DXGI_FORMAT_R16G16B16A16_SINT;
                case R32I:    return DXGI_FORMAT_R32_SINT;
                case RG32I:   return DXGI_FORMAT_R32G32_SINT;
                case RGBA32I: return DXGI_FORMAT_R32G32B32A32_SINT;
                case D16:     return DXGI_FORMAT_D16_UNORM;
                case D32:     return DXGI_FORMAT_D32_FLOAT;
                case D24S8:   return DXGI_FORMAT_D24_UNORM_S8_UINT;
                case BC1:     return DXGI_FORMAT_BC1_UNORM;
                case BC3:     return DXGI_FORMAT_BC3_UNORM;
                case BC4:     return DXGI_FORMAT_BC4_UNORM;
                case BC5:     return DXGI_FORMAT_BC5_UNORM;
                case BC7:     return DXGI_FORMAT_BC7_UNORM;
                default:      return DXGI_FORMAT_UNKNOWN;
            }
        }

		static constexpr DXGI_FORMAT GetFormat( ERHIVertexElementType a_Type )
		{
			switch ( a_Type )
			{
				using enum ERHIVertexElementType;
				case Float1: return DXGI_FORMAT_R32_FLOAT;
				case Float2: return DXGI_FORMAT_R32G32_FLOAT;
				case Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
				case Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case UByte4: return DXGI_FORMAT_R8G8B8A8_UINT;
				case Color:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Int1:   return DXGI_FORMAT_R32_SINT;
				case Int2:   return DXGI_FORMAT_R32G32_SINT;
				case Int3:   return DXGI_FORMAT_R32G32B32_SINT;
				case Int4:   return DXGI_FORMAT_R32G32B32A32_SINT;
				case UInt1:  return DXGI_FORMAT_R32_UINT;
				case UInt2:  return DXGI_FORMAT_R32G32_UINT;
				case UInt3:  return DXGI_FORMAT_R32G32B32_UINT;
				case UInt4:  return DXGI_FORMAT_R32G32B32A32_UINT;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

	}

}