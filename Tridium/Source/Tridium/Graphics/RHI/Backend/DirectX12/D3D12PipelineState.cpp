#include "tripch.h"
#include "D3D12PipelineState.h"
#include "D3D12RHI.h"

namespace Tridium {

	namespace Helpers {

		D3D12_SHADER_BYTECODE GetShaderBytecode( const RHIShaderModuleRef& a_Shader )
		{
			return a_Shader
				? D3D12_SHADER_BYTECODE( a_Shader->GetDescriptor()->Binary.data(), a_Shader->GetDescriptor()->Binary.size_bytes() )
				: D3D12_SHADER_BYTECODE();
		}

		D3D12_BLEND_DESC GetBlendDesc( const RHIBlendState& a_BlendState )
		{
			D3D12_BLEND_DESC desc = {};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = a_BlendState.IsEnabled;
			desc.RenderTarget[0].SrcBlend = ToD3D12::GetBlend( a_BlendState.SrcFactorColour );
			desc.RenderTarget[0].DestBlend = ToD3D12::GetBlend( a_BlendState.DstFactorColour );
			desc.RenderTarget[0].BlendOp = ToD3D12::GetBlendOp( a_BlendState.BlendEquation );
			desc.RenderTarget[0].SrcBlendAlpha = ToD3D12::GetBlend( a_BlendState.SrcFactorAlpha );
			desc.RenderTarget[0].DestBlendAlpha = ToD3D12::GetBlend( a_BlendState.DstFactorAlpha );
			desc.RenderTarget[0].BlendOpAlpha = ToD3D12::GetBlendOp( a_BlendState.BlendEquation );
			desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			return desc;
		}

		D3D12_RASTERIZER_DESC GetRasterizerDesc( const RHIPipelineStateDescriptor& a_PSD )
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

		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc( const RHIPipelineStateDescriptor& a_PSD )
		{
			D3D12_DEPTH_STENCIL_DESC desc = {};
			desc.DepthEnable = a_PSD.DepthState.IsEnabled;
			TODO( "Depth write mask" );
			desc.DepthWriteMask = a_PSD.DepthState.IsEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

			desc.DepthFunc = ToD3D12::GetComparisonFunc( a_PSD.DepthState.Comparison );
			desc.StencilEnable = a_PSD.StencilState.IsEnabled;
			desc.StencilReadMask = a_PSD.StencilState.StencilReadMask;
			desc.StencilWriteMask = a_PSD.StencilState.StencilWriteMask;
			desc.FrontFace.StencilFailOp = ToD3D12::GetStencilOp( a_PSD.StencilState.Fail );
			desc.FrontFace.StencilDepthFailOp = ToD3D12::GetStencilOp( a_PSD.StencilState.DepthFail );
			desc.FrontFace.StencilPassOp = ToD3D12::GetStencilOp( a_PSD.StencilState.Pass );
			desc.FrontFace.StencilFunc = ToD3D12::GetComparisonFunc( a_PSD.StencilState.Comparison );
			desc.BackFace = desc.FrontFace;
			return desc;
		}

	} // namespace Helpers



    bool D3D12PipelineState::Commit( const void* a_Params )
    {
		const RHIPipelineStateDescriptor* desc = static_cast<const RHIPipelineStateDescriptor*>( a_Params );
		if ( !desc || desc->GetPipelineType() == ERHIPipelineType::Invalid )
		{
			LOG( LogCategory::RHI, Error, "Invalid pipeline type" );
			return false;
		}

		// Create the input layout
		for ( size_t i = 0; i < desc->VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = desc->VertexLayout.Elements[i];
			m_VertexLayout[i] = {
				element.Name.data(), 0,
				ToD3D12::GetFormat( element.Type ), 0,
				static_cast<UINT>( element.Offset ),
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			};
		}

		// Create the pipeline state object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.pRootSignature = nullptr; // TODO: Root signature
		if ( desc->GetPipelineType() == ERHIPipelineType::Graphics )
		{
			psd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

			// Set the input layout
			psd.InputLayout.NumElements = desc->VertexLayout.Elements.Size();
			psd.InputLayout.pInputElementDescs = m_VertexLayout;
			psd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

			// Set the shaders
			psd.VS = Helpers::GetShaderBytecode( desc->VertexShader );
			psd.PS = Helpers::GetShaderBytecode( desc->PixelShader );
			psd.GS = Helpers::GetShaderBytecode( desc->GeometryShader );

			// Set the blend state
			psd.BlendState = Helpers::GetBlendDesc( desc->BlendState );

			psd.SampleMask = UINT_MAX; // Enable all samples

			// Set the rasterizer state
			psd.RasterizerState = Helpers::GetRasterizerDesc( *desc );

			psd.DepthStencilState = Helpers::GetDepthStencilDesc( *desc );
			psd.PrimitiveTopologyType = ToD3D12::GetTopologyType( desc->Topology );
			psd.NumRenderTargets = desc->ColourTargetFormats.Size();
			for ( size_t i = 0; i < desc->ColourTargetFormats.Size(); ++i )
			{
				psd.RTVFormats[i] = ToD3D12::GetFormat( desc->ColourTargetFormats[i] );
			}
			psd.DSVFormat = ToD3D12::GetFormat( desc->DepthStencilFormat );
			psd.SampleDesc.Count = 1;
			psd.SampleDesc.Quality = 0;
			psd.NodeMask = 0;
			psd.CachedPSO = { nullptr, 0 };

			// Create the pipeline state object
			ComPtr<D3D12::Device> device = RHI::GetDirectX12RHI()->GetDevice();
			if ( FAILED( device->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( &m_PipelineState ) ) ) )
			{
				LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
				return false;
			}
		}
		else if ( desc->GetPipelineType() == ERHIPipelineType::Compute )
		{
			LOG( LogCategory::RHI, Error, "Compute pipeline state not implemented" );
			return false;
		}

		return true;
    }

	bool D3D12PipelineState::Release()
	{
		return false;
	}

	bool D3D12PipelineState::IsValid() const
	{
		return false;
	}

	const void* D3D12PipelineState::NativePtr() const
	{
		return nullptr;
	}

} // namespace Tridium