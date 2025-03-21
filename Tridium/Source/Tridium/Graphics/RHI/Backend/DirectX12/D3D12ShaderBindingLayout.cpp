#include "tripch.h"
#include "D3D12ShaderBindingLayout.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

	static constexpr D3D12_STATIC_SAMPLER_DESC MakeStaticSampler( D3D12_FILTER a_Filter, D3D12_TEXTURE_ADDRESS_MODE a_WrapMode, uint32_t a_Register, uint32_t a_Space, uint32_t a_MaxAnisotropy = 1 )
    {
        D3D12_STATIC_SAMPLER_DESC result = {};

        result.Filter = a_Filter;
        result.AddressU = a_WrapMode;
        result.AddressV = a_WrapMode;
        result.AddressW = a_WrapMode;
        result.MipLODBias = 0.0f;
        result.MaxAnisotropy = a_MaxAnisotropy;
        result.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        result.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        result.MinLOD = 0.0f;
        result.MaxLOD = D3D12_FLOAT32_MAX;
        result.ShaderRegister = a_Register;
        result.RegisterSpace = a_Space;
        result.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        return result;
    }

	constexpr uint32_t s_NumStaticSamplers = 6;
    static constexpr D3D12_STATIC_SAMPLER_DESC s_StaticSamplerDescs[] = {
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_MIP_POINT,        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  0, 1000 ),
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_MIP_POINT,        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 1, 1000 ),
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP,  2, 1000 ),
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 3, 1000 ),
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_MIP_LINEAR,       D3D12_TEXTURE_ADDRESS_MODE_WRAP,  4, 1000 ),
        MakeStaticSampler( D3D12_FILTER_MIN_MAG_MIP_LINEAR,       D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 5, 1000 ),
    };

    bool D3D12ShaderBindingLayout::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIShaderBindingLayoutDescriptor>( a_Params );
        if ( !desc )
            return false;

		const auto& device = RHI::GetD3D12RHI()->GetDevice();

		Array<D3D12_ROOT_PARAMETER> rootParams;

		// Reserve space for the root parameters
		rootParams.Reserve( desc->Bindings.Size() );

        Array<Array<D3D12_DESCRIPTOR_RANGE>> descriptorRangesList; // Stores ranges for each root param
        descriptorRangesList.Reserve( desc->Bindings.Size() ); // Reserve memory

        for ( const auto& binding : desc->Bindings )
        {
            switch ( binding.BindingType )
            {
            case ERHIShaderBindingType::Constant:
            {
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );

                if ( binding.IsInlined() )
                {
                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                    rootParam.Constants.Num32BitValues = binding.WordSize;
                    rootParam.Constants.ShaderRegister = binding.BindSlot;
                    rootParams.PushBack( rootParam );
                }
                else
                {
                    descriptorRangesList.EmplaceBack(); // New range for this param
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 ); // Single descriptor range
                    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					range[0].NumDescriptors = binding.WordSize;
                    range[0].BaseShaderRegister = binding.BindSlot;
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
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );

                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
                range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                range[0].NumDescriptors = 1;
                range[0].BaseShaderRegister = binding.BindSlot;
                range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                rootParams.PushBack( rootParam );
                break;
            }
            case ERHIShaderBindingType::Sampler:
            {
                D3D12_ROOT_PARAMETER rootParam = {};
                rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );

                // Dynamic sampler - Descriptor Heap Binding
                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
                range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                range[0].NumDescriptors = 1;
                range[0].BaseShaderRegister = binding.BindSlot;
                range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                rootParams.PushBack( rootParam );
                break;
            }
            case ERHIShaderBindingType::CombinedSampler:
            {
                {
                    D3D12_ROOT_PARAMETER rootParam = {};
                    rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );

                    descriptorRangesList.EmplaceBack();
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 );
                    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    range[0].NumDescriptors = 1;
                    range[0].BaseShaderRegister = binding.BindSlot;
                    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                    rootParam.DescriptorTable.NumDescriptorRanges = 1;
                    rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                    rootParams.PushBack( rootParam );
                }
                {
                    D3D12_ROOT_PARAMETER rootParam = {};
                    rootParam.ShaderVisibility = ToD3D12::GetShaderVisibility( binding.Visibility );
                    // Dynamic sampler - Descriptor Heap Binding
                    descriptorRangesList.EmplaceBack();
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 );
                    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    range[0].NumDescriptors = 1;
                    range[0].BaseShaderRegister = binding.BindSlot;
                    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                    rootParam.DescriptorTable.NumDescriptorRanges = 1;
                    rootParam.DescriptorTable.pDescriptorRanges = range.Data();
                    rootParams.PushBack( rootParam );
                }
				break;
            }
            }
        }

		// Create the root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.NumParameters = rootParams.Size();
		rootSignatureDesc.pParameters = rootParams.Data();
		rootSignatureDesc.NumStaticSamplers = s_NumStaticSamplers;
		rootSignatureDesc.pStaticSamplers = s_StaticSamplerDescs;
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
