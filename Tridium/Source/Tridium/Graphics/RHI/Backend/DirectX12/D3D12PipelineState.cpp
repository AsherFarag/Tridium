#include "tripch.h"
#include "D3D12PipelineState.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ShaderBindingLayout.h"

namespace Tridium {

	namespace ToD3D12 {

		D3D12_SHADER_BYTECODE GetShaderBytecode( const RHIShaderModuleRef& a_Shader )
		{
			return a_Shader
				? D3D12_SHADER_BYTECODE( a_Shader->GetDescriptor()->Bytecode.data(), a_Shader->GetDescriptor()->Bytecode.size_bytes() )
				: D3D12_SHADER_BYTECODE();
		}

		D3D12_BLEND_DESC GetBlendDesc( const RHIBlendState& a_BlendState )
		{
			D3D12_BLEND_DESC desc = {};
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = a_BlendState.IsEnabled;
			desc.RenderTarget[0].SrcBlend = D3D12::To<D3D12_BLEND>::From( a_BlendState.SrcFactorColour );
			desc.RenderTarget[0].DestBlend = D3D12::To<D3D12_BLEND>::From( a_BlendState.DstFactorColour );
			desc.RenderTarget[0].BlendOp = D3D12::To<D3D12_BLEND_OP>::From( a_BlendState.BlendEquation );
			desc.RenderTarget[0].SrcBlendAlpha = D3D12::To<D3D12_BLEND>::From( a_BlendState.SrcFactorAlpha );
			desc.RenderTarget[0].DestBlendAlpha = D3D12::To<D3D12_BLEND>::From( a_BlendState.DstFactorAlpha );
			desc.RenderTarget[0].BlendOpAlpha = D3D12::To<D3D12_BLEND_OP>::From( a_BlendState.BlendEquation );
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

			desc.DepthFunc = D3D12::To<D3D12_COMPARISON_FUNC>::From( a_PSD.DepthState.Comparison );
			desc.StencilEnable = a_PSD.StencilState.IsEnabled;
			desc.StencilReadMask = a_PSD.StencilState.StencilReadMask;
			desc.StencilWriteMask = a_PSD.StencilState.StencilWriteMask;
			desc.FrontFace.StencilFailOp = D3D12::To<D3D12_STENCIL_OP>::From( a_PSD.StencilState.Fail );
			desc.FrontFace.StencilDepthFailOp = D3D12::To<D3D12_STENCIL_OP>::From( a_PSD.StencilState.DepthFail );
			desc.FrontFace.StencilPassOp = D3D12::To<D3D12_STENCIL_OP>::From( a_PSD.StencilState.Pass );
			desc.FrontFace.StencilFunc = D3D12::To<D3D12_COMPARISON_FUNC>::From( a_PSD.StencilState.Comparison );
			desc.BackFace = desc.FrontFace;
			return desc;
		}

	} // namespace ToD3D12

    bool D3D12GraphicsPipelineState::Commit( const void* a_Params )
    {
		const RHIGraphicsPipelineStateDescriptor* desc = ParamsToDescriptor<RHIGraphicsPipelineStateDescriptor>( a_Params );

		// Create the vertex input layout
		for ( size_t i = 0; i < desc->VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = desc->VertexLayout.Elements[i];
			VertexLayout[i] = {
				.SemanticName = element.Name.data(), 
				.SemanticIndex = 0,
				.Format = D3D12::To<DXGI_FORMAT>::From( element.Type ),
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
		psd.pRootSignature = desc->ShaderBindingLayout->As<D3D12ShaderBindingLayout>()->m_RootSignature.Get();

		// Set the input layout
		psd.InputLayout.NumElements = desc->VertexLayout.Elements.Size();
		psd.InputLayout.pInputElementDescs = VertexLayout;
		psd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Set the shaders
		psd.VS = ToD3D12::GetShaderBytecode( desc->VertexShader );
		psd.HS = ToD3D12::GetShaderBytecode( desc->HullShader );
		psd.DS = ToD3D12::GetShaderBytecode( desc->DomainShader );
		psd.GS = ToD3D12::GetShaderBytecode( desc->GeometryShader );
		psd.PS = ToD3D12::GetShaderBytecode( desc->PixelShader );

		// Set the blend state
		psd.BlendState = ToD3D12::GetBlendDesc( desc->BlendState );

		// Set the sample mask
		psd.SampleMask = UINT_MAX; // Enable all samples

		// Set the rasterizer state
		psd.RasterizerState = ToD3D12::GetRasterizerDesc( *desc );

		psd.DepthStencilState = ToD3D12::GetDepthStencilDesc( *desc );
		psd.PrimitiveTopologyType = D3D12::To<D3D12_PRIMITIVE_TOPOLOGY_TYPE>::From( desc->Topology );

		psd.NumRenderTargets = desc->ColourTargetFormats.Size();
		for ( size_t i = 0; i < desc->ColourTargetFormats.Size(); ++i )
		{
			psd.RTVFormats[i] = D3D12::To<DXGI_FORMAT>::From( desc->ColourTargetFormats[i] );
		}
		psd.DSVFormat = D3D12::To<DXGI_FORMAT>::From( desc->DepthStencilFormat );
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		psd.NodeMask = 0;
		psd.CachedPSO = { nullptr, 0 };

		// Create the pipeline state object
		ComPtr<D3D12::Device> device = RHI::GetD3D12RHI()->GetDevice();
		if ( FAILED( device->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( &PSO ) ) ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
			return false;
		}


	#if RHI_DEBUG_ENABLED
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			WString wName( desc->Name.begin(), desc->Name.end() );
			PSO->SetName( wName.c_str() );
			D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
		}
	#endif

		return true;

		return false;
    }

	bool D3D12GraphicsPipelineState::Release()
	{
		PSO.Release();
		return true;
	}

} // namespace Tridium