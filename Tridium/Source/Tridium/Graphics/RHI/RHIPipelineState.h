#pragma once
#include "RHIResource.h"
#include "RHIVertexLayout.h"
#include "RHITexture.h"
#include "RHIShader.h"
#include "RHIShaderBindings.h"

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
		uint8_t StencilReadMask = 0;
		uint8_t StencilWriteMask = 0;
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
	// IRHIGraphicsPipelineState
	//  A pipeline state object that contains the state of the GPU pipeline.
	//=======================================================================

	//==============================================
	// RHI Graphics Pipeline State Descriptor
	struct RHIGraphicsPipelineStateDesc
	{
		using ResourceType = class IRHIGraphicsPipelineState;
		ERHITopology Topology = ERHITopology::Unknown;
		RHIVertexLayout VertexLayout{};
		InlineArray<RHIBindingLayoutRef, RHIConstants::MaxBindingLayouts> BindingLayouts{};

		RHIShaderModuleRef VertexShader{};
		RHIShaderModuleRef HullShader{};
		RHIShaderModuleRef DomainShader{};
		RHIShaderModuleRef GeometryShader{};
		RHIShaderModuleRef PixelShader{};


		RHIBlendState BlendState = RHIBlendStates::Opaque;
		RHIDepthState DepthState{};
		RHIStencilState StencilState{};
		RHIRasterizerState RasterizerState{};

		RHIFramebufferInfo FramebufferInfo{};

		String Name{};

		IRHIShaderModule* GetShader( ERHIShaderType a_Type ) const
		{
			switch ( a_Type )
			{
				case ERHIShaderType::Vertex:     return VertexShader.get();
				case ERHIShaderType::Hull:       return HullShader.get();
				case ERHIShaderType::Domain:     return DomainShader.get();
				case ERHIShaderType::Geometry:   return GeometryShader.get();
				case ERHIShaderType::Pixel:      return PixelShader.get();
			}

			RHI_DEV_CHECK( false, "Attempting to retrieve an invalid shader type from a GraphicsPipelineState" );
			return nullptr;
		}
	};

	//==============================================
	// RHI Graphics Pipeline State Interface
	class IRHIGraphicsPipelineState : public IRHIObject
	{
		RHI_OBJECT_INTERFACE_BODY( GraphicsPipelineState )
		IRHIGraphicsPipelineState( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
			: IRHIObject( a_Device ), m_Desc( a_Desc ) 
		{}
		virtual ~IRHIGraphicsPipelineState() = default;
	};

} // namespace Tridium