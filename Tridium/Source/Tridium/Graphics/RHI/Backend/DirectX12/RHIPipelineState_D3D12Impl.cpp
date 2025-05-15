#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include <algorithm>

namespace Tridium::D3D12 {

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
		desc.RenderTarget[0].BlendEnable = a_BlendState.IsEnabled;
		desc.RenderTarget[0].SrcBlend = D3D12::Translate( a_BlendState.SrcFactorColor );
		desc.RenderTarget[0].DestBlend = D3D12::Translate( a_BlendState.DstFactorColor );
		desc.RenderTarget[0].BlendOp = D3D12::Translate( a_BlendState.BlendEquation );
		desc.RenderTarget[0].SrcBlendAlpha = D3D12::Translate( a_BlendState.SrcFactorAlpha );
		desc.RenderTarget[0].DestBlendAlpha = D3D12::Translate( a_BlendState.DstFactorAlpha );
		desc.RenderTarget[0].BlendOpAlpha = D3D12::Translate( a_BlendState.BlendEquation );
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

	RHIGraphicsPipelineState_D3D12Impl::RHIGraphicsPipelineState_D3D12Impl( const DescriptorType& a_Desc )
		: RHIGraphicsPipelineState( a_Desc )
    {
		// Create the vertex input layout
		// We create tempory strings here of the vertex element names,
		// as RHIVertexAttribute::Name is a StringView and can be not null terminated.
		// And SemanticName requires a null terminated string.
		FixedArray<String, RHIConstants::MaxVertexAttributes> vertexElementNames;
		VertexLayoutSize = a_Desc.VertexLayout.Elements.Size();
		for ( size_t i = 0; i < VertexLayoutSize; ++i )
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
		psd.pRootSignature = a_Desc.BindingLayout->As<RHIBindingLayout_D3D12Impl>()->m_RootSignature.Get();

		// Set the input layout
		psd.InputLayout.NumElements = VertexLayoutSize;
		psd.InputLayout.pInputElementDescs = VertexLayout;
		psd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Set the shaders
		psd.VS = GetShaderBytecode( a_Desc.VertexShader );
		psd.HS = GetShaderBytecode( a_Desc.HullShader );
		psd.DS = GetShaderBytecode( a_Desc.DomainShader );
		psd.GS = GetShaderBytecode( a_Desc.GeometryShader );
		psd.PS = GetShaderBytecode( a_Desc.PixelShader );

		// Set the blend state
		psd.BlendState = GetBlendDesc( a_Desc.BlendState );

		// Set the sample mask
		psd.SampleMask = ~0u; // Enable all samples

		// Set the rasterizer state
		psd.RasterizerState = GetRasterizerDesc( a_Desc );

		// Set the depth stencil state
		psd.DepthStencilState = GetDepthStencilDesc( a_Desc );
		psd.DSVFormat = D3D12::Translate( a_Desc.FramebufferInfo.DepthStencilFormat );

		switch ( a_Desc.Topology )
		{
			case ERHITopology::Point:         psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT; break;
			case ERHITopology::Line:          psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case ERHITopology::LineStrip:     psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
			case ERHITopology::Triangle:      psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
			case ERHITopology::TriangleStrip: psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
			default:         ASSERT( false ); psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED; break;
		}

		psd.NumRenderTargets = a_Desc.FramebufferInfo.ColorFormats.Size();
		for ( size_t i = 0; i < a_Desc.FramebufferInfo.ColorFormats.Size(); ++i )
			psd.RTVFormats[i] = D3D12::Translate( a_Desc.FramebufferInfo.ColorFormats[i] );
		psd.DSVFormat = D3D12::Translate( a_Desc.FramebufferInfo.DepthStencilFormat );
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		psd.NodeMask = 0;
		psd.CachedPSO = { nullptr, 0 };

		// Create the pipeline state object
		if ( FAILED( GetD3D12RHI()->GetD3D12Device()->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( &PSO ) ) ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
			Release();
			return;
		}

		D3D12_SET_DEBUG_NAME( PSO.Get(), a_Desc.Name );
    }

	bool RHIGraphicsPipelineState_D3D12Impl::Release()
	{
		PSO.Release();
		return true;
	}

	RootSignature RootSignature::Build( Span<RHIBindingLayoutRef> a_Layouts, bool a_AllowInputLayout, bool a_IsLocal, Span<const D3D12_ROOT_PARAMETER1> a_CustomParams )
	{
		TODO( "Support bindless" );

		HRESULT hr = S_OK;
		RootSignature rootSig;

		// Visit each binding layout, get the num of bindings and add them to NumParams
		size_t numParams = a_CustomParams.size();
		for ( const auto& layout : a_Layouts ) 
			numParams += layout->Descriptor().Bindings.Size();

		Array<D3D12_ROOT_PARAMETER1> rootParams;
		rootParams.Reserve( numParams );

		// Add the custom params
		for ( const auto& param : a_CustomParams )
			rootParams.EmplaceBack( param );

		// Add the binding layouts
		for ( size_t i = 0; i < a_Layouts.size(); ++i )
		{
			const auto& layout = a_Layouts[i]->As<RHIBindingLayout_D3D12Impl>();
			RootParameterIndex rootParamOffset = RootParameterIndex( rootParams.Size() );

			rootSig.Layouts.EmplaceBack( layout, rootParamOffset );
			rootParams.Insert( rootParams.End(), layout->RootParams.Begin(), layout->RootParams.End() );

			if ( layout->InlinedConstantsSize > 0 )
			{
				rootSig.InlinedConstantsSize = layout->InlinedConstantsSize;
				rootSig.RootParamInlinedConstants = rootParamOffset + layout->RootParamInlinedConstants;
			}
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if ( a_AllowInputLayout )
			rootSigDesc.Desc_1_1.Flags |=  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		if ( a_IsLocal )
			rootSigDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

		if ( !rootParams.IsEmpty() )
		{
			rootSigDesc.Desc_1_1.NumParameters = static_cast<UINT>(rootParams.Size());
			rootSigDesc.Desc_1_1.pParameters = rootParams.Data();
		}

		ComPtr<ID3DBlob> rootSigBlob;
		ComPtr<ID3DBlob> errorBlob;
		hr = D3D12SerializeVersionedRootSignature( &rootSigDesc, &rootSigBlob, &errorBlob );

		if ( FAILED( hr ) )
		{
			if ( errorBlob )
			{
				LOG( LogCategory::RHI, Error, "Failed to serialize root signature: {}", StringView( Cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize() ) );
			}
			else
			{
				LOG( LogCategory::RHI, Error, "Failed to serialize root signature" );
			}
			return {};
		}

		hr = GetD3D12RHI()->GetD3D12Device()->CreateRootSignature( 
			0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS( &rootSig.D3D12Signature ) 
		);

		if ( FAILED( hr ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create root signature" );
			return {};
		}

		return rootSig;
	}

} // namespace Tridium