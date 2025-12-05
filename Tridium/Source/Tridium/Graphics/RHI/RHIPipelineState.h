#pragma once
#include "RHIResource.h"
#include "RHIVertexLayout.h"
#include "RHITexture.h"
#include "RHIShader.h"
#include "RHIShaderBindings.h"

namespace Tridium {

	//========================
	// RHI Depth State
	//  Describes the depth state for the graphics pipeline.
	struct RHIDepthState
	{
		// Enable or disable depth testing.
		bool DepthTestEnabled = true;
		// Enable or disable writing to the depth buffer.
		bool DepthWriteEnabled = true;
		// A comparison function used to compare the depth of the incoming pixel/fragment against the depth buffer.
		ERHIComparison Comparison = ERHIComparison::Less;

		constexpr auto& SetDepthTestEnabled( bool a_Enabled ) { DepthTestEnabled = a_Enabled; return *this; }
		constexpr auto& SetDepthWriteEnabled( bool a_Enabled ) { DepthWriteEnabled = a_Enabled; return *this; }
		constexpr auto& SetComparison( ERHIComparison a_Comparison ) { Comparison = a_Comparison; return *this; }
	};



	//========================
	// RHI Stencil State
	//  Describes the stencil state for the graphics pipeline.
	struct RHIStencilState
	{
		struct StencilOpDesc
		{
			// The operation to perform when the stencil test fails.
			ERHIStencilOp StencilFailOp = ERHIStencilOp::Keep;
			// The operation to perform when the stencil test passes but the depth test fails.
			ERHIStencilOp DepthFailOp = ERHIStencilOp::Keep;
			// The operation to perform when both the stencil and depth tests pass.
			ERHIStencilOp PassOp = ERHIStencilOp::Keep;
			// A comparison function used to compare the stencil value of the incoming pixel/fragment against the stencil buffer.
			ERHIComparison Comparison = ERHIComparison::Always;

			constexpr auto& SetStencilFailOp( ERHIStencilOp a_Op ) { StencilFailOp = a_Op; return *this; }
			constexpr auto& SetDepthFailOp( ERHIStencilOp a_Op ) { DepthFailOp = a_Op; return *this; }
			constexpr auto& SetPassOp( ERHIStencilOp a_Op ) { PassOp = a_Op; return *this; }
			constexpr auto& SetComparison( ERHIComparison a_Comparison ) { Comparison = a_Comparison; return *this; }
		};

		// Enable or disable stencil testing.
		bool Enabled = false;
		// Identifies which bits of the stencil buffer to read.
		uint8_t ReadMask = uint8_t( ~0u );
		// Identifies which bits of the stencil buffer to write.
		uint8_t WriteMask = uint8_t( ~0u );
		// Specifies the reference value used in stencil comparisons. The stencil test compares the reference value against the stencil buffer value.
		uint8_t RefValue = 0u;
		// The operations to perform for the front-facing polygons.
		StencilOpDesc FrontFace{};
		// The operations to perform for the back-facing polygons.
		StencilOpDesc BackFace{};

		constexpr auto& SetEnabled( bool a_Enabled ) { Enabled = a_Enabled; return *this; }
		constexpr auto& SetReadMask( uint8_t a_Mask ) { ReadMask = a_Mask; return *this; }
		constexpr auto& SetWriteMask( uint8_t a_Mask ) { WriteMask = a_Mask; return *this; }
		constexpr auto& SetRefValue( uint8_t a_Value ) { RefValue = a_Value; return *this; }
		constexpr auto& SetFrontFace( const StencilOpDesc& a_Op ) { FrontFace = a_Op; return *this; }
		constexpr auto& SetBackFace( const StencilOpDesc& a_Op ) { BackFace = a_Op; return *this; }
	};



	//========================
	// RHI Blend State
	//  Describes the blending state for the graphics pipeline.
	struct RHIBlendState
	{
		struct RenderTarget
		{
			// Enable or disable blending for this render target.
			bool BlendEnabled = false;
			// Enable or disable a logical operation for blending.
			bool LogicOpEnabled = false;
			// Specifies the blend factor for RGB value output from the pixel shader.
			ERHIBlendFactor SrcColor = ERHIBlendFactor::One;
			// Specifies the blend factor for RGB value output from the render target.
			ERHIBlendFactor DstColor = ERHIBlendFactor::Zero;
			// Specifies the blend factor for alpha value output from the pixel shader.
			ERHIBlendFactor SrcAlpha = ERHIBlendFactor::One;
			// Specifies the blend factor for alpha value output from the render target.
			ERHIBlendFactor DstAlpha = ERHIBlendFactor::Zero;
			// Defines how to combine the source and destination RGB values after applying the blend factors.
			ERHIBlendOp BlendOpColor = ERHIBlendOp::Add;
			// Defines how to combine the source and destination alpha values after applying the blend factors.
			ERHIBlendOp BlendOpAlpha = ERHIBlendOp::Add;
			// Specifies the logical operation to perform when blending is enabled.
			ERHILogicOp LogicOp = ERHILogicOp::NoOp;
			// Specifies which color channels to write to the render target.
			ERHIColorMask ColorWriteMask = ERHIColorMask::RGBA;

			constexpr auto& SetBlendEnabled( bool a_Enabled ) { BlendEnabled = a_Enabled; return *this; }
			constexpr auto& SetLogicOpEnabled( bool a_Enabled ) { LogicOpEnabled = a_Enabled; return *this; }
			constexpr auto& SetSrcColor( ERHIBlendFactor a_Factor ) { SrcColor = a_Factor; return *this; }
			constexpr auto& SetDstColor( ERHIBlendFactor a_Factor ) { DstColor = a_Factor; return *this; }
			constexpr auto& SetSrcAlpha( ERHIBlendFactor a_Factor ) { SrcAlpha = a_Factor; return *this; }
			constexpr auto& SetDstAlpha( ERHIBlendFactor a_Factor ) { DstAlpha = a_Factor; return *this; }
			constexpr auto& SetBlendOpColor( ERHIBlendOp a_Op ) { BlendOpColor = a_Op; return *this; }
			constexpr auto& SetBlendOpAlpha( ERHIBlendOp a_Op ) { BlendOpAlpha = a_Op; return *this; }
			constexpr auto& SetLogicOp( ERHILogicOp a_Op ) { LogicOp = a_Op; return *this; }
			constexpr auto& SetColorWriteMask( ERHIColorMask a_Mask ) { ColorWriteMask = a_Mask; return *this; }
		};

		// Enable alpha-to-coverage for MSAA
		bool AlphaToCoverageEnabled = false;
		// If true, the blend state is independent for each render target.
		// If false, the blend state is shared across all render targets.
		bool IndependentBlendEnabled = false;
		// The blend states for each render target.
		FixedArray<RenderTarget, RHIConstants::MaxColorTargets> RenderTargets{};

		constexpr auto& SetAlphaToCoverageEnabled( bool a_Enabled ) { AlphaToCoverageEnabled = a_Enabled; return *this; }
		constexpr auto& SetIndependentBlendEnabled( bool a_Enabled ) { IndependentBlendEnabled = a_Enabled; return *this; }
		constexpr auto& SetRenderTarget( size_t a_Index, const RenderTarget& a_State ) { RenderTargets.At( a_Index ) = a_State; return *this; }
	};



	//========================
	// RHI Rasterizer State
	//  Describes the rasterization state for the graphics pipeline.
	struct RHIRasterizerState
	{
		// The culling mode to use for polygons.
		ERHICullMode CullMode = ERHICullMode::Back;
		// The fill mode to use for polygons.
		ERHIFillMode FillMode = ERHIFillMode::Solid;
		// If true, a polygon will be considered front-facing if its vertices are clockwise
		// and back-facing if counter-clockwise.
		// If false, the opposite is true.
		bool Clockwise = true;
		// Enable or disable clipping against the near and far clip planes.
		bool DepthClipEnabled = true;
		// Enable or disable antialiased lines.
		bool AnitaliasedLinesEnabled = false;
		// A default value added to the depth of each pixel.
		int32_t DepthBias = 0;
		// A multiplier that scales the given pixel's slope before adding to the pixel's depth.
		float SlopeScaledDepthBias = 0.0f;

		constexpr auto& SetCullMode( ERHICullMode a_Mode ) { CullMode = a_Mode; return *this; }
		constexpr auto& SetFillMode( ERHIFillMode a_Mode ) { FillMode = a_Mode; return *this; }
		constexpr auto& SetClockwise( bool a_Clockwise ) { Clockwise = a_Clockwise; return *this; }
		constexpr auto& SetDepthClipEnabled( bool a_Enabled ) { DepthClipEnabled = a_Enabled; return *this; }
		constexpr auto& SetAnitaliasedLinesEnabled( bool a_Enabled ) { AnitaliasedLinesEnabled = a_Enabled; return *this; }
		constexpr auto& SetDepthBias( int32_t a_Bias ) { DepthBias = a_Bias; return *this; }
		constexpr auto& SetSlopeScaledDepthBias( float a_Bias ) { SlopeScaledDepthBias = a_Bias; return *this; }
	};



	//=======================================================================
	// IRHIGraphicsPipelineState
	//  A pipeline state object that contains the state of the GPU pipeline.
	//=======================================================================

	//==============================================
	// RHI Graphics Pipeline State Descriptor
	struct RHIGraphicsPipelineStateDesc
	{
		using ResourceType = class IRHIGraphicsPipelineState;
		String Name{};
		ERHITopology Topology = ERHITopology::Unknown;
		RHIVertexLayout VertexLayout{};
		InlineArray<RHIBindingLayoutRef, RHIConstants::MaxBindingLayouts> BindingLayouts{};

		RHIShaderModuleRef VertexShader{};
		RHIShaderModuleRef HullShader{};
		RHIShaderModuleRef DomainShader{};
		RHIShaderModuleRef GeometryShader{};
		RHIShaderModuleRef PixelShader{};

		RHIBlendState BlendState{};
		RHIDepthState DepthState{};
		RHIStencilState StencilState{};
		RHIRasterizerState RasterizerState{};

		RHIFramebufferInfo FramebufferInfo{};

		IRHIShaderModule* GetShader( ERHIShaderType a_Type ) const
		{
			switch ( a_Type )
			{
				case ERHIShaderType::Vertex:     return VertexShader.get();
				case ERHIShaderType::Hull:       return HullShader.get();
				case ERHIShaderType::Domain:     return DomainShader.get();
				case ERHIShaderType::Geometry:   return GeometryShader.get();
				case ERHIShaderType::Pixel:      return PixelShader.get();
				default:                         return nullptr;
			}
		}

		auto& SetName( StringView a_Name ) { Name = a_Name; return *this; }
		auto& SetTopology( ERHITopology a_Topology ) { Topology = a_Topology; return *this; }
		auto& SetVertexLayout( const RHIVertexLayout& a_VertexLayout ) { VertexLayout = a_VertexLayout; return *this; }
		auto& AddBindingLayout( RHIBindingLayoutRef a_BindingLayout ) { BindingLayouts.EmplaceBack( std::move( a_BindingLayout ) ); return *this; }
		auto& SetVertexShader( RHIShaderModuleRef a_VertexShader ) { VertexShader = std::move( a_VertexShader ); return *this; }
		auto& SetHullShader( RHIShaderModuleRef a_HullShader ) { HullShader = std::move( a_HullShader ); return *this; }
		auto& SetDomainShader( RHIShaderModuleRef a_DomainShader ) { DomainShader = std::move( a_DomainShader ); return *this; }
		auto& SetGeometryShader( RHIShaderModuleRef a_GeometryShader ) { GeometryShader = std::move( a_GeometryShader ); return *this; }
		auto& SetPixelShader( RHIShaderModuleRef a_PixelShader ) { PixelShader = std::move( a_PixelShader ); return *this; }
		auto& SetBlendState( const RHIBlendState& a_BlendState ) { BlendState = a_BlendState; return *this; }
		auto& SetDepthState( const RHIDepthState& a_DepthState ) { DepthState = a_DepthState; return *this; }
		auto& SetStencilState( const RHIStencilState& a_StencilState ) { StencilState = a_StencilState; return *this; }
		auto& SetRasterizerState( const RHIRasterizerState& a_RasterizerState ) { RasterizerState = a_RasterizerState; return *this; }
		auto& SetFramebufferInfo( const RHIFramebufferInfo& a_FramebufferInfo ) { FramebufferInfo = a_FramebufferInfo; return *this; }
		auto& SetShader( ERHIShaderType a_Type, RHIShaderModuleRef a_Shader )
		{
			switch ( a_Type )
			{
				case ERHIShaderType::Vertex:     VertexShader = std::move( a_Shader ); break;
				case ERHIShaderType::Hull:       HullShader = std::move( a_Shader ); break;
				case ERHIShaderType::Domain:     DomainShader = std::move( a_Shader ); break;
				case ERHIShaderType::Geometry:   GeometryShader = std::move( a_Shader ); break;
				case ERHIShaderType::Pixel:      PixelShader = std::move( a_Shader ); break;
				default:                         break;
			}
			return *this;
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

namespace std {

	template<>
	struct hash<Tridium::RHIDepthState>
	{
		size_t operator()( const Tridium::RHIDepthState& a_State ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			seed = HashCombine( seed, a_State.DepthTestEnabled );
			seed = HashCombine( seed, a_State.DepthWriteEnabled );
			seed = HashCombine( seed, a_State.Comparison );

			return seed;
		}
	};

	template<>
	struct hash<Tridium::RHIStencilState>
	{
		size_t operator()( const Tridium::RHIStencilState& a_State ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			seed = HashCombine( seed, a_State.Enabled );
			seed = HashCombine( seed, a_State.ReadMask );
			seed = HashCombine( seed, a_State.WriteMask );
			seed = HashCombine( seed, a_State.RefValue );
			seed = HashCombine( seed, a_State.FrontFace.StencilFailOp );
			seed = HashCombine( seed, a_State.FrontFace.DepthFailOp );
			seed = HashCombine( seed, a_State.FrontFace.PassOp );
			seed = HashCombine( seed, a_State.FrontFace.Comparison );
			seed = HashCombine( seed, a_State.BackFace.StencilFailOp );
			seed = HashCombine( seed, a_State.BackFace.DepthFailOp );
			seed = HashCombine( seed, a_State.BackFace.PassOp );
			seed = HashCombine( seed, a_State.BackFace.Comparison );

			return seed;
		}
	};

	template<>
	struct hash<Tridium::RHIBlendState>
	{
		size_t operator()( const Tridium::RHIBlendState& a_State ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			seed = HashCombine( seed, a_State.AlphaToCoverageEnabled );
			seed = HashCombine( seed, a_State.IndependentBlendEnabled );

			for ( const auto& rt : a_State.RenderTargets )
			{
				seed = HashCombine( seed, rt.BlendEnabled );
				seed = HashCombine( seed, rt.LogicOpEnabled );
				seed = HashCombine( seed, rt.SrcColor );
				seed = HashCombine( seed, rt.DstColor );
				seed = HashCombine( seed, rt.SrcAlpha );
				seed = HashCombine( seed, rt.DstAlpha );
				seed = HashCombine( seed, rt.BlendOpColor );
				seed = HashCombine( seed, rt.BlendOpAlpha );
				seed = HashCombine( seed, rt.LogicOp );
				seed = HashCombine( seed, rt.ColorWriteMask );
			}

			return seed;
		}
	};

	template<>
	struct hash<Tridium::RHIRasterizerState>
	{
		size_t operator()( const Tridium::RHIRasterizerState& a_State ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			seed = HashCombine( seed, a_State.CullMode );
			seed = HashCombine( seed, a_State.FillMode );
			seed = HashCombine( seed, a_State.Clockwise );
			seed = HashCombine( seed, a_State.DepthClipEnabled );
			seed = HashCombine( seed, a_State.AnitaliasedLinesEnabled );
			seed = HashCombine( seed, a_State.DepthBias );
			seed = HashCombine( seed, a_State.SlopeScaledDepthBias );

			return seed;
		}
	};

	template<>
	struct hash<Tridium::RHIFramebufferInfo>
	{
		size_t operator()( const Tridium::RHIFramebufferInfo& a_Info ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			for ( const auto& format : a_Info.ColorFormats )
			{
				seed = HashCombine( seed, format );
			}

			seed = HashCombine( seed, a_Info.DepthStencilFormat );
			seed = HashCombine( seed, a_Info.SampleCount );
			seed = HashCombine( seed, a_Info.SampleQuality );

			return seed;
		}
	};

	template<>
	struct hash<Tridium::RHIGraphicsPipelineStateDesc>
	{
		size_t operator()( const Tridium::RHIGraphicsPipelineStateDesc& a_Desc ) const
		{
			using namespace Tridium;
			using Hashing::HashCombine;

			size_t seed = 0;

			seed = HashCombine( seed, a_Desc.Topology );
			seed = HashCombine( seed, a_Desc.VertexLayout );

			for ( const auto& layout : a_Desc.BindingLayouts )
			{
				seed = HashCombine( seed, layout.get() ); // Hash the pointer value
			}

			for ( ERHIShaderType type = ERHIShaderType::Vertex; type < ERHIShaderType::COUNT; type = ERHIShaderType( uint8_t( type ) + 1 ) )
			{
				auto* shader = a_Desc.GetShader( type );
				seed = HashCombine( seed, shader ); // Hash the pointer of the shader module
			}

			seed = HashCombine( seed, a_Desc.BlendState );
			seed = HashCombine( seed, a_Desc.DepthState );
			seed = HashCombine( seed, a_Desc.StencilState );
			seed = HashCombine( seed, a_Desc.RasterizerState );
			seed = HashCombine( seed, a_Desc.FramebufferInfo );

			return seed;
		}
	};

} // namespace std