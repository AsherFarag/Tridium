#pragma once
#include "D3D12.h"
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	namespace D3D12 {

		// Simple conversion functions for D3D12 enums to RHI enums.
		// If a specification exists, you can call To<Enum>::From( value ) to convert between D3D12 and Tridium RHI.
		template<typename _To>
		struct To;

		template<typename _From>
		constexpr auto Translate( const _From& a_From )
		{
			using ToType = To<_From>::FromType;
			return To<ToType>::From(a_From);
		}

		template<typename _From>
		constexpr auto Translate( const EnumFlags<_From>& a_From )
		{
			using ToType = To<_From>::FromType;
			return To<ToType>::From( a_From );
		}

		template<typename _To, typename _From>
		constexpr _To Translate( const _From& a_From )
		{
			return To<_To>::From( a_From );
		}

		//////////////////////////////////////////////////////////////////////////
		// RHI SHADER MODEL - D3D12 SHADER MODEL
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIShaderModel>
		{
			using FromType = D3D_SHADER_MODEL;
			static constexpr ERHIShaderModel From( D3D_SHADER_MODEL a_Model )
			{
				switch ( a_Model )
				{
				case D3D_SHADER_MODEL_5_1: return ERHIShaderModel::SM_5_0;
				case D3D_SHADER_MODEL_6_0: return ERHIShaderModel::SM_6_0;
				case D3D_SHADER_MODEL_6_1: return ERHIShaderModel::SM_6_1;
				case D3D_SHADER_MODEL_6_2: return ERHIShaderModel::SM_6_2;
				case D3D_SHADER_MODEL_6_3: return ERHIShaderModel::SM_6_3;
				case D3D_SHADER_MODEL_6_4: return ERHIShaderModel::SM_6_4;
				case D3D_SHADER_MODEL_6_5: return ERHIShaderModel::SM_6_5;
				default:                   return ERHIShaderModel::SM_5_0;
				}
			}
		};

		template<>
		struct To<D3D_SHADER_MODEL>
		{
			using FromType = ERHIShaderModel;
			static constexpr D3D_SHADER_MODEL From( ERHIShaderModel a_Model )
			{
				switch ( a_Model )
				{
				case ERHIShaderModel::SM_5_0: return D3D_SHADER_MODEL_5_1;
				case ERHIShaderModel::SM_6_0: return D3D_SHADER_MODEL_6_0;
				case ERHIShaderModel::SM_6_1: return D3D_SHADER_MODEL_6_1;
				case ERHIShaderModel::SM_6_2: return D3D_SHADER_MODEL_6_2;
				case ERHIShaderModel::SM_6_3: return D3D_SHADER_MODEL_6_3;
				case ERHIShaderModel::SM_6_4: return D3D_SHADER_MODEL_6_4;
				case ERHIShaderModel::SM_6_5: return D3D_SHADER_MODEL_6_5;
				default:                      return D3D_SHADER_MODEL_5_1;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI BLEND OP - D3D12 BLEND 
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIBlendFactor>
		{
			using FromType = D3D12_BLEND;
			static constexpr ERHIBlendFactor From( D3D12_BLEND a_Factor )
			{
				switch ( a_Factor )
				{
				case D3D12_BLEND_ZERO:            return ERHIBlendFactor::Zero;
				case D3D12_BLEND_ONE:             return ERHIBlendFactor::One;
				case D3D12_BLEND_SRC_COLOR:       return ERHIBlendFactor::SrcColor;
				case D3D12_BLEND_INV_SRC_COLOR:   return ERHIBlendFactor::OneMinusSrcColor;
				case D3D12_BLEND_SRC_ALPHA:       return ERHIBlendFactor::SrcAlpha;
				case D3D12_BLEND_INV_SRC_ALPHA:   return ERHIBlendFactor::OneMinusSrcAlpha;
				case D3D12_BLEND_DEST_COLOR:      return ERHIBlendFactor::DstColor;
				case D3D12_BLEND_INV_DEST_COLOR:  return ERHIBlendFactor::OneMinusDstColor;
				case D3D12_BLEND_DEST_ALPHA:      return ERHIBlendFactor::DstAlpha;
				case D3D12_BLEND_INV_DEST_ALPHA:  return ERHIBlendFactor::OneMinusDstAlpha;
				case D3D12_BLEND_SRC_ALPHA_SAT:   return ERHIBlendFactor::SrcAlphaSaturate;
				default:                          return ERHIBlendFactor::Zero;
				}
			}
		};

		template<>
		struct To<D3D12_BLEND>
		{
			using FromType = ERHIBlendFactor;
			static constexpr D3D12_BLEND From( ERHIBlendFactor a_Factor )
			{
				switch ( a_Factor )
				{
				case ERHIBlendFactor::Zero:              return D3D12_BLEND_ZERO;
				case ERHIBlendFactor::One:               return D3D12_BLEND_ONE;
				case ERHIBlendFactor::SrcColor:         return D3D12_BLEND_SRC_COLOR;
				case ERHIBlendFactor::OneMinusSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
				case ERHIBlendFactor::SrcAlpha:          return D3D12_BLEND_SRC_ALPHA;
				case ERHIBlendFactor::OneMinusSrcAlpha:  return D3D12_BLEND_INV_SRC_ALPHA;
				case ERHIBlendFactor::DstColor:         return D3D12_BLEND_DEST_COLOR;
				case ERHIBlendFactor::OneMinusDstColor: return D3D12_BLEND_INV_DEST_COLOR;
				case ERHIBlendFactor::DstAlpha:          return D3D12_BLEND_DEST_ALPHA;
				case ERHIBlendFactor::OneMinusDstAlpha:  return D3D12_BLEND_INV_DEST_ALPHA;
				case ERHIBlendFactor::SrcAlphaSaturate:  return D3D12_BLEND_SRC_ALPHA_SAT;
				default:                             return D3D12_BLEND_ZERO;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI BLEND EQ - D3D12 BLEND EQ
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIBlendOp>
		{
			using FromType = D3D12_BLEND_OP;
			static constexpr ERHIBlendOp From( D3D12_BLEND_OP a_Eq )
			{
				switch ( a_Eq )
				{
				case D3D12_BLEND_OP_ADD:            return ERHIBlendOp::Add;
				case D3D12_BLEND_OP_SUBTRACT:       return ERHIBlendOp::Subtract;
				case D3D12_BLEND_OP_REV_SUBTRACT:   return ERHIBlendOp::ReverseSubtract;
				case D3D12_BLEND_OP_MIN:            return ERHIBlendOp::Min;
				case D3D12_BLEND_OP_MAX:            return ERHIBlendOp::Max;
				default:                            return ERHIBlendOp::Add;
				}
			}
		};

		template<>
		struct To<D3D12_BLEND_OP>
		{
			using FromType = ERHIBlendOp;
			static constexpr D3D12_BLEND_OP From( ERHIBlendOp a_Eq )
			{
				switch ( a_Eq )
				{
				case ERHIBlendOp::Add:             return D3D12_BLEND_OP_ADD;
				case ERHIBlendOp::Subtract:        return D3D12_BLEND_OP_SUBTRACT;
				case ERHIBlendOp::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
				case ERHIBlendOp::Min:             return D3D12_BLEND_OP_MIN;
				case ERHIBlendOp::Max:             return D3D12_BLEND_OP_MAX;
				default:                           return D3D12_BLEND_OP_ADD;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI COMPARISON - D3D12 COMPARISON
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIComparison>
		{
			using FromType = D3D12_COMPARISON_FUNC;
			static constexpr ERHIComparison From( D3D12_COMPARISON_FUNC a_Comparison )
			{
				switch ( a_Comparison )
				{
				case D3D12_COMPARISON_FUNC_NEVER:        return ERHIComparison::Never;
				case D3D12_COMPARISON_FUNC_LESS:         return ERHIComparison::Less;
				case D3D12_COMPARISON_FUNC_EQUAL:        return ERHIComparison::Equal;
				case D3D12_COMPARISON_FUNC_LESS_EQUAL:   return ERHIComparison::LessEqual;
				case D3D12_COMPARISON_FUNC_GREATER:      return ERHIComparison::Greater;
				case D3D12_COMPARISON_FUNC_NOT_EQUAL:    return ERHIComparison::NotEqual;
				case D3D12_COMPARISON_FUNC_GREATER_EQUAL: return ERHIComparison::GreaterEqual;
				case D3D12_COMPARISON_FUNC_ALWAYS:       return ERHIComparison::Always;
				default:                                 return ERHIComparison::Never;
				}
			}
		};

		template<>
		struct To<D3D12_COMPARISON_FUNC>
		{
			using FromType = ERHIComparison;
			static constexpr D3D12_COMPARISON_FUNC From( ERHIComparison a_Comparison )
			{
				switch ( a_Comparison )
				{
				case ERHIComparison::Never:        return D3D12_COMPARISON_FUNC_NEVER;
				case ERHIComparison::Less:         return D3D12_COMPARISON_FUNC_LESS;
				case ERHIComparison::Equal:        return D3D12_COMPARISON_FUNC_EQUAL;
				case ERHIComparison::LessEqual:    return D3D12_COMPARISON_FUNC_LESS_EQUAL;
				case ERHIComparison::Greater:      return D3D12_COMPARISON_FUNC_GREATER;
				case ERHIComparison::NotEqual:     return D3D12_COMPARISON_FUNC_NOT_EQUAL;
				case ERHIComparison::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
				case ERHIComparison::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
				default:                           return D3D12_COMPARISON_FUNC_NEVER;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI STENCIL OP - D3D12 STENCIL OP
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIStencilOp>
		{
			using FromType = D3D12_STENCIL_OP;
			static constexpr ERHIStencilOp From( D3D12_STENCIL_OP a_Op )
			{
				switch ( a_Op )
				{
				case D3D12_STENCIL_OP_KEEP:    return ERHIStencilOp::Keep;
				case D3D12_STENCIL_OP_ZERO:    return ERHIStencilOp::Zero;
				case D3D12_STENCIL_OP_REPLACE: return ERHIStencilOp::Replace;
				case D3D12_STENCIL_OP_INVERT:  return ERHIStencilOp::Invert;
				case D3D12_STENCIL_OP_INCR:    return ERHIStencilOp::Increment;
				case D3D12_STENCIL_OP_DECR:    return ERHIStencilOp::Decrement;
				default:                       return ERHIStencilOp::Keep;
				}
			}
		};

		template<>
		struct To<D3D12_STENCIL_OP>
		{
			using FromType = ERHIStencilOp;
			static constexpr D3D12_STENCIL_OP From( ERHIStencilOp a_Op )
			{
				switch ( a_Op )
				{
				case ERHIStencilOp::Keep:      return D3D12_STENCIL_OP_KEEP;
				case ERHIStencilOp::Zero:      return D3D12_STENCIL_OP_ZERO;
				case ERHIStencilOp::Replace:   return D3D12_STENCIL_OP_REPLACE;
				case ERHIStencilOp::Invert:    return D3D12_STENCIL_OP_INVERT;
				case ERHIStencilOp::Increment: return D3D12_STENCIL_OP_INCR;
				case ERHIStencilOp::Decrement: return D3D12_STENCIL_OP_DECR;
				default:                       return D3D12_STENCIL_OP_KEEP;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI TOPOLOGY - D3D12 TOPOLOGY
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHITopology>
		{
			using FromType = D3D12_PRIMITIVE_TOPOLOGY_TYPE;
			static constexpr ERHITopology From( D3D12_PRIMITIVE_TOPOLOGY_TYPE a_Topology )
			{
				switch ( a_Topology )
				{
				case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:    return ERHITopology::Point;
				case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:     return ERHITopology::Line;
				case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE: return ERHITopology::Triangle;
				default:                                     return ERHITopology::Unknown;
				}
			}
		};

		template<>
		struct To<D3D12_PRIMITIVE_TOPOLOGY_TYPE>
		{
			using FromType = ERHITopology;
			static constexpr D3D_PRIMITIVE_TOPOLOGY From( ERHITopology a_Topology )
			{
				switch ( a_Topology )
				{
				case ERHITopology::Point:         return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
				case ERHITopology::Line:          return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
				case ERHITopology::Triangle:      return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				case ERHITopology::LineStrip:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
				case ERHITopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
				default:                          return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI TEXTURE FORMAT - D3D12 FORMAT
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIFormat>
		{
			using FromType = DXGI_FORMAT;
			static constexpr ERHIFormat From( DXGI_FORMAT a_Format )
			{
				switch ( a_Format )
				{
					case DXGI_FORMAT_UNKNOWN:              return ERHIFormat::Unknown;

					// 8-bit Unsigned-Normalized
					case DXGI_FORMAT_R8_UNORM:             return ERHIFormat::R8_UNORM;
					case DXGI_FORMAT_R8G8_UNORM:           return ERHIFormat::RG8_UNORM;
					case DXGI_FORMAT_R8G8B8A8_UNORM:       return ERHIFormat::RGBA8_UNORM;

					// 8-bit Signed-Integer
					case DXGI_FORMAT_R8_SINT:              return ERHIFormat::R8_SINT;
					case DXGI_FORMAT_R8G8_SINT:            return ERHIFormat::RG8_SINT;
					case DXGI_FORMAT_R8G8B8A8_SINT:        return ERHIFormat::RGBA8_SINT;

					// 8-bit Unsigned-Integer
					case DXGI_FORMAT_R8_UINT:              return ERHIFormat::R8_UINT;
					case DXGI_FORMAT_R8G8_UINT:            return ERHIFormat::RG8_UINT;
					case DXGI_FORMAT_R8G8B8A8_UINT:        return ERHIFormat::RGBA8_UINT;

					// 16-bit Unsigned-Normalized
					case DXGI_FORMAT_R16_UNORM:            return ERHIFormat::R16_UNORM;
					case DXGI_FORMAT_R16G16_UNORM:         return ERHIFormat::RG16_UNORM;
					case DXGI_FORMAT_R16G16B16A16_UNORM:   return ERHIFormat::RGBA16_UNORM;

					// 16-bit Float
					case DXGI_FORMAT_R16_FLOAT:            return ERHIFormat::R16_FLOAT;
					case DXGI_FORMAT_R16G16_FLOAT:         return ERHIFormat::RG16_FLOAT;
					case DXGI_FORMAT_R16G16B16A16_FLOAT:   return ERHIFormat::RGBA16_FLOAT;

					// 16-bit Signed-Integer
					case DXGI_FORMAT_R16_SINT:             return ERHIFormat::R16_SINT;
					case DXGI_FORMAT_R16G16_SINT:          return ERHIFormat::RG16_SINT;
					case DXGI_FORMAT_R16G16B16A16_SINT:    return ERHIFormat::RGBA16_SINT;

					// 16-bit Unsigned-Integer
					case DXGI_FORMAT_R16_UINT:             return ERHIFormat::R16_UINT;
					case DXGI_FORMAT_R16G16_UINT:          return ERHIFormat::RG16_UINT;
					case DXGI_FORMAT_R16G16B16A16_UINT:    return ERHIFormat::RGBA16_UINT;

					// 32-bit Float
					case DXGI_FORMAT_R32_FLOAT:            return ERHIFormat::R32_FLOAT;
					case DXGI_FORMAT_R32G32_FLOAT:         return ERHIFormat::RG32_FLOAT;
					case DXGI_FORMAT_R32G32B32_FLOAT:      return ERHIFormat::RGB32_FLOAT;
					case DXGI_FORMAT_R32G32B32A32_FLOAT:   return ERHIFormat::RGBA32_FLOAT;

					// 32-bit Signed-Integer
					case DXGI_FORMAT_R32_SINT:             return ERHIFormat::R32_SINT;
					case DXGI_FORMAT_R32G32_SINT:          return ERHIFormat::RG32_SINT;
					case DXGI_FORMAT_R32G32B32_SINT:       return ERHIFormat::RGB32_SINT;
					case DXGI_FORMAT_R32G32B32A32_SINT:    return ERHIFormat::RGBA32_SINT;

					// 32-bit Unsigned-Integer
					case DXGI_FORMAT_R32_UINT:             return ERHIFormat::R32_UINT;
					case DXGI_FORMAT_R32G32_UINT:          return ERHIFormat::RG32_UINT;
					case DXGI_FORMAT_R32G32B32_UINT:       return ERHIFormat::RGB32_UINT;
					case DXGI_FORMAT_R32G32B32A32_UINT:    return ERHIFormat::RGBA32_UINT;

					// SRGB
					case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:  return ERHIFormat::SRGBA8_UNORM;

					// Depth-Stencil
					case DXGI_FORMAT_D16_UNORM:            return ERHIFormat::D16_UNORM;
					case DXGI_FORMAT_D32_FLOAT:            return ERHIFormat::D32_FLOAT;
					case DXGI_FORMAT_D24_UNORM_S8_UINT:    return ERHIFormat::D24_UNORM_S8_UINT;

					// Compressed
					case DXGI_FORMAT_BC1_UNORM:            return ERHIFormat::BC1_UNORM;
					case DXGI_FORMAT_BC3_UNORM:            return ERHIFormat::BC3_UNORM;
					case DXGI_FORMAT_BC4_UNORM:            return ERHIFormat::BC4_UNORM;
					case DXGI_FORMAT_BC5_UNORM:            return ERHIFormat::BC5_UNORM;
					case DXGI_FORMAT_BC7_UNORM:            return ERHIFormat::BC7_UNORM;

					default:                               return ERHIFormat::Unknown;
				}
			}
		};
		template<>
		struct To<DXGI_FORMAT>
		{
			static constexpr DXGI_FORMAT From( ERHIFormat a_Format )
			{
				switch ( a_Format )
				{
					case ERHIFormat::Unknown:           return DXGI_FORMAT_UNKNOWN;

					// 8-bit Unsigned-Normalized
					case ERHIFormat::R8_UNORM:          return DXGI_FORMAT_R8_UNORM;
					case ERHIFormat::RG8_UNORM:         return DXGI_FORMAT_R8G8_UNORM;
					case ERHIFormat::RGB8_UNORM:        return DXGI_FORMAT_UNKNOWN; // DXGI does not support RGB8_UNORM directly
					case ERHIFormat::RGBA8_UNORM:       return DXGI_FORMAT_R8G8B8A8_UNORM;

					// 8-bit Float (not natively supported in DXGI)
					case ERHIFormat::R8_FLOAT:          return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RG8_FLOAT:         return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGB8_FLOAT:        return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA8_FLOAT:       return DXGI_FORMAT_UNKNOWN;

					// 8-bit Signed-Integer
					case ERHIFormat::R8_SINT:           return DXGI_FORMAT_R8_SINT;
					case ERHIFormat::RG8_SINT:          return DXGI_FORMAT_R8G8_SINT;
					case ERHIFormat::RGB8_SINT:         return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA8_SINT:        return DXGI_FORMAT_R8G8B8A8_SINT;

					// 8-bit Unsigned-Integer
					case ERHIFormat::R8_UINT:           return DXGI_FORMAT_R8_UINT;
					case ERHIFormat::RG8_UINT:          return DXGI_FORMAT_R8G8_UINT;
					case ERHIFormat::RGB8_UINT:         return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA8_UINT:        return DXGI_FORMAT_R8G8B8A8_UINT;

					// 16-bit Unsigned-Normalized
					case ERHIFormat::R16_UNORM:         return DXGI_FORMAT_R16_UNORM;
					case ERHIFormat::RG16_UNORM:        return DXGI_FORMAT_R16G16_UNORM;
					case ERHIFormat::RGB16_UNORM:       return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA16_UNORM:      return DXGI_FORMAT_R16G16B16A16_UNORM;

					// 16-bit Float
					case ERHIFormat::R16_FLOAT:         return DXGI_FORMAT_R16_FLOAT;
					case ERHIFormat::RG16_FLOAT:        return DXGI_FORMAT_R16G16_FLOAT;
					case ERHIFormat::RGB16_FLOAT:       return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA16_FLOAT:      return DXGI_FORMAT_R16G16B16A16_FLOAT;

					// 16-bit Signed-Integer
					case ERHIFormat::R16_SINT:          return DXGI_FORMAT_R16_SINT;
					case ERHIFormat::RG16_SINT:         return DXGI_FORMAT_R16G16_SINT;
					case ERHIFormat::RGB16_SINT:        return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA16_SINT:       return DXGI_FORMAT_R16G16B16A16_SINT;

					// 16-bit Unsigned-Integer
					case ERHIFormat::R16_UINT:          return DXGI_FORMAT_R16_UINT;
					case ERHIFormat::RG16_UINT:         return DXGI_FORMAT_R16G16_UINT;
					case ERHIFormat::RGB16_UINT:        return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA16_UINT:       return DXGI_FORMAT_R16G16B16A16_UINT;

					// 32-bit Unsigned-Normalized (DXGI doesn't support *_UNORM for 32-bit)
					case ERHIFormat::R32_UNORM:         return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RG32_UNORM:        return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGB32_UNORM:       return DXGI_FORMAT_UNKNOWN;
					case ERHIFormat::RGBA32_UNORM:      return DXGI_FORMAT_UNKNOWN;

					// 32-bit Float
					case ERHIFormat::R32_FLOAT:         return DXGI_FORMAT_R32_FLOAT;
					case ERHIFormat::RG32_FLOAT:        return DXGI_FORMAT_R32G32_FLOAT;
					case ERHIFormat::RGB32_FLOAT:       return DXGI_FORMAT_R32G32B32_FLOAT;
					case ERHIFormat::RGBA32_FLOAT:      return DXGI_FORMAT_R32G32B32A32_FLOAT;

					// 32-bit Signed-Integer
					case ERHIFormat::R32_SINT:          return DXGI_FORMAT_R32_SINT;
					case ERHIFormat::RG32_SINT:         return DXGI_FORMAT_R32G32_SINT;
					case ERHIFormat::RGB32_SINT:        return DXGI_FORMAT_R32G32B32_SINT;
					case ERHIFormat::RGBA32_SINT:       return DXGI_FORMAT_R32G32B32A32_SINT;

					// 32-bit Unsigned-Integer
					case ERHIFormat::R32_UINT:          return DXGI_FORMAT_R32_UINT;
					case ERHIFormat::RG32_UINT:         return DXGI_FORMAT_R32G32_UINT;
					case ERHIFormat::RGB32_UINT:        return DXGI_FORMAT_R32G32B32_UINT;
					case ERHIFormat::RGBA32_UINT:       return DXGI_FORMAT_R32G32B32A32_UINT;

					// SRGB
					case ERHIFormat::SRGBA8_UNORM:      return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

					// Depth-Stencil
					case ERHIFormat::D16_UNORM:         return DXGI_FORMAT_D16_UNORM;
					case ERHIFormat::D32_FLOAT:         return DXGI_FORMAT_D32_FLOAT;
					case ERHIFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;

					// Compressed Formats
					case ERHIFormat::BC1_UNORM:         return DXGI_FORMAT_BC1_UNORM;
					case ERHIFormat::BC3_UNORM:         return DXGI_FORMAT_BC3_UNORM;
					case ERHIFormat::BC4_UNORM:         return DXGI_FORMAT_BC4_UNORM;
					case ERHIFormat::BC5_UNORM:         return DXGI_FORMAT_BC5_UNORM;
					case ERHIFormat::BC7_UNORM:         return DXGI_FORMAT_BC7_UNORM;

					default:						    return DXGI_FORMAT_UNKNOWN;
				}
			}

			static constexpr DXGI_FORMAT From( ERHIDataType a_Type )
			{
				switch ( a_Type )
				{
				case ERHIDataType::UInt8:   return DXGI_FORMAT_R8_UINT;
				case ERHIDataType::UInt16:  return DXGI_FORMAT_R16_UINT;
				case ERHIDataType::UInt32:  return DXGI_FORMAT_R32_UINT;
				case ERHIDataType::Int8:    return DXGI_FORMAT_R8_SINT;
				case ERHIDataType::Int16:   return DXGI_FORMAT_R16_SINT;
				case ERHIDataType::Int32:   return DXGI_FORMAT_R32_SINT;
				case ERHIDataType::Float16: return DXGI_FORMAT_R16_FLOAT;
				case ERHIDataType::Float32: return DXGI_FORMAT_R32_FLOAT;
				default:                    return DXGI_FORMAT_UNKNOWN;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI SHADER VISIBILITY - D3D12 SHADER VISIBILITY
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIShaderVisibility>
		{
			using FromType = D3D12_SHADER_VISIBILITY;
			static constexpr ERHIShaderVisibility From( D3D12_SHADER_VISIBILITY a_Visibility )
			{
				switch ( a_Visibility )
				{
				case D3D12_SHADER_VISIBILITY_ALL:     return ERHIShaderVisibility::All;
				case D3D12_SHADER_VISIBILITY_VERTEX:  return ERHIShaderVisibility::Vertex;
				case D3D12_SHADER_VISIBILITY_HULL:    return ERHIShaderVisibility::Hull;
				case D3D12_SHADER_VISIBILITY_DOMAIN:  return ERHIShaderVisibility::Domain;
				case D3D12_SHADER_VISIBILITY_GEOMETRY: return ERHIShaderVisibility::Geometry;
				case D3D12_SHADER_VISIBILITY_PIXEL:   return ERHIShaderVisibility::Pixel;
				default:                              return ERHIShaderVisibility::All;
				}
			}
		};

		template<>
		struct To<D3D12_SHADER_VISIBILITY>
		{
			using FromType = ERHIShaderVisibility;
			static constexpr D3D12_SHADER_VISIBILITY From( ERHIShaderVisibility a_Visibility )
			{
				switch ( a_Visibility )
				{
				case ERHIShaderVisibility::All:      return D3D12_SHADER_VISIBILITY_ALL;
				case ERHIShaderVisibility::Vertex:   return D3D12_SHADER_VISIBILITY_VERTEX;
				case ERHIShaderVisibility::Hull:     return D3D12_SHADER_VISIBILITY_HULL;
				case ERHIShaderVisibility::Domain:   return D3D12_SHADER_VISIBILITY_DOMAIN;
				case ERHIShaderVisibility::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
				case ERHIShaderVisibility::Pixel:    return D3D12_SHADER_VISIBILITY_PIXEL;
				default:                             return D3D12_SHADER_VISIBILITY_ALL;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI RESOURCE STATE - D3D12 RESOURCE STATES
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_RESOURCE_STATES>
		{
			using FromType = ERHIResourceStates;
			static constexpr D3D12_RESOURCE_STATES From( ERHIResourceStates a_State )
			{
				const EnumFlags stateFlags = EnumFlags( a_State );
				D3D12_RESOURCE_STATES d3d12State = D3D12_RESOURCE_STATE_COMMON;

				if ( stateFlags.HasFlag( ERHIResourceStates::CopySource ) )
					d3d12State |= D3D12_RESOURCE_STATE_COPY_SOURCE;
				if ( stateFlags.HasFlag( ERHIResourceStates::CopyDest ) )
					d3d12State |= D3D12_RESOURCE_STATE_COPY_DEST;
				if ( stateFlags.HasFlag( ERHIResourceStates::RenderTarget ) )
					d3d12State |= D3D12_RESOURCE_STATE_RENDER_TARGET;
				if ( stateFlags.HasFlag( ERHIResourceStates::DepthStencilWrite ) )
					d3d12State |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
				if ( stateFlags.HasFlag( ERHIResourceStates::DepthStencilRead ) )
					d3d12State |= D3D12_RESOURCE_STATE_DEPTH_READ;
				if ( stateFlags.HasFlag( ERHIResourceStates::ShaderResource ) )
					d3d12State |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				if ( stateFlags.HasFlag( ERHIResourceStates::UnorderedAccess ) )
					d3d12State |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				if ( stateFlags.HasFlag( ERHIResourceStates::IndirectArgument ) )
					d3d12State |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
				if ( stateFlags.HasFlag( ERHIResourceStates::VertexBuffer ) )
					d3d12State |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				if ( stateFlags.HasFlag( ERHIResourceStates::IndexBuffer ) )
					d3d12State |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
				if ( stateFlags.HasFlag( ERHIResourceStates::ConstantBuffer ) )
					d3d12State |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				if ( stateFlags.HasFlag( ERHIResourceStates::Present ) )
					d3d12State |= D3D12_RESOURCE_STATE_PRESENT;

				return d3d12State;
			}
		};

		template<>
		struct To<ERHIResourceStates>
		{
			using FromType = D3D12_RESOURCE_STATES;
			static constexpr ERHIResourceStates From( D3D12_RESOURCE_STATES a_State )
			{
				switch ( a_State )
				{
				case D3D12_RESOURCE_STATE_COMMON:                     return ERHIResourceStates::Common;
				case D3D12_RESOURCE_STATE_COPY_SOURCE:                return ERHIResourceStates::CopySource;
				case D3D12_RESOURCE_STATE_COPY_DEST:                  return ERHIResourceStates::CopyDest;
				case D3D12_RESOURCE_STATE_RENDER_TARGET:              return ERHIResourceStates::RenderTarget;
				case D3D12_RESOURCE_STATE_DEPTH_WRITE:                return ERHIResourceStates::DepthStencilWrite;
				case D3D12_RESOURCE_STATE_DEPTH_READ:                 return ERHIResourceStates::DepthStencilRead;
				case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE:      return ERHIResourceStates::ShaderResource;
				case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:  return ERHIResourceStates::ShaderResource;
				case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:           return ERHIResourceStates::UnorderedAccess;
				case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT:          return ERHIResourceStates::IndirectArgument;
				case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return ERHIResourceStates::VertexBuffer;
				case D3D12_RESOURCE_STATE_INDEX_BUFFER:               return ERHIResourceStates::IndexBuffer;
				//case D3D12_RESOURCE_STATE_PRESENT:                    return ERHIResourceStates::Present;
				default:                                              return ERHIResourceStates::Unknown;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI SAMPLER FILTER - D3D12 FILTER
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_FILTER>
		{
			using FromType = ERHISamplerFilter;
			static constexpr D3D12_FILTER From( ERHISamplerFilter a_Filter )
			{
				using enum ERHISamplerFilter;
				switch ( a_Filter )
				{
				case MinMagMipPoint:						return D3D12_FILTER_MIN_MAG_MIP_POINT;
				case MinMagPointMipLinear:					return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				case MinPointMagLinearMipPoint:				return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case MinPointMagMipLinear:					return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				case MinLinearMagMipPoint:					return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				case MinLinearMagPointMipLinear:			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case MinMagLinearMipPoint:					return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				case MinMagMipLinear:						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				case Anisotropic:							return D3D12_FILTER_ANISOTROPIC;
				case ComparisonMinMagMipPoint:				return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
				case ComparisonMinMagPointMipLinear:		return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
				case ComparisonMinPointMagLinearMipPoint:	return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case ComparisonMinPointMagMipLinear:		return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
				case ComparisonMinLinearMagMipPoint:		return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
				case ComparisonMinLinearMagPointMipLinear:	return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case ComparisonMinMagLinearMipPoint:		return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
				case ComparisonMinMagMipLinear:				return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
				case ComparisonAnisotropic:					return D3D12_FILTER_COMPARISON_ANISOTROPIC;
				}

				RHI_DEV_CHECK( false, "Unknown sampler filter type." );
				return D3D12_FILTER_MIN_MAG_MIP_POINT; // Default fallback
			}
		};

		template<>
		struct To<ERHISamplerFilter>
		{
			using FromType = D3D12_FILTER;
			static constexpr ERHISamplerFilter From( D3D12_FILTER a_Filter )
			{
				using enum ERHISamplerFilter;
				switch ( a_Filter )
				{
				case D3D12_FILTER_MIN_MAG_MIP_POINT:							return MinMagMipPoint;
				case D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR:						return MinMagPointMipLinear;
				case D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:				return MinPointMagLinearMipPoint;
				case D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR:						return MinPointMagMipLinear;
				case D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT:						return MinLinearMagMipPoint;
				case D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:				return MinLinearMagPointMipLinear;
				case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT:						return MinMagLinearMipPoint;
				case D3D12_FILTER_MIN_MAG_MIP_LINEAR:							return MinMagMipLinear;
				case D3D12_FILTER_ANISOTROPIC:									return Anisotropic;
				case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT:					return ComparisonMinMagMipPoint;
				case D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:			return ComparisonMinMagPointMipLinear;
				case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:	return ComparisonMinPointMagLinearMipPoint;
				case D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:			return ComparisonMinPointMagMipLinear;
				case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:			return ComparisonMinLinearMagMipPoint;
				case D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:   return ComparisonMinLinearMagPointMipLinear;
				case D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:		    return ComparisonMinMagLinearMipPoint;
				case D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:				return ComparisonMinMagMipLinear;
				case D3D12_FILTER_COMPARISON_ANISOTROPIC:					    return ComparisonAnisotropic;
				}

				RHI_DEV_CHECK( false, "Unknown sampler filter type." );
				return Unknown; // Default fallback
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI SAMPLER ADDRESS MODE - D3D12 TEXTURE ADDRESS MODE
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHISamplerAddressMode>
		{
			using FromType = D3D12_TEXTURE_ADDRESS_MODE;
			static constexpr ERHISamplerAddressMode From( D3D12_TEXTURE_ADDRESS_MODE a_Mode )
			{
				switch ( a_Mode )
				{
				case D3D12_TEXTURE_ADDRESS_MODE_WRAP:    return ERHISamplerAddressMode::Repeat;
				case D3D12_TEXTURE_ADDRESS_MODE_MIRROR:  return ERHISamplerAddressMode::Mirror;
				case D3D12_TEXTURE_ADDRESS_MODE_CLAMP:   return ERHISamplerAddressMode::Clamp;
				case D3D12_TEXTURE_ADDRESS_MODE_BORDER:  return ERHISamplerAddressMode::Border;
				default:                                 return ERHISamplerAddressMode::Repeat;
				}
			}
		};

		template<>
		struct To<D3D12_TEXTURE_ADDRESS_MODE>
		{
			using FromType = ERHISamplerAddressMode;
			static constexpr D3D12_TEXTURE_ADDRESS_MODE From( ERHISamplerAddressMode a_Mode )
			{
				switch ( a_Mode )
				{
				case ERHISamplerAddressMode::Repeat:  return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				case ERHISamplerAddressMode::Mirror:  return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				case ERHISamplerAddressMode::Clamp:   return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				case ERHISamplerAddressMode::Border:  return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				}

				RHI_DEV_CHECK( false, "Invalid sampler address mode" );
				return D3D12_TEXTURE_ADDRESS_MODE_WRAP; // Default fallback
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI SAMPLER
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_SAMPLER_DESC>
		{
			using FromType = RHISampler;
			static constexpr D3D12_SAMPLER_DESC From( const RHISampler& a_Desc )
			{
				D3D12_SAMPLER_DESC desc = {};
				desc.Filter = To<D3D12_FILTER>::From( a_Desc.Filter );
				desc.AddressU = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressU );
				desc.AddressV = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressV );
				desc.AddressW = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressW );
				desc.MaxAnisotropy = a_Desc.MaxAnisotropy;
				desc.ComparisonFunc = To<D3D12_COMPARISON_FUNC>::From( a_Desc.ComparisonFunc );
				desc.MipLODBias = a_Desc.MipLODBias;
				desc.BorderColor[0] = a_Desc.BorderColor[0];
				desc.BorderColor[1] = a_Desc.BorderColor[1];
				desc.BorderColor[2] = a_Desc.BorderColor[2];
				desc.BorderColor[3] = a_Desc.BorderColor[3];
				desc.MinLOD = a_Desc.MinLOD;
				desc.MaxLOD = a_Desc.MaxLOD;
				return desc;
			}
		};

		template<>
		struct To<D3D12_STATIC_SAMPLER_DESC>
		{
			using FromType = RHISampler;
			static constexpr D3D12_STATIC_SAMPLER_DESC From( const RHISampler& a_Desc )
			{
				D3D12_STATIC_SAMPLER_DESC desc = {};
				desc.Filter = To<D3D12_FILTER>::From( a_Desc.Filter );
				desc.AddressU = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressU );
				desc.AddressV = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressV );
				desc.AddressW = To<D3D12_TEXTURE_ADDRESS_MODE>::From( a_Desc.AddressW );
				desc.MaxAnisotropy = a_Desc.MaxAnisotropy;
				desc.ComparisonFunc = To<D3D12_COMPARISON_FUNC>::From( a_Desc.ComparisonFunc );
				desc.MipLODBias = a_Desc.MipLODBias;
				desc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				desc.MinLOD = a_Desc.MinLOD;
				desc.MaxLOD = a_Desc.MaxLOD;
				return desc;
			}
		};


		//////////////////////////////////////////////////////////////////////////
		// RHI DESCRIPTOR HEAP TYPE - D3D12 DESCRIPTOR HEAP TYPE
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIDescriptorHeapType>
		{
			using FromType = D3D12_DESCRIPTOR_HEAP_TYPE;
			static constexpr ERHIDescriptorHeapType From( D3D12_DESCRIPTOR_HEAP_TYPE a_Type )
			{
				switch ( a_Type )
				{
				case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:         return ERHIDescriptorHeapType::Sampler;
				case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:     return ERHIDescriptorHeapType::RenderResource;
				case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:             return ERHIDescriptorHeapType::RenderTarget;
				case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:             return ERHIDescriptorHeapType::DepthStencil;
				default:                                         return ERHIDescriptorHeapType::RenderResource;
				}
			}
		};

		template<>
		struct To<D3D12_DESCRIPTOR_HEAP_TYPE>
		{
			using FromType = ERHIDescriptorHeapType;
			static constexpr D3D12_DESCRIPTOR_HEAP_TYPE From( ERHIDescriptorHeapType a_Type )
			{
				switch ( a_Type )
				{
				case ERHIDescriptorHeapType::Sampler:         return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				case ERHIDescriptorHeapType::RenderResource:  return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				case ERHIDescriptorHeapType::RenderTarget:    return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				case ERHIDescriptorHeapType::DepthStencil:    return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				default:                                      return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI TEXTURE DIMENSION - D3D12 RESOURCE DIMENSION & D3D12 SRV DIMENSION
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHITextureDimension>
		{
			static constexpr ERHITextureDimension From( D3D12_RESOURCE_DIMENSION a_Type )
			{
				switch ( a_Type )
				{
				case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return ERHITextureDimension::Texture1D;
				case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return ERHITextureDimension::Texture2D;
				case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return ERHITextureDimension::Texture3D;
				default:                                 return ERHITextureDimension::Texture2D;
				}
			}

			static constexpr ERHITextureDimension From( D3D12_SRV_DIMENSION a_Type )
			{
				switch ( a_Type )
				{
				case D3D12_SRV_DIMENSION_TEXTURE1D:        return ERHITextureDimension::Texture1D;
					//case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:   return ERHITextureDimension::Texture1D;
				case D3D12_SRV_DIMENSION_TEXTURE2D:        return ERHITextureDimension::Texture2D;
					//case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:   return ERHITextureDimension::Texture2D;
				case D3D12_SRV_DIMENSION_TEXTURE3D:        return ERHITextureDimension::Texture3D;
				case D3D12_SRV_DIMENSION_TEXTURECUBE:      return ERHITextureDimension::TextureCube;
					//case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY: return ERHITextureDimension::TextureCube;
				default:                                   return ERHITextureDimension::Unknown;
				}
			}
		};

		template<>
		struct To<D3D12_RESOURCE_DIMENSION>
		{
			using FromType = ERHITextureDimension;
			static constexpr D3D12_RESOURCE_DIMENSION From( ERHITextureDimension a_Type )
			{
				switch ( a_Type )
				{
				case ERHITextureDimension::Texture1D:   return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				case ERHITextureDimension::Texture2D:   return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				case ERHITextureDimension::Texture3D:   return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				default:                                return D3D12_RESOURCE_DIMENSION_UNKNOWN;
				}
			}
		};

		template<>
		struct To<D3D12_SRV_DIMENSION>
		{
			using FromType = ERHITextureDimension;
			static constexpr D3D12_SRV_DIMENSION From( ERHITextureDimension a_Type, bool a_IsArray )
			{
				switch ( a_Type )
				{
				case ERHITextureDimension::Texture1D:      return a_IsArray ? D3D12_SRV_DIMENSION_TEXTURE1DARRAY : D3D12_SRV_DIMENSION_TEXTURE1D;
				case ERHITextureDimension::Texture2D:      return a_IsArray ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D;
				case ERHITextureDimension::Texture3D:      return D3D12_SRV_DIMENSION_TEXTURE3D;
				case ERHITextureDimension::TextureCube:    return a_IsArray ? D3D12_SRV_DIMENSION_TEXTURECUBEARRAY : D3D12_SRV_DIMENSION_TEXTURECUBE;
				default:                                   return D3D12_SRV_DIMENSION_UNKNOWN;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI CLEAR FLAG - D3D12 CLEAR FLAG
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_CLEAR_FLAGS>
		{
			using FromType = ERHIClearFlags;
			static constexpr D3D12_CLEAR_FLAGS From( ERHIClearFlags a_Flag )
			{
				int flags = (D3D12_CLEAR_FLAGS)0;
				if ( bool( a_Flag & ERHIClearFlags::Depth ) ) flags |= (int)D3D12_CLEAR_FLAG_DEPTH;
				if ( bool( a_Flag & ERHIClearFlags::Stencil ) ) flags |= (int)D3D12_CLEAR_FLAG_STENCIL;
				return (D3D12_CLEAR_FLAGS)flags;
			}
		};

		template<>
		struct To<ERHIClearFlags>
		{
			using FromType = D3D12_CLEAR_FLAGS;
			static constexpr ERHIClearFlags From( D3D12_CLEAR_FLAGS a_Flag )
			{
				ERHIClearFlags flags = (ERHIClearFlags)0;
				if ( a_Flag & D3D12_CLEAR_FLAG_DEPTH ) flags |= ERHIClearFlags::Depth;
				if ( a_Flag & D3D12_CLEAR_FLAG_STENCIL ) flags |= ERHIClearFlags::Stencil;
				return flags;
			}
		};

		////////////////////////////////////////////////////////////////////////////
		// RHI COMMAND QUEUE TYPE - D3D12 COMMAND LIST TYPE
		////////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHICommandQueueType>
		{
			using FromType = D3D12_COMMAND_LIST_TYPE;
			static constexpr ERHICommandQueueType From( D3D12_COMMAND_LIST_TYPE a_Type )
			{
				switch ( a_Type )
				{
				case D3D12_COMMAND_LIST_TYPE_DIRECT:   return ERHICommandQueueType::Graphics;
				case D3D12_COMMAND_LIST_TYPE_COMPUTE:  return ERHICommandQueueType::Compute;
				case D3D12_COMMAND_LIST_TYPE_COPY:     return ERHICommandQueueType::Copy;
				default:                               return ERHICommandQueueType::Graphics;
				}
			}
		};

		template<>
		struct To<D3D12_COMMAND_LIST_TYPE>
		{
			using FromType = ERHICommandQueueType;
			static constexpr D3D12_COMMAND_LIST_TYPE From( ERHICommandQueueType a_Type )
			{
				switch ( a_Type )
				{
				case ERHICommandQueueType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
				case ERHICommandQueueType::Compute:  return D3D12_COMMAND_LIST_TYPE_COMPUTE;
				case ERHICommandQueueType::Copy:     return D3D12_COMMAND_LIST_TYPE_COPY;
				default:                             return D3D12_COMMAND_LIST_TYPE_NONE;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////////
		// RHI LOGIC OPERATION - D3D12 LOGIC OPERATION
		//////////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHILogicOp>
		{
			using FromType = D3D12_LOGIC_OP;
			static constexpr ERHILogicOp From( D3D12_LOGIC_OP a_Op )
			{
				switch ( a_Op )
				{
					using enum ERHILogicOp;
					case D3D12_LOGIC_OP_CLEAR:         return Clear;
					case D3D12_LOGIC_OP_SET:           return Set;
					case D3D12_LOGIC_OP_COPY:          return Copy;
					case D3D12_LOGIC_OP_COPY_INVERTED: return CopyInverted;
					case D3D12_LOGIC_OP_NOOP:          return NoOp;
					case D3D12_LOGIC_OP_INVERT:		   return Invert;
					case D3D12_LOGIC_OP_AND:		   return And;
					case D3D12_LOGIC_OP_NAND:		   return Nand;
					case D3D12_LOGIC_OP_OR:			   return Or;
					case D3D12_LOGIC_OP_NOR:		   return Nor;
					case D3D12_LOGIC_OP_XOR:		   return Xor;
					case D3D12_LOGIC_OP_EQUIV:		   return Eqv;
					case D3D12_LOGIC_OP_AND_REVERSE:   return AndReverse;
					case D3D12_LOGIC_OP_AND_INVERTED:  return AndInverted;
					case D3D12_LOGIC_OP_OR_REVERSE:	   return OrReverse;
					case D3D12_LOGIC_OP_OR_INVERTED:   return OrInverted;
					default:                           return NoOp;
				}
			}
		};

		template<>
		struct To<D3D12_LOGIC_OP>
		{
			using FromType = ERHILogicOp;
			static constexpr D3D12_LOGIC_OP From( ERHILogicOp a_Op )
			{
				switch ( a_Op )
				{
					using enum ERHILogicOp;
					case Clear:         return D3D12_LOGIC_OP_CLEAR;
					case Set:           return D3D12_LOGIC_OP_SET;
					case Copy:          return D3D12_LOGIC_OP_COPY;
					case CopyInverted:  return D3D12_LOGIC_OP_COPY_INVERTED;
					case NoOp:          return D3D12_LOGIC_OP_NOOP;
					case Invert:        return D3D12_LOGIC_OP_INVERT;
					case And:           return D3D12_LOGIC_OP_AND;
					case Nand:          return D3D12_LOGIC_OP_NAND;
					case Or:            return D3D12_LOGIC_OP_OR;
					case Nor:           return D3D12_LOGIC_OP_NOR;
					case Xor:           return D3D12_LOGIC_OP_XOR;
					case Eqv:           return D3D12_LOGIC_OP_EQUIV;
					case AndReverse:    return D3D12_LOGIC_OP_AND_REVERSE;
					case AndInverted:   return D3D12_LOGIC_OP_AND_INVERTED;
					case OrReverse:     return D3D12_LOGIC_OP_OR_REVERSE;
					case OrInverted:    return D3D12_LOGIC_OP_OR_INVERTED;
					default:            return D3D12_LOGIC_OP_NOOP;
				}
			}
		};

	} // namespace D3D12

} // namespace Tridium