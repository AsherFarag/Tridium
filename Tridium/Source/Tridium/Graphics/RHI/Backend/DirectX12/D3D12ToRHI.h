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
		constexpr auto Translate( _From a_From )
		{
			using ToType = To<_From>::FromType;
			return To<ToType>::From(a_From);
		}

		//////////////////////////////////////////////////////////////////////////
		// RHI BLEND OP - D3D12 BLEND 
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIBlendOp>
		{
			using FromType = D3D12_BLEND;
			static constexpr ERHIBlendOp From( D3D12_BLEND a_Factor )
			{
				switch ( a_Factor )
				{
				case D3D12_BLEND_ZERO:            return ERHIBlendOp::Zero;
				case D3D12_BLEND_ONE:             return ERHIBlendOp::One;
				case D3D12_BLEND_SRC_COLOR:       return ERHIBlendOp::SrcColour;
				case D3D12_BLEND_INV_SRC_COLOR:   return ERHIBlendOp::OneMinusSrcColour;
				case D3D12_BLEND_SRC_ALPHA:       return ERHIBlendOp::SrcAlpha;
				case D3D12_BLEND_INV_SRC_ALPHA:   return ERHIBlendOp::OneMinusSrcAlpha;
				case D3D12_BLEND_DEST_COLOR:      return ERHIBlendOp::DstColour;
				case D3D12_BLEND_INV_DEST_COLOR:  return ERHIBlendOp::OneMinusDstColour;
				case D3D12_BLEND_DEST_ALPHA:      return ERHIBlendOp::DstAlpha;
				case D3D12_BLEND_INV_DEST_ALPHA:  return ERHIBlendOp::OneMinusDstAlpha;
				case D3D12_BLEND_SRC_ALPHA_SAT:   return ERHIBlendOp::SrcAlphaSaturate;
				default:                          return ERHIBlendOp::Zero;
				}
			}
		};

		template<>
		struct To<D3D12_BLEND>
		{
			using FromType = ERHIBlendOp;
			static constexpr D3D12_BLEND From( ERHIBlendOp a_Factor )
			{
				switch ( a_Factor )
				{
				case ERHIBlendOp::Zero:              return D3D12_BLEND_ZERO;
				case ERHIBlendOp::One:               return D3D12_BLEND_ONE;
				case ERHIBlendOp::SrcColour:         return D3D12_BLEND_SRC_COLOR;
				case ERHIBlendOp::OneMinusSrcColour: return D3D12_BLEND_INV_SRC_COLOR;
				case ERHIBlendOp::SrcAlpha:          return D3D12_BLEND_SRC_ALPHA;
				case ERHIBlendOp::OneMinusSrcAlpha:  return D3D12_BLEND_INV_SRC_ALPHA;
				case ERHIBlendOp::DstColour:         return D3D12_BLEND_DEST_COLOR;
				case ERHIBlendOp::OneMinusDstColour: return D3D12_BLEND_INV_DEST_COLOR;
				case ERHIBlendOp::DstAlpha:          return D3D12_BLEND_DEST_ALPHA;
				case ERHIBlendOp::OneMinusDstAlpha:  return D3D12_BLEND_INV_DEST_ALPHA;
				case ERHIBlendOp::SrcAlphaSaturate:  return D3D12_BLEND_SRC_ALPHA_SAT;
				default:                             return D3D12_BLEND_ZERO;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI BLEND EQ - D3D12 BLEND EQ
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHIBlendEq>
		{
			using FromType = D3D12_BLEND_OP;
			static constexpr ERHIBlendEq From( D3D12_BLEND_OP a_Eq )
			{
				switch ( a_Eq )
				{
				case D3D12_BLEND_OP_ADD:            return ERHIBlendEq::Add;
				case D3D12_BLEND_OP_SUBTRACT:       return ERHIBlendEq::Subtract;
				case D3D12_BLEND_OP_REV_SUBTRACT:   return ERHIBlendEq::ReverseSubtract;
				case D3D12_BLEND_OP_MIN:            return ERHIBlendEq::Min;
				case D3D12_BLEND_OP_MAX:            return ERHIBlendEq::Max;
				default:                            return ERHIBlendEq::Add;
				}
			}
		};

		template<>
		struct To<D3D12_BLEND_OP>
		{
			using FromType = ERHIBlendEq;
			static constexpr D3D12_BLEND_OP From( ERHIBlendEq a_Eq )
			{
				switch ( a_Eq )
				{
				case ERHIBlendEq::Add:             return D3D12_BLEND_OP_ADD;
				case ERHIBlendEq::Subtract:        return D3D12_BLEND_OP_SUBTRACT;
				case ERHIBlendEq::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
				case ERHIBlendEq::Min:             return D3D12_BLEND_OP_MIN;
				case ERHIBlendEq::Max:             return D3D12_BLEND_OP_MAX;
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
			static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE From( ERHITopology a_Topology )
			{
				switch ( a_Topology )
				{
				case ERHITopology::Point:         return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
				case ERHITopology::Line:          return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case ERHITopology::Triangle:      return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				case ERHITopology::LineStrip:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
				case ERHITopology::TriangleStrip: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				default:                          return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI TEXTURE FORMAT - D3D12 FORMAT
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHITextureFormat>
		{
			using FromType = DXGI_FORMAT;
			static constexpr ERHITextureFormat From( DXGI_FORMAT a_Format )
			{
				switch ( a_Format )
				{
				case DXGI_FORMAT_R8_UNORM:              return ERHITextureFormat::R8;
				case DXGI_FORMAT_R8G8_UNORM:            return ERHITextureFormat::RG8;
				case DXGI_FORMAT_R8G8B8A8_UNORM:        return ERHITextureFormat::RGBA8;
				case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return ERHITextureFormat::SRGBA8;
				case DXGI_FORMAT_R16_UNORM:             return ERHITextureFormat::R16;
				case DXGI_FORMAT_R16G16_UNORM:          return ERHITextureFormat::RG16;
				case DXGI_FORMAT_R16G16B16A16_UNORM:    return ERHITextureFormat::RGBA16;
				case DXGI_FORMAT_R16_FLOAT:             return ERHITextureFormat::R16F;
				case DXGI_FORMAT_R16G16_FLOAT:          return ERHITextureFormat::RG16F;
				case DXGI_FORMAT_R16G16B16A16_FLOAT:    return ERHITextureFormat::RGBA16F;
				case DXGI_FORMAT_R32_FLOAT:             return ERHITextureFormat::R32F;
				case DXGI_FORMAT_R32G32_FLOAT:          return ERHITextureFormat::RG32F;
				case DXGI_FORMAT_R32G32B32A32_FLOAT:    return ERHITextureFormat::RGBA32F;
				case DXGI_FORMAT_R16_SINT:              return ERHITextureFormat::R16I;
				case DXGI_FORMAT_R16G16_SINT:           return ERHITextureFormat::RG16I;
				case DXGI_FORMAT_R16G16B16A16_SINT:     return ERHITextureFormat::RGBA16I;
				case DXGI_FORMAT_R32_SINT:              return ERHITextureFormat::R32I;
				case DXGI_FORMAT_R32G32_SINT:           return ERHITextureFormat::RG32I;
				case DXGI_FORMAT_R32G32B32A32_SINT:     return ERHITextureFormat::RGBA32I;
				case DXGI_FORMAT_D16_UNORM:             return ERHITextureFormat::D16;
				case DXGI_FORMAT_D32_FLOAT:             return ERHITextureFormat::D32;
				case DXGI_FORMAT_D24_UNORM_S8_UINT:     return ERHITextureFormat::D24S8;
				case DXGI_FORMAT_BC1_UNORM:             return ERHITextureFormat::BC1;
				case DXGI_FORMAT_BC3_UNORM:             return ERHITextureFormat::BC3;
				case DXGI_FORMAT_BC4_UNORM:             return ERHITextureFormat::BC4;
				case DXGI_FORMAT_BC5_UNORM:             return ERHITextureFormat::BC5;
				case DXGI_FORMAT_BC7_UNORM:             return ERHITextureFormat::BC7;
				default:                                return ERHITextureFormat::Unknown;
				}
			}
		};

		template<>
		struct To<ERHIVertexElementType>
		{
			using FromType = DXGI_FORMAT;
			static constexpr ERHIVertexElementType From( DXGI_FORMAT a_Format )
			{
				switch ( a_Format )
				{
				case DXGI_FORMAT_R32_FLOAT:          return ERHIVertexElementType::Float1;
				case DXGI_FORMAT_R32G32_FLOAT:       return ERHIVertexElementType::Float2;
				case DXGI_FORMAT_R32G32B32_FLOAT:    return ERHIVertexElementType::Float3;
				case DXGI_FORMAT_R32G32B32A32_FLOAT: return ERHIVertexElementType::Float4;
				case DXGI_FORMAT_R8G8B8A8_UINT:      return ERHIVertexElementType::UByte4;
				case DXGI_FORMAT_R32_SINT:           return ERHIVertexElementType::Int1;
				case DXGI_FORMAT_R32G32_SINT:        return ERHIVertexElementType::Int2;
				case DXGI_FORMAT_R32G32B32_SINT:     return ERHIVertexElementType::Int3;
				case DXGI_FORMAT_R32G32B32A32_SINT:  return ERHIVertexElementType::Int4;
				case DXGI_FORMAT_R32_UINT:           return ERHIVertexElementType::UInt1;
				case DXGI_FORMAT_R32G32_UINT:        return ERHIVertexElementType::UInt2;
				case DXGI_FORMAT_R32G32B32_UINT:     return ERHIVertexElementType::UInt3;
				case DXGI_FORMAT_R32G32B32A32_UINT:  return ERHIVertexElementType::UInt4;
				default:                             return ERHIVertexElementType::None;
				}
			}
		};

		template<>
		struct To<DXGI_FORMAT>
		{
			static constexpr DXGI_FORMAT From( ERHITextureFormat a_Format )
			{
				switch ( a_Format )
				{
				case ERHITextureFormat::R8:      return DXGI_FORMAT_R8_UNORM;
				case ERHITextureFormat::RG8:     return DXGI_FORMAT_R8G8_UNORM;
				case ERHITextureFormat::RGBA8:   return DXGI_FORMAT_R8G8B8A8_UNORM;
				case ERHITextureFormat::SRGBA8:  return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				case ERHITextureFormat::R16:     return DXGI_FORMAT_R16_UNORM;
				case ERHITextureFormat::RG16:    return DXGI_FORMAT_R16G16_UNORM;
				case ERHITextureFormat::RGBA16:  return DXGI_FORMAT_R16G16B16A16_UNORM;
				case ERHITextureFormat::R16F:    return DXGI_FORMAT_R16_FLOAT;
				case ERHITextureFormat::RG16F:   return DXGI_FORMAT_R16G16_FLOAT;
				case ERHITextureFormat::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case ERHITextureFormat::R32F:    return DXGI_FORMAT_R32_FLOAT;
				case ERHITextureFormat::RG32F:   return DXGI_FORMAT_R32G32_FLOAT;
				case ERHITextureFormat::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case ERHITextureFormat::R16I:    return DXGI_FORMAT_R16_SINT;
				case ERHITextureFormat::RG16I:   return DXGI_FORMAT_R16G16_SINT;
				case ERHITextureFormat::RGBA16I: return DXGI_FORMAT_R16G16B16A16_SINT;
				case ERHITextureFormat::R32I:    return DXGI_FORMAT_R32_SINT;
				case ERHITextureFormat::RG32I:   return DXGI_FORMAT_R32G32_SINT;
				case ERHITextureFormat::RGBA32I: return DXGI_FORMAT_R32G32B32A32_SINT;
				case ERHITextureFormat::D16:     return DXGI_FORMAT_D16_UNORM;
				case ERHITextureFormat::D32:     return DXGI_FORMAT_D32_FLOAT;
				case ERHITextureFormat::D24S8:   return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case ERHITextureFormat::BC1:     return DXGI_FORMAT_BC1_UNORM;
				case ERHITextureFormat::BC3:     return DXGI_FORMAT_BC3_UNORM;
				case ERHITextureFormat::BC4:     return DXGI_FORMAT_BC4_UNORM;
				case ERHITextureFormat::BC5:     return DXGI_FORMAT_BC5_UNORM;
				case ERHITextureFormat::BC7:     return DXGI_FORMAT_BC7_UNORM;
				default:                         return DXGI_FORMAT_UNKNOWN;
				}
			}

			static constexpr DXGI_FORMAT From( ERHIVertexElementType a_Format )
			{
				switch ( a_Format )
				{
				case ERHIVertexElementType::Float1: return DXGI_FORMAT_R32_FLOAT;
				case ERHIVertexElementType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
				case ERHIVertexElementType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
				case ERHIVertexElementType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case ERHIVertexElementType::UByte4: return DXGI_FORMAT_R8G8B8A8_UINT;
				case ERHIVertexElementType::Color:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case ERHIVertexElementType::Int1:   return DXGI_FORMAT_R32_SINT;
				case ERHIVertexElementType::Int2:   return DXGI_FORMAT_R32G32_SINT;
				case ERHIVertexElementType::Int3:   return DXGI_FORMAT_R32G32B32_SINT;
				case ERHIVertexElementType::Int4:   return DXGI_FORMAT_R32G32B32A32_SINT;
				case ERHIVertexElementType::UInt1:  return DXGI_FORMAT_R32_UINT;
				case ERHIVertexElementType::UInt2:  return DXGI_FORMAT_R32G32_UINT;
				case ERHIVertexElementType::UInt3:  return DXGI_FORMAT_R32G32B32_UINT;
				case ERHIVertexElementType::UInt4:  return DXGI_FORMAT_R32G32B32A32_UINT;
				default:                            return DXGI_FORMAT_UNKNOWN;
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
		// RHI USAGE HINT - D3D12 HEAP TYPE
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_HEAP_TYPE>
		{
			using FromType = ERHIUsageHint;
			static constexpr D3D12_HEAP_TYPE From( ERHIUsageHint a_Type )
			{
				if ( EnumFlags( a_Type ).HasFlag( ERHIUsageHint::CPUWriteMany ) ) return D3D12_HEAP_TYPE_UPLOAD;
				if ( EnumFlags( a_Type ).HasFlag( ERHIUsageHint::GPUWriteMany ) ) return D3D12_HEAP_TYPE_DEFAULT;
				if ( EnumFlags( a_Type ).HasFlag( ERHIUsageHint::CPUReadMany ) )  return D3D12_HEAP_TYPE_READBACK;
				return D3D12_HEAP_TYPE_DEFAULT; // Default to GPU memory
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI RESOURCE STATE - D3D12 RESOURCE STATES
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<D3D12_RESOURCE_STATES>
		{
			using FromType = ERHIResourceState;
			static constexpr D3D12_RESOURCE_STATES From( ERHIResourceState a_State )
			{
				switch ( a_State )
				{
				case ERHIResourceState::General:              return D3D12_RESOURCE_STATE_COMMON;
				case ERHIResourceState::CopySource:           return D3D12_RESOURCE_STATE_COPY_SOURCE;
				case ERHIResourceState::CopyDest:             return D3D12_RESOURCE_STATE_COPY_DEST;
				case ERHIResourceState::RenderTarget:         return D3D12_RESOURCE_STATE_RENDER_TARGET;
				case ERHIResourceState::DepthStencilWrite:    return D3D12_RESOURCE_STATE_DEPTH_WRITE;
				case ERHIResourceState::DepthStencilReadOnly: return D3D12_RESOURCE_STATE_DEPTH_READ;
				case ERHIResourceState::ShaderResource:       return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				case ERHIResourceState::UnorderedAccess:      return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				case ERHIResourceState::IndirectArgument:     return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
				case ERHIResourceState::VertexBuffer:         return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				case ERHIResourceState::IndexBuffer:          return D3D12_RESOURCE_STATE_INDEX_BUFFER;
				case ERHIResourceState::ConstantBuffer:       return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				case ERHIResourceState::Present:              return D3D12_RESOURCE_STATE_PRESENT;
				case ERHIResourceState::GPUWrite:             return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				default:                                      return D3D12_RESOURCE_STATE_COMMON;
				}
			}
		};

		template<>
		struct To<ERHIResourceState>
		{
			using FromType = D3D12_RESOURCE_STATES;
			static constexpr ERHIResourceState From( D3D12_RESOURCE_STATES a_State )
			{
				switch ( a_State )
				{
				case D3D12_RESOURCE_STATE_COMMON:                     return ERHIResourceState::General;
				case D3D12_RESOURCE_STATE_COPY_SOURCE:                return ERHIResourceState::CopySource;
				case D3D12_RESOURCE_STATE_COPY_DEST:                  return ERHIResourceState::CopyDest;
				case D3D12_RESOURCE_STATE_RENDER_TARGET:              return ERHIResourceState::RenderTarget;
				case D3D12_RESOURCE_STATE_DEPTH_WRITE:                return ERHIResourceState::DepthStencilWrite;
				case D3D12_RESOURCE_STATE_DEPTH_READ:                 return ERHIResourceState::DepthStencilReadOnly;
				case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE:      return ERHIResourceState::ShaderResource;
				case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:  return ERHIResourceState::ShaderResource;
				case D3D12_RESOURCE_STATE_UNORDERED_ACCESS:           return ERHIResourceState::UnorderedAccess;
				case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT:          return ERHIResourceState::IndirectArgument;
				case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return ERHIResourceState::VertexBuffer;
				case D3D12_RESOURCE_STATE_INDEX_BUFFER:               return ERHIResourceState::IndexBuffer;
				//case D3D12_RESOURCE_STATE_PRESENT:                  return ERHIResourceState::Present;
				default:                                              return ERHIResourceState::General;
				}
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// RHI SAMPLER FILTER - D3D12 FILTER
		//////////////////////////////////////////////////////////////////////////

		template<>
		struct To<ERHISamplerFilter>
		{
			using FromType = D3D12_FILTER;
			static constexpr ERHISamplerFilter From( D3D12_FILTER a_Filter )
			{
				switch ( a_Filter )
				{
				case D3D12_FILTER_MIN_MAG_MIP_POINT:        return ERHISamplerFilter::Point;
				case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT: return ERHISamplerFilter::Bilinear;
				case D3D12_FILTER_MIN_MAG_MIP_LINEAR:       return ERHISamplerFilter::Trilinear;
				case D3D12_FILTER_ANISOTROPIC:              return ERHISamplerFilter::AnisotropicLinear;
				default:                                    return ERHISamplerFilter::Point;
				}
			}
		};

		template<>
		struct To<D3D12_FILTER>
		{
			using FromType = ERHISamplerFilter;
			static constexpr D3D12_FILTER From( ERHISamplerFilter a_Filter )
			{
				switch ( a_Filter )
				{
				case ERHISamplerFilter::Point:             return D3D12_FILTER_MIN_MAG_MIP_POINT;
				case ERHISamplerFilter::Bilinear:          return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				case ERHISamplerFilter::Trilinear:         return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				case ERHISamplerFilter::AnisotropicPoint:  return D3D12_FILTER_ANISOTROPIC;
				case ERHISamplerFilter::AnisotropicLinear: return D3D12_FILTER_ANISOTROPIC;
				default:                                   return D3D12_FILTER_MIN_MAG_MIP_POINT;
				}
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
				default:                              return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				}
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

	} // namespace D3D12

} // namespace Tridium