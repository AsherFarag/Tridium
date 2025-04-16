#pragma once
#include "RHIResource.h"
#include "RHIVertexLayout.h"
#include "RHITexture.h"
#include "RHIShader.h"
#include "RHIShaderBindingLayout.h"

namespace Tridium {

	//========================
	// RHI Depth State
	//========================
	struct RHIDepthState
	{
		bool IsEnabled = true;
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
		ERHIBlendOp SrcFactorColor = ERHIBlendOp::SrcAlpha;
		ERHIBlendOp DstFactorColor = ERHIBlendOp::OneMinusSrcAlpha;
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
	// Common Blend States
	namespace RHIBlendStates
	{
		constexpr RHIBlendState Opaque =      { false, ERHIBlendOp::One,      ERHIBlendOp::One,              ERHIBlendOp::One,      ERHIBlendOp::One,      ERHIBlendEq::Add };
		constexpr RHIBlendState Transparent = { true,  ERHIBlendOp::SrcAlpha, ERHIBlendOp::OneMinusSrcAlpha, ERHIBlendOp::SrcAlpha, ERHIBlendOp::DstAlpha, ERHIBlendEq::Add };
		constexpr RHIBlendState Additive =    { true,  ERHIBlendOp::SrcAlpha, ERHIBlendOp::One,              ERHIBlendOp::SrcAlpha, ERHIBlendOp::DstAlpha, ERHIBlendEq::Add };
	} // namespace RHIBlendStates



	//=======================================================================
	// RHIGraphicsPipelineState
	//  A pipeline state object that contains the state of the GPU pipeline.
	//=======================================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHIGraphicsPipelineStateDescriptor, RHIGraphicsPipelineState )
	{
		ERHITopology Topology = ERHITopology::Triangle;
		RHIVertexLayout VertexLayout{};
		RHIShaderBindingLayoutRef ShaderBindingLayout{};

		RHIShaderModuleRef VertexShader{};
		RHIShaderModuleRef HullShader{};
		RHIShaderModuleRef DomainShader{};
		RHIShaderModuleRef GeometryShader{};
		RHIShaderModuleRef PixelShader{};

		RHIBlendState BlendState = RHIBlendStates::Opaque;
		RHIDepthState DepthState{};
		RHIStencilState StencilState{};
		RHIRasterizerState RasterizerState{};

		FixedArray<ERHIFormat, RHIConstants::MaxColorTargets> ColorTargetFormats = {};
		ERHIFormat DepthStencilFormat = ERHIFormat::D16_UNORM;

		const RHIShaderModuleRef& GetShader( ERHIShaderType a_Type ) const
		{
			switch ( a_Type )
			{
				case ERHIShaderType::Vertex:     return VertexShader;
				case ERHIShaderType::Hull:       return HullShader;
				case ERHIShaderType::Domain:     return DomainShader;
				case ERHIShaderType::Geometry:   return GeometryShader;
				case ERHIShaderType::Pixel:      return PixelShader;
			}

			return nullptr;
		}
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHIGraphicsPipelineState )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHIGraphicsPipelineState, ERHIResourceType::GraphicsPipelineState );
		virtual bool Commit( const RHIGraphicsPipelineStateDescriptor& a_Desc ) = 0;
	};

} // namespace Tridium