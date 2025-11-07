#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include <algorithm>

namespace Tridium::D3D12 {

	D3D12_SHADER_BYTECODE GetShaderBytecode( const RHIShaderModuleRef& a_Shader )
	{
		return a_Shader
			? D3D12_SHADER_BYTECODE( a_Shader->Desc().Bytecode.data(), a_Shader->Desc().Bytecode.size_bytes() )
			: D3D12_SHADER_BYTECODE();
	}

	D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendDesc( const RHIBlendState::RenderTarget& a_RenderTarget )
	{
		D3D12_RENDER_TARGET_BLEND_DESC desc{};
		desc.BlendEnable = a_RenderTarget.BlendEnabled;
		desc.LogicOpEnable = a_RenderTarget.LogicOpEnabled;
		desc.SrcBlend = Translate( a_RenderTarget.SrcColor );
		desc.DestBlend = Translate( a_RenderTarget.DstColor );
		desc.BlendOp = Translate( a_RenderTarget.BlendOpColor );
		desc.SrcBlendAlpha = Translate( a_RenderTarget.SrcAlpha );
		desc.DestBlendAlpha = Translate( a_RenderTarget.DstAlpha );
		desc.BlendOpAlpha = Translate( a_RenderTarget.BlendOpAlpha );
		desc.LogicOp = Translate( a_RenderTarget.LogicOp );

		desc.RenderTargetWriteMask = 0;
		desc.RenderTargetWriteMask |= EnumFlags( a_RenderTarget.ColorWriteMask ).HasFlag( ERHIColorMask::Red ) ? D3D12_COLOR_WRITE_ENABLE_RED : 0;
		desc.RenderTargetWriteMask |= EnumFlags( a_RenderTarget.ColorWriteMask ).HasFlag( ERHIColorMask::Green ) ? D3D12_COLOR_WRITE_ENABLE_GREEN : 0;
		desc.RenderTargetWriteMask |= EnumFlags( a_RenderTarget.ColorWriteMask ).HasFlag( ERHIColorMask::Blue ) ? D3D12_COLOR_WRITE_ENABLE_BLUE : 0;
		desc.RenderTargetWriteMask |= EnumFlags( a_RenderTarget.ColorWriteMask ).HasFlag( ERHIColorMask::Alpha ) ? D3D12_COLOR_WRITE_ENABLE_ALPHA : 0;

		return desc;
	}

	D3D12_BLEND_DESC GetBlendDesc( const RHIBlendState& a_BlendState )
	{
		D3D12_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = a_BlendState.AlphaToCoverageEnabled;
		desc.IndependentBlendEnable = a_BlendState.IndependentBlendEnabled;
		if ( desc.IndependentBlendEnable )
		{
			for ( size_t i = 0; i < a_BlendState.RenderTargets.MaxSize(); ++i )
				desc.RenderTarget[i] = GetRenderTargetBlendDesc( a_BlendState.RenderTargets[i] );
		}
		else
		{
			desc.RenderTarget[0] = GetRenderTargetBlendDesc( a_BlendState.RenderTargets[0] );
		}

		return desc;
	}

	D3D12_RASTERIZER_DESC GetRasterizerDesc( const RHIGraphicsPipelineStateDesc& a_PSD )
	{
		D3D12_RASTERIZER_DESC desc{};

		// Set the fill mode
		desc.FillMode = a_PSD.RasterizerState.FillMode == ERHIFillMode::Wireframe
			? D3D12_FILL_MODE_WIREFRAME
			: D3D12_FILL_MODE_SOLID;

		// Set the cull mode
		switch ( a_PSD.RasterizerState.CullMode )
		{
			using enum ERHICullMode;
		case Front: desc.CullMode = D3D12_CULL_MODE_FRONT; break;
		case Back:  desc.CullMode = D3D12_CULL_MODE_BACK;  break;
		case None:  desc.CullMode = D3D12_CULL_MODE_NONE;  break;
		}

		desc.FrontCounterClockwise = a_PSD.RasterizerState.Clockwise;

		// Set depth values
		desc.DepthBias = a_PSD.RasterizerState.DepthBias;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = a_PSD.RasterizerState.SlopeScaledDepthBias;
		desc.DepthClipEnable = a_PSD.RasterizerState.DepthClipEnabled;

		// Set the multisampling values
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = a_PSD.RasterizerState.AnitaliasedLinesEnabled;
		desc.ForcedSampleCount = 0;

		return desc;
	}

	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc( const RHIGraphicsPipelineStateDesc& a_PSD )
	{
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.DepthEnable      = a_PSD.DepthState.DepthTestEnabled;
		desc.DepthWriteMask   = a_PSD.DepthState.DepthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc        = Translate( a_PSD.DepthState.Comparison );
		desc.StencilEnable    = a_PSD.StencilState.Enabled;
		desc.StencilReadMask  = a_PSD.StencilState.ReadMask;
		desc.StencilWriteMask = a_PSD.StencilState.WriteMask;
		desc.FrontFace.StencilFailOp      = Translate( a_PSD.StencilState.FrontFace.StencilFailOp );
		desc.FrontFace.StencilDepthFailOp = Translate( a_PSD.StencilState.FrontFace.DepthFailOp );
		desc.FrontFace.StencilPassOp      = Translate( a_PSD.StencilState.FrontFace.PassOp );
		desc.FrontFace.StencilFunc        = Translate( a_PSD.StencilState.FrontFace.Comparison );
		desc.BackFace.StencilFailOp       = Translate( a_PSD.StencilState.BackFace.StencilFailOp );
		desc.BackFace.StencilDepthFailOp  = Translate( a_PSD.StencilState.BackFace.DepthFailOp );
		desc.BackFace.StencilPassOp       = Translate( a_PSD.StencilState.BackFace.PassOp );
		desc.BackFace.StencilFunc         = Translate( a_PSD.StencilState.BackFace.Comparison );
		return desc;
	}

	RHIGraphicsPipelineState_D3D12Impl::RHIGraphicsPipelineState_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, SharedPtr<RootSignature> a_RootSig )
		: IRHIGraphicsPipelineState( a_Device, a_Desc )
    {
		ASSERT( a_RootSig, "Root signature must not be null" );

		RootSig = std::move( a_RootSig );

		// Create the ID3D12PipelineState object

		// Create the vertex input layout
		// We create tempory strings here of the vertex element names,
		// as RHIVertexAttribute::Name is a StringView and can be not null terminated.
		// And SemanticName requires a null terminated string.
		FixedArray<String, RHIConstants::MaxVertexAttributes> vertexElementNames;
		for ( size_t i = 0; i < a_Desc.VertexLayout.Elements.Size(); ++i )
		{
			vertexElementNames[i] = a_Desc.VertexLayout.Elements[i].Name;
			const RHIVertexAttribute& element = a_Desc.VertexLayout.Elements[i];
			VertexLayout.EmplaceBack( D3D12_INPUT_ELEMENT_DESC{
				.SemanticName = vertexElementNames[i].c_str(),
				.SemanticIndex = 0,
				.Format = GetDXGIFormatMap( element.Type ).SRVFormat,
				.InputSlot = 0,
				.AlignedByteOffset = Cast<UINT>( element.Offset ),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0 
				} );
		}

		// Create the pipeline state object
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
		psd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		// Set the root signature
		psd.pRootSignature = RootSig->D3D12Signature.Get();

		// Set the input layout
		psd.InputLayout.NumElements = VertexLayout.Size();
		psd.InputLayout.pInputElementDescs = VertexLayout.Data();
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
		psd.DSVFormat = GetDXGIFormatMap( a_Desc.FramebufferInfo.DepthStencilFormat ).RTVFormat;

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
			psd.RTVFormats[i] = GetDXGIFormatMap( a_Desc.FramebufferInfo.ColorFormats[i] ).RTVFormat;
		psd.DSVFormat = GetDXGIFormatMap( a_Desc.FramebufferInfo.DepthStencilFormat ).RTVFormat;
		psd.SampleDesc.Count = 1;
		psd.SampleDesc.Quality = 0;
		psd.NodeMask = 0;
		psd.CachedPSO = { nullptr, 0 };

		// Create the pipeline state object
		if ( FAILED( GetD3D12RHI()->GetD3D12Device()->CreateGraphicsPipelineState( &psd, IID_PPV_ARGS( PSO.GetAddressOf() ) ) ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create graphics pipeline state" );
			Release();
			return;
		}

		D3D12_SET_DEBUG_NAME( PSO.Get(), a_Desc.Name, L"Unnamed Pipeline State" );
    }

	bool RHIGraphicsPipelineState_D3D12Impl::Release()
	{
		PSO.Reset();
		return true;
	}

	RootSignature RootSignature::Build( Span<const RHIBindingLayoutRef> a_Layouts, bool a_AllowInputLayout, bool a_IsLocal, Span<const D3D12_ROOT_PARAMETER1> a_CustomParams )
	{
		TODO( "Support bindless" );

		HRESULT hr = S_OK;
		RootSignature rootSig;

		// Visit each binding layout, get the num of bindings and add them to NumParams
		size_t numParams = a_CustomParams.size();
		for ( const auto& layout : a_Layouts ) 
			numParams += layout->Desc().Bindings.Size();

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

			rootSig.Layouts.EmplaceBack( a_Layouts[i], rootParamOffset );
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

		if ( !rootParams.Empty() )
		{
			rootSigDesc.Desc_1_1.NumParameters = Cast<UINT>(rootParams.Size());
			rootSigDesc.Desc_1_1.pParameters = rootParams.Data();
		}
		else
		{
			rootSigDesc.Desc_1_1.NumParameters = 0;
			rootSigDesc.Desc_1_1.pParameters = nullptr;
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
			0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS( rootSig.D3D12Signature.GetAddressOf() )
		);

		if ( FAILED( hr ) )
		{
			LOG( LogCategory::RHI, Error, "Failed to create root signature" );
			return {};
		}

		return rootSig;
	}

} // namespace Tridium