#pragma once
#include "RHIResource.h"
#include "RHIMesh.h"
#include "RHITexture.h"
#include "RHIShader.h"
#include "RHIShaderBindingLayout.h"

namespace Tridium {

	//========================
	// RHI Depth State
	//========================
	struct RHIDepthState
	{
		bool IsEnabled = false;
		ERHIDepthOp DepthOp = ERHIDepthOp::Replace;
		ERHIComparison Comparison = ERHIComparison::Less;
	};



	//========================
	// RHI Stencil State
	//========================
	struct RHIStencilState
	{
		bool IsEnabled = false;
		ERHIComparison Comparison = ERHIComparison::Never;
		ERHIStencilOp Fail = ERHIStencilOp::Keep;
		ERHIStencilOp DepthFail = ERHIStencilOp::Keep;
		ERHIStencilOp Pass = ERHIStencilOp::Keep;
		uint8_t StencilReadMask = 0x00;
		uint8_t StencilWriteMask = 0x00;
	};



	//========================
	// RHI Blend State
	//========================
	struct RHIBlendState
	{
		bool IsEnabled = false;
		ERHIBlendOp SrcFactorColour = ERHIBlendOp::SrcAlpha;
		ERHIBlendOp DstFactorColour = ERHIBlendOp::OneMinusSrcAlpha;
		ERHIBlendOp SrcFactorAlpha = ERHIBlendOp::SrcAlpha;
		ERHIBlendOp DstFactorAlpha = ERHIBlendOp::DstAlpha;
		ERHIBlendEq BlendEquation = ERHIBlendEq::Add;
	};



	//========================
	// RHI Rasterizer State
	//========================
	struct RHIRasterizerState
	{
		bool Conservative = true;
		bool Clockwise = true;
		ERHIRasterizerCullMode CullMode = ERHIRasterizerCullMode::Back;
		ERHIRasterizerFillMode FillMode = ERHIRasterizerFillMode::Solid;
	};



	//=======================================================================
	// RHI Pipeline Type
	//  An RHI pipeline state can only be used for Graphics or Compute.
	//  If the pipeline is used for both, then it is invalid.
	//=======================================================================
	enum class ERHIPipelineType : uint8_t
	{
		Invalid = 0,
		Graphics,
		Compute,
	};



	//=======================================================================
	// Common Blend States
	namespace RHIBlendStates
	{
		constexpr RHIBlendState Opaque =      { false, ERHIBlendOp::One,      ERHIBlendOp::One,              ERHIBlendOp::One,      ERHIBlendOp::One,      ERHIBlendEq::Add };
		constexpr RHIBlendState Transparent = { true,  ERHIBlendOp::SrcAlpha, ERHIBlendOp::OneMinusSrcAlpha, ERHIBlendOp::SrcAlpha, ERHIBlendOp::DstAlpha, ERHIBlendEq::Add };
		constexpr RHIBlendState Additive =    { true,  ERHIBlendOp::SrcAlpha, ERHIBlendOp::One,              ERHIBlendOp::SrcAlpha, ERHIBlendOp::DstAlpha, ERHIBlendEq::Add };
	} // namespace RHIBlendStates



	//=======================================================================
	// RHIPipelineState
	//  A pipeline state object that contains the state of the GPU pipeline.
	RHI_RESOURCE_BASE_TYPE( PipelineState )
	{
		ERHITopology Topology = ERHITopology::Triangle;
		RHIVertexLayout VertexLayout;
		RHIShaderBindingLayoutRef ShaderBindingLayout;

		RHIShaderModuleRef VertexShader;
		RHIShaderModuleRef HullShader;
		RHIShaderModuleRef DomainShader;
		RHIShaderModuleRef GeometryShader;
		RHIShaderModuleRef PixelShader;
		RHIShaderModuleRef ComputeShader;

		RHIBlendState BlendState = RHIBlendStates::Opaque;
		RHIDepthState DepthState;
		RHIStencilState StencilState;
		RHIRasterizerState RasterizerState;

		FixedArray<ERHITextureFormat, RHIQuery::MaxColorTargets> ColourTargetFormats = {};
		ERHITextureFormat DepthStencilFormat = ERHITextureFormat::D32;

		ERHIPipelineType GetPipelineType() const
		{
			if ( ComputeShader && ( VertexShader || HullShader || DomainShader || GeometryShader || PixelShader ) )
			{
				return ERHIPipelineType::Invalid;
			}

			if ( ComputeShader )
			{
				return ERHIPipelineType::Compute;
			}

			return ERHIPipelineType::Graphics;
		}

		const RHIShaderModuleRef& GetShader( ERHIShaderType a_Type ) const
		{
			switch ( a_Type )
			{
				case ERHIShaderType::Vertex:     return VertexShader;
				case ERHIShaderType::Hull:       return HullShader;
				case ERHIShaderType::Domain:     return DomainShader;
				case ERHIShaderType::Geometry:   return GeometryShader;
				case ERHIShaderType::Pixel:      return PixelShader;
				case ERHIShaderType::Compute:    return ComputeShader;
			}

			return nullptr;
		}
	};

} // namespace Tridium