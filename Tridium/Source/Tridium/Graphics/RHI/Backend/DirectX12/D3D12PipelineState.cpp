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

	} // namespace ToD3D12

    bool D3D12PipelineState::Commit( const void* a_Params )
    {
		const RHIPipelineStateDescriptor* desc = ParamsToDescriptor<RHIPipelineStateDescriptor>( a_Params );
		if ( !desc || desc->GetPipelineType() == ERHIPipelineType::Invalid )
		{
			LOG( LogCategory::RHI, Error, "Invalid pipeline type" );
			return false;
		}

		// Create the vertex input layout
		for ( size_t i = 0; i < desc->VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = desc->VertexLayout.Elements[i];
			VertexLayout[i] = {
				.SemanticName = element.Name.data(), 
				.SemanticIndex = 0,
				.Format = ToD3D12::GetFormat( element.Type ),
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<UINT>( element.Offset ),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			};
		}

		if ( desc->GetPipelineType() == ERHIPipelineType::Graphics )
		{
			return CommitGraphics();
		}
		else if ( desc->GetPipelineType() == ERHIPipelineType::Compute )
		{
			return CommitCompute();
		}

		return false;
    }

	bool D3D12PipelineState::Release()
	{
		PSO.Release();
		return true;
	}

	bool D3D12PipelineState::IsValid() const
	{
		return PSO.Get() != nullptr;
	}

	const void* D3D12PipelineState::NativePtr() const
	{
		return PSO.Get();
	}

	bool D3D12PipelineState::CommitGraphics()
	{
		const RHIPipelineStateDescriptor* desc = GetDescriptor();

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
		ComPtr<D3D12::Device> device = RHI::GetD3D12RHI()->GetDevice();
		if ( FAILED( device->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( &PSO ) ) ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
			return false;
		}


		#if RHI_DEBUG_ENABLED
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			WString wName = ToD3D12::ToWString( desc->Name.data() );
			PSO->SetName( wName.c_str() );
			D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
		}
		#endif

		return true;
	}

	bool D3D12PipelineState::CommitCompute()
	{
		LOG( LogCategory::RHI, Error, "Compute pipeline state not implemented" );
		return false;
	}

} // namespace Tridium