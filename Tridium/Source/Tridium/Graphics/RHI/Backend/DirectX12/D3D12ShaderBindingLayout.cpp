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

	bool D3D12ShaderBindingLayout::Commit( const RHIShaderBindingLayoutDescriptor& a_Desc )
    {
		m_Desc = a_Desc;
		const auto& device = GetD3D12RHI()->GetD3D12Device();

		Array<D3D12::RootParameter> rootParams;

		// Reserve space for the root parameters
		rootParams.Reserve( a_Desc.Bindings.Size() );

        Array<Array<D3D12::DescriptorRange>> descriptorRangesList; // Stores ranges for each root param
        descriptorRangesList.Reserve( a_Desc.Bindings.Size() ); // Reserve memory

        for ( const auto& binding : a_Desc.Bindings )
        {
            switch ( binding.BindingType )
            {
            case ERHIShaderBindingType::ConstantBuffer:
            {
                if ( binding.IsInlined() )
                {
					rootParams.EmplaceBack().AsConstants( binding.WordSize, D3D12::Translate( binding.Visibility ), binding.BindSlot );
                }
                else
                {
                    descriptorRangesList.EmplaceBack(); // New range for this param
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 ); // Single descriptor range
					range[0] = D3D12::DescriptorRange( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, binding.WordSize, binding.BindSlot );

					rootParams.EmplaceBack().AsDescriptorTable( D3D12::Translate( binding.Visibility ), range );
                }
                break;
            }
            case ERHIShaderBindingType::Texture:
            case ERHIShaderBindingType::MutableBuffer:
            {
                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
				range[0] = D3D12::DescriptorRange( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, binding.BindSlot );

				rootParams.EmplaceBack().AsCBV( D3D12::Translate( binding.Visibility ), binding.BindSlot );
                break;
            }
            case ERHIShaderBindingType::Sampler:
            {
                // Dynamic sampler - Descriptor Heap Binding
                descriptorRangesList.EmplaceBack();
                auto& range = descriptorRangesList.Back();
                range.Resize( 1 );
				range[0] = D3D12::DescriptorRange( D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, binding.BindSlot );

				rootParams.EmplaceBack().AsDescriptorTable( D3D12::Translate( binding.Visibility ), range );
                break;
            }
            case ERHIShaderBindingType::CombinedSampler:
            {
                {
					// Dynamic sampler - Descriptor Heap Binding
                    descriptorRangesList.EmplaceBack();
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 );
					range[0] = D3D12::DescriptorRange( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, binding.BindSlot );

					rootParams.EmplaceBack().AsDescriptorTable( D3D12::Translate( binding.Visibility ), range );
                }
                {
                    // Dynamic sampler - Descriptor Heap Binding
                    descriptorRangesList.EmplaceBack();
                    auto& range = descriptorRangesList.Back();
                    range.Resize( 1 );
					range[0] = D3D12::DescriptorRange( D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, binding.BindSlot );

					rootParams.EmplaceBack().AsDescriptorTable( D3D12::Translate( binding.Visibility ), range );
                }
				break;
            }
            }
        }

        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// Create the root signature
        D3D12::RootSignatureDesc rootSignatureDesc(
            { rootParams.Data(), rootParams.Size() },
            { s_StaticSamplerDescs, s_NumStaticSamplers },
            flags );

        m_RootSignature = rootSignatureDesc.Create();

        #if RHI_DEBUG_ENABLED
        if ( RHIQuery::IsDebug() && !a_Desc.Name.empty() )
        {
			WString wName( a_Desc.Name.begin(), a_Desc.Name.end() );
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
