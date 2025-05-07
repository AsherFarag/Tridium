#include "tripch.h"
#include "D3D12PipelineState.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ShaderBindingLayout.h"

namespace Tridium {

	namespace ToD3D12 {

		D3D12_SHADER_BYTECODE GetShaderBytecode( const RHIShaderModuleRef& a_Shader )
		{
			return a_Shader
				? D3D12_SHADER_BYTECODE( a_Shader->Descriptor().Bytecode.data(), a_Shader->Descriptor().Bytecode.size_bytes() )
				: D3D12_SHADER_BYTECODE();
		}

		D3D12_BLEND_DESC GetBlendDesc( const RHIBlendState& a_BlendState )
		{
			D3D12_BLEND_DESC desc = {};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable    = a_BlendState.IsEnabled;
			desc.RenderTarget[0].SrcBlend       = D3D12::Translate( a_BlendState.SrcFactorColor );
			desc.RenderTarget[0].DestBlend      = D3D12::Translate( a_BlendState.DstFactorColor );
			desc.RenderTarget[0].BlendOp        = D3D12::Translate( a_BlendState.BlendEquation );
			desc.RenderTarget[0].SrcBlendAlpha  = D3D12::Translate( a_BlendState.SrcFactorAlpha );
			desc.RenderTarget[0].DestBlendAlpha = D3D12::Translate( a_BlendState.DstFactorAlpha );
			desc.RenderTarget[0].BlendOpAlpha   = D3D12::Translate( a_BlendState.BlendEquation );
			desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			return desc;
		}

		D3D12_RASTERIZER_DESC GetRasterizerDesc( const RHIGraphicsPipelineStateDescriptor& a_PSD )
		{
			D3D12_RASTERIZER_DESC desc{};

			// Set the fill mode
			desc.FillMode = a_PSD.RasterizerState.FillMode == ERHIRasterizerFillMode::Wireframe
				? D3D12_FILL_MODE_WIREFRAME 
				: D3D12_FILL_MODE_SOLID;

			// Set the cull mode
			switch ( a_PSD.RasterizerState.CullMode )
			{
				using enum ERHIRasterizerCullMode;
				case Front: desc.CullMode = D3D12_CULL_MODE_FRONT; break;
				case Back:  desc.CullMode = D3D12_CULL_MODE_BACK;  break;
				case None:  desc.CullMode = D3D12_CULL_MODE_NONE;  break;
			}

			desc.FrontCounterClockwise = a_PSD.RasterizerState.Clockwise;

			// Set depth values
			desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			desc.DepthClipEnable = FALSE;

			// Set the multisampling values
			desc.MultisampleEnable = FALSE;
			desc.AntialiasedLineEnable = FALSE;
			desc.ForcedSampleCount = 0;

			return desc;
		}

		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc( const RHIGraphicsPipelineStateDescriptor& a_PSD )
		{
			D3D12_DEPTH_STENCIL_DESC desc = {};
			desc.DepthEnable = a_PSD.DepthState.IsEnabled;
			TODO( "Depth write mask" );
			desc.DepthWriteMask = a_PSD.DepthState.IsEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

			desc.DepthFunc = D3D12::Translate( a_PSD.DepthState.Comparison );
			desc.StencilEnable = a_PSD.StencilState.IsEnabled;
			desc.StencilReadMask = a_PSD.StencilState.StencilReadMask;
			desc.StencilWriteMask = a_PSD.StencilState.StencilWriteMask;
			desc.FrontFace.StencilFailOp = D3D12::Translate( a_PSD.StencilState.Fail );
			desc.FrontFace.StencilDepthFailOp = D3D12::Translate( a_PSD.StencilState.DepthFail );
			desc.FrontFace.StencilPassOp = D3D12::Translate( a_PSD.StencilState.Pass );
			desc.FrontFace.StencilFunc = D3D12::Translate( a_PSD.StencilState.Comparison );
			desc.BackFace = desc.FrontFace;
			return desc;
		}

	} // namespace ToD3D12

	bool D3D12GraphicsPipelineState::Commit( const RHIGraphicsPipelineStateDescriptor& a_Desc )
    {
		m_Desc = a_Desc;

		// Create the vertex input layout
		// We create tempory strings here of the vertex element names,
		// as RHIVertexAttribute::Name is a StringView and can be not null terminated.
		// And SemanticName requires a null terminated string.
		FixedArray<String, RHIConstants::MaxVertexAttributes> vertexElementNames;
		for ( size_t i = 0; i < a_Desc.VertexLayout.Elements.Size(); ++i )
		{
			vertexElementNames[i] = a_Desc.VertexLayout.Elements[i].Name;
			const RHIVertexAttribute& element = a_Desc.VertexLayout.Elements[i];
			VertexLayout[i] = {
				.SemanticName = vertexElementNames[i].c_str(),
				.SemanticIndex = 0,
				.Format = D3D12::Translate( element.Type ),
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<UINT>( element.Offset ),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			};
		}

		// Create the pipeline state object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		// Set the root signature
		psd.pRootSignature = a_Desc.ShaderBindingLayout->As<D3D12ShaderBindingLayout>()->m_RootSignature.Get();

		// Set the input layout
		psd.InputLayout.NumElements = a_Desc.VertexLayout.Elements.Size();
		psd.InputLayout.pInputElementDescs = VertexLayout;
		psd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Set the shaders
		psd.VS = ToD3D12::GetShaderBytecode( a_Desc.VertexShader );
		psd.HS = ToD3D12::GetShaderBytecode( a_Desc.HullShader );
		psd.DS = ToD3D12::GetShaderBytecode( a_Desc.DomainShader );
		psd.GS = ToD3D12::GetShaderBytecode( a_Desc.GeometryShader );
		psd.PS = ToD3D12::GetShaderBytecode( a_Desc.PixelShader );

		// Set the blend state
		psd.BlendState = ToD3D12::GetBlendDesc( a_Desc.BlendState );

		// Set the sample mask
		psd.SampleMask = UINT_MAX; // Enable all samples

		// Set the rasterizer state
		psd.RasterizerState = ToD3D12::GetRasterizerDesc( a_Desc );

		// Set the depth stencil state
		psd.DepthStencilState = ToD3D12::GetDepthStencilDesc( a_Desc );
		psd.DSVFormat = D3D12::Translate( a_Desc.DepthStencilFormat );

		switch ( a_Desc.Topology )
		{
			case ERHITopology::Point:         psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT; break;
			case ERHITopology::Line:          psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case ERHITopology::LineStrip:     psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case ERHITopology::Triangle:      psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
			case ERHITopology::TriangleStrip: psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
			default:         ASSERT( false ); psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED; break;
		}

		psd.NumRenderTargets = a_Desc.ColorTargetFormats.Size();
		for ( size_t i = 0; i < a_Desc.ColorTargetFormats.Size(); ++i )
		{
			psd.RTVFormats[i] = D3D12::Translate( a_Desc.ColorTargetFormats[i] );
		}
		psd.DSVFormat = D3D12::Translate( a_Desc.DepthStencilFormat );
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		psd.NodeMask = 0;
		psd.CachedPSO = { nullptr, 0 };

		// Create the pipeline state object
		if ( FAILED( GetD3D12RHI()->GetD3D12Device()->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( &PSO ) ) ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
			return false;
		}

		D3D12_SET_DEBUG_NAME( PSO.Get(), a_Desc.Name );

		return true;
    }

	bool D3D12GraphicsPipelineState::Release()
	{
		PSO.Release();
		return true;
	}

} // namespace Tridium