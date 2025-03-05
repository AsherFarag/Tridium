#include "tripch.h"
#include "D3D12ShaderBindingLayout.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

    bool D3D12ShaderBindingLayout::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIShaderBindingLayoutDescriptor>( a_Params );
        if ( !desc )
            return false;

		const auto& device = RHI::GetD3D12RHI()->GetDevice();

		Array<D3D12_ROOT_PARAMETER> rootParams;
		Array<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

		// Reserve space for the root parameters
		rootParams.Reserve( desc->Bindings.Size() );

        Array<Array<D3D12_DESCRIPTOR_RANGE>> descriptorRangesList; // Stores ranges for each root param
        descriptorRangesList.Reserve( desc->Bindings.Size() ); // Reserve memory

        for ( const auto& binding : desc->Bindings )
        {
            D3D12_ROOT_PARAMETER rootParam = {};
            rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );

            switch ( binding.BindingType )
            {
            case ERHIShaderBindingType::Constant:
            {
                if ( binding.IsInlined )
                {
                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                    rootParam.Constants.Num32BitValues = binding.WordSize;
                    rootParam.Constants.ShaderRegister = binding.Register;
                    rootParams.PushBack( rootParam );
                }
                else
                {
                    descriptorRangesList.EmplaceBack(); // New range for this param
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 ); // Single descriptor range
                    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					range[0].NumDescriptors = binding.WordSize;
                    range[0].BaseShaderRegister = binding.Register;
                    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                    rootParam.DescriptorTable.NumDescriptorRanges = 1;
                    rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                    rootParams.PushBack( rootParam );
                }
                break;
            }
            case ERHIShaderBindingType::Texture:
            case ERHIShaderBindingType::Mutable:
            {
                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
                range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                range[0].NumDescriptors = 1;
                range[0].BaseShaderRegister = binding.Register;
                range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                rootParams.PushBack( rootParam );
                break;
            }
            case ERHIShaderBindingType::Sampler:
            {
                // Dynamic sampler - Descriptor Heap Binding
                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
                range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                range[0].NumDescriptors = 1;
                range[0].BaseShaderRegister = binding.Register;
                range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                rootParams.PushBack( rootParam );
                break;
            }
            case ERHIShaderBindingType::StaticSampler:
            {
                D3D12_STATIC_SAMPLER_DESC staticSampler = {};
                staticSampler.Filter = ToD3D12::GetFilter( binding.SamplerDesc.Filter );
                staticSampler.AddressU = ToD3D12::GetAddressMode( binding.SamplerDesc.AddressU );
                staticSampler.AddressV = ToD3D12::GetAddressMode( binding.SamplerDesc.AddressV );
                staticSampler.AddressW = ToD3D12::GetAddressMode( binding.SamplerDesc.AddressW );
                staticSampler.MipLODBias = binding.SamplerDesc.MipLODBias;
                staticSampler.MaxAnisotropy = binding.SamplerDesc.MaxAnisotropy;
                staticSampler.ComparisonFunc = ToD3D12::GetComparisonFunc( binding.SamplerDesc.ComparisonFunc );
                staticSampler.MinLOD = binding.SamplerDesc.MinLOD;
                staticSampler.MaxLOD = binding.SamplerDesc.MaxLOD;
                staticSampler.ShaderRegister = binding.Register;
                staticSampler.ShaderVisibility = rootParam.ShaderVisibility;

                if ( binding.SamplerDesc.BorderColor == Color::White() )
				{
					staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				}
				else if ( binding.SamplerDesc.BorderColor.a < 1.0f )
				{
					staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
				}
				else
				{
					staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
				}

                staticSamplers.PushBack( staticSampler );
            }
            }
        }

		// Create the root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.NumParameters = rootParams.Size();
		rootSignatureDesc.pParameters = rootParams.Data();
		rootSignatureDesc.NumStaticSamplers = staticSamplers.Size();
		rootSignatureDesc.pStaticSamplers = staticSamplers.Data();
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // Serialize and Create Root Signature
        ComPtr<ID3DBlob> serializedRootSig;
        ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &serializedRootSig,
            &errorBlob
        );

        if ( FAILED( hr ) )
        {
            if ( errorBlob )
            {
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature in Shader Binding Layout '{0}' - Error: {1}", desc->Name.data(), (char*)errorBlob->GetBufferPointer() );
            }
            else
            {
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature in Shader Binding Layout '{0}'", desc->Name.data() );
            }
			return false;
        }

		hr = device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS( &m_RootSignature )
		);

        if ( FAILED( hr ) )
        {
            LOG( LogCategory::DirectX, Error, "Failed to create root signature" );
            return false;
        }


        #if RHI_DEBUG_ENABLED
        if ( RHIQuery::IsDebug() && !desc->Name.empty() )
        {
            WString wName = ToD3D12::ToWString( desc->Name.data() );
            m_RootSignature->SetName( wName.c_str() );
            D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
        }
        #endif

        return true;
    }

    bool D3D12ShaderBindingLayout::Release()
    {
		m_RootSignature.Release();
        return true;
    }

    bool D3D12ShaderBindingLayout::IsValid() const
    {
		return m_RootSignature != nullptr;
    }

    const void* D3D12ShaderBindingLayout::NativePtr() const
    {
		return m_RootSignature.Get();
    }

}
