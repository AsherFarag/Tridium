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
		Array<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
		Array<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

		// Reserve space for the root parameters
		rootParams.Reserve( desc->Bindings.Size() );

		// Create the root parameters
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
						// Inlined constants -> Root Constants
						rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
						rootParam.Constants.Num32BitValues = binding.WordSize;
						rootParam.Constants.ShaderRegister = binding.Register;
                        rootParams.PushBack( rootParam );
					}
					else
					{
                        // Referenced Constants -> CBV Descriptor Table
                        D3D12_DESCRIPTOR_RANGE range = {};
                        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                        range.NumDescriptors = binding.WordSize;
                        range.BaseShaderRegister = binding.Register;
                        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                        descriptorRanges.PushBack( range );

                        rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                        rootParam.DescriptorTable.NumDescriptorRanges = 1;
                        rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.Back();
                        rootParams.PushBack( rootParam );
					}
					break;
				}
                case ERHIShaderBindingType::Mutable:
                {
					// Mutable Buffers -> SRV/UAV Descriptor Table
                    D3D12_DESCRIPTOR_RANGE range = {};
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // Could also be UAV
                    range.NumDescriptors = binding.WordSize;
                    range.BaseShaderRegister = binding.Register;
                    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                    descriptorRanges.PushBack( range );

                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                    rootParam.DescriptorTable.NumDescriptorRanges = 1;
                    rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.Back();
                    rootParams.PushBack( rootParam );
                    break;
                }
                case ERHIShaderBindingType::Texture:
                {
                    // Textures -> SRV Descriptor Table
                    D3D12_DESCRIPTOR_RANGE range = {};
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    range.NumDescriptors = binding.WordSize;
                    range.BaseShaderRegister = binding.Register;
                    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
                    descriptorRanges.PushBack( range );

                    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                    rootParam.DescriptorTable.NumDescriptorRanges = 1;
                    rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.Back();
                    rootParams.PushBack( rootParam );
                    break;
                }
				case ERHIShaderBindingType::Sampler:
				{
					TODO( "Implement Sampler" );
					// Static Sampler
                    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
                    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
                    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                    samplerDesc.ShaderRegister = binding.Register;
                    staticSamplers.PushBack( samplerDesc );
					break;
				}
				default:
					break;
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
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature: %s", (char*)errorBlob->GetBufferPointer() );
            }
            else
            {
				LOG( LogCategory::DirectX, Error, "Failed to serialize root signature" );
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
