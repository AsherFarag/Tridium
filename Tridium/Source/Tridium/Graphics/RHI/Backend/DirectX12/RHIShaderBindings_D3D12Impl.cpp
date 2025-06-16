#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include "D3D12RootSig.h"

namespace Tridium::D3D12 {

    static constexpr bool AreBindingsCompatible( ERHIBindingType a_First, ERHIBindingType a_Second )
    {
        using enum ERHIBindingType;

        if ( a_First == a_Second )
            return true;

        // SRV
        if (   ( a_First == StructuredBuffer && a_Second == Texture )
            || ( a_Second == StructuredBuffer && a_First == Texture ) )
            return true;

        // UAV
        if (   ( a_First == StorageBuffer && a_Second == StorageTexture )
            || ( a_Second == StorageBuffer && a_First == StorageTexture) )
            return true;

        return false;
    }

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

    // Based off NVRHI d3d12::BindingLayout()
	RHIBindingLayout_D3D12Impl::RHIBindingLayout_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
        : IRHIBindingLayout( a_Device, a_Desc )
    {
        uint32_t currentSlot = ~0u;
        ERHIBindingType currentType = ERHIBindingType::Unknown;
		D3D12_ROOT_CONSTANTS rootConstants{};

        for ( const auto& binding : a_Desc.Bindings )
        {
            if ( binding.Type() == ERHIBindingType::InlinedConstants )
            {
                InlinedConstantsSize = binding.Size;
                rootConstants.Num32BitValues = NumDWORDsFromBytes( binding.Size );
                rootConstants.ShaderRegister = binding.Slot;
                rootConstants.RegisterSpace = a_Desc.RegisterSpace;
            }
            // Do we need to start a new range?
            else if ( !AreBindingsCompatible( binding.Type(), currentType )
                || binding.Slot != currentSlot + 1 )
            {
                if ( binding.Type() == ERHIBindingType::Sampler )
                {
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesSamplers.EmplaceBack();
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    range.NumDescriptors = 1;
                    range.BaseShaderRegister = binding.Slot;
                    range.RegisterSpace = a_Desc.RegisterSpace;
                    range.OffsetInDescriptorsFromTableStart = DescriptorTableSizeSamplers++;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                }
                else
                {
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.EmplaceBack();
                    switch ( binding.Type() )
                    {
                        // SRV
                        case ERHIBindingType::StructuredBuffer:
                        case ERHIBindingType::Texture:
                        {
                            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                            break;
                        }
                        // UAV
                        case ERHIBindingType::StorageBuffer:
                        case ERHIBindingType::StorageTexture:
                        {
                            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                            break;
                        }
                        // CBV
                        case ERHIBindingType::ConstantBuffer:
                        {
                            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                            break;
                        }
                        default:
                        {
                            RHI_DEV_CHECK( false, "Invalid binding type '{}'", ToString( binding.Type() ) );
                            continue;
                        }
                    }

                    range.NumDescriptors = 1;
                    range.BaseShaderRegister = binding.Slot;
                    range.RegisterSpace = a_Desc.RegisterSpace;
                    range.OffsetInDescriptorsFromTableStart = DescriptorTableSizeRenderResources++;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

                    RenderResourceBindingLayouts.PushBack( binding );
                }

                currentSlot = binding.Slot;
                currentType = binding.Type();
            }
            // Extend the current range
            else
            {
                if ( binding.Type() == ERHIBindingType::Sampler )
                {
                    ASSERT( !DescriptorRangesSamplers.Empty() );
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesSamplers.EmplaceBack();
                    range.NumDescriptors += 1;
                    DescriptorTableSizeSamplers++;
                }
                else
                {
                    ASSERT( !DescriptorRangesRenderResources.Empty() );
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.EmplaceBack();
                    range.NumDescriptors += 1;
                    DescriptorTableSizeRenderResources++;
                    RenderResourceBindingLayouts.PushBack( binding );
                }

                currentSlot = binding.Slot;
            }
        }

        RootParams.Clear();

        if ( rootConstants.Num32BitValues )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            param.ShaderVisibility = Translate( a_Desc.Visibility );
            param.Constants = rootConstants;
            RootParamInlinedConstants = RootParameterIndex( RootParams.Size() - 1 );
        }

        if ( DescriptorTableSizeSamplers )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = Translate( a_Desc.Visibility );
            param.DescriptorTable.NumDescriptorRanges = UINT( DescriptorRangesSamplers.Size() );
            param.DescriptorTable.pDescriptorRanges = &DescriptorRangesSamplers[0];
            RootParamSamplers = RootParameterIndex( RootParams.Size() - 1 );
        }

        if ( DescriptorTableSizeRenderResources > 0 )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = Translate( a_Desc.Visibility );
            param.DescriptorTable.NumDescriptorRanges = UINT( DescriptorRangesRenderResources.Size() );
            param.DescriptorTable.pDescriptorRanges = &DescriptorRangesRenderResources[0];
            RootParamRenderResources = RootParameterIndex( RootParams.Size() - 1 );
        }
    }

    bool RHIBindingLayout_D3D12Impl::Release()
    {
        InlinedConstantsSize = 0;
        RootParamInlinedConstants = c_InvalidRootParameterIndex;
        RootParamRenderResources = c_InvalidRootParameterIndex;
        RootParamSamplers = c_InvalidRootParameterIndex;
        DescriptorTableSizeRenderResources = 0;
        DescriptorTableSizeSamplers = 0;
        DescriptorRangesRenderResources.Clear();
        DescriptorRangesSamplers.Clear();
        RenderResourceBindingLayouts.Clear();
        RootParams.Clear();
        return true;
    }

    bool RHIBindingLayout_D3D12Impl::Valid() const
    {
		return true;
    }

    const void* RHIBindingLayout_D3D12Impl::NativePtr() const
    {
        return nullptr;
    }

    RHIBindingSet_D3D12Impl::RHIBindingSet_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIBindingSet( a_Device, a_Desc )
    {
    }

    bool RHIBindingSet_D3D12Impl::Release()
    {
		return true;
    }

    bool RHIBindingSet_D3D12Impl::Valid() const
    {
        return true;
    }

    const void* RHIBindingSet_D3D12Impl::NativePtr() const
    {
		return nullptr;
    }

}