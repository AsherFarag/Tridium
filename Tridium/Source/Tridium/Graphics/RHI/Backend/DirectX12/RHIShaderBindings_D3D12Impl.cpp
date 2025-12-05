#include "tripch.h"
#include "RHI_D3D12Impl.h"
#include <ranges>

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

        // Bindless texture arrays are compatible with other SRV types
        if (   ( a_First == BindlessTextureArray && ( a_Second == Texture || a_Second == StructuredBuffer ) )
            || ( a_Second == BindlessTextureArray && ( a_First == Texture || a_First == StructuredBuffer ) ) )
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
        // Shader bindings must be sorted.
        TODO( "Should we be doing it here?" );
        std::sort( m_Desc.Bindings.Begin(), m_Desc.Bindings.End(), []( const RHIShaderBinding& a_LHS, const RHIShaderBinding& a_RHS )
            {
                if ( a_LHS.Type() != a_RHS.Type() )
                {
                    return a_LHS.Type() < a_RHS.Type();
                }

                return a_LHS.Slot < a_RHS.Slot;
		} );

		// Now update the binding map to reflect the new indices.
        {
            ERHIBindingType lastType = ERHIBindingType::Unknown;
            for ( size_t i = 0; i < m_Desc.Bindings.Size(); i++ )
            {
                const RHIShaderBinding& binding = m_Desc.Bindings[i];
                m_Desc.BindingMap[binding.NameHash].first = uint32_t( i );

                if ( binding.Type() != lastType )
                {
                    switch ( binding.Type() )
                    {
                    case ERHIBindingType::ConstantBuffer:
                        CBVBindingOffset = i;
                        break;
                    case ERHIBindingType::StructuredBuffer:
                    case ERHIBindingType::Texture:
                        if ( lastType != ERHIBindingType::ConstantBuffer )
                        {
                            CBVBindingOffset = i;
                        }
                        SRVBindingOffset = i;
                        break;
                    case ERHIBindingType::StorageBuffer:
                    case ERHIBindingType::StorageTexture:
                        if ( lastType != ERHIBindingType::ConstantBuffer
                            && lastType != ERHIBindingType::StructuredBuffer
                            && lastType != ERHIBindingType::Texture )
                        {
                            CBVBindingOffset = i;
                            SRVBindingOffset = i;
                        }
                        UAVBindingOffset = i;
                        break;
                    case ERHIBindingType::BindlessTextureArray:
                        // Bindless arrays are treated as SRVs
                        if ( lastType != ERHIBindingType::ConstantBuffer )
                        {
                            CBVBindingOffset = i;
                        }
                        SRVBindingOffset = i;
                        break;
                    default:
                        break;
                    }
                    lastType = binding.Type();
				}
            }
        }

        uint32_t currentSlot = ~0u;
        ERHIBindingType currentType = ERHIBindingType::Unknown;
		D3D12_ROOT_CONSTANTS rootConstants{};

        for ( const auto& binding : m_Desc.Bindings )
        {
            if ( binding.Type() == ERHIBindingType::InlinedConstants )
            {
                InlinedConstantsSize = binding.InlinedConstantSize();
                rootConstants.Num32BitValues = NumDWORDsFromBytes( InlinedConstantsSize );
                rootConstants.ShaderRegister = binding.Slot;
                rootConstants.RegisterSpace = INLINED_CONSTANTS_SPACE;
            }
            // Do we need to start a new range?
            else if ( !AreBindingsCompatible( binding.Type(), currentType )
                || binding.Slot != currentSlot + 1 )
            {
                D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.EmplaceBack();
                switch ( binding.Type() )
                {
                case ERHIBindingType::StructuredBuffer:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    break;
                }
                case ERHIBindingType::Texture:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

                    // We combine samplers and textures into a single binding
					// So we need to create a sampler range if this is a texture binding
                    D3D12_DESCRIPTOR_RANGE1& samplerRange = DescriptorRangesSamplers.EmplaceBack();
                    samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    samplerRange.NumDescriptors = 1;
                    samplerRange.BaseShaderRegister = binding.Slot;
                    samplerRange.RegisterSpace = m_Desc.RegisterSpace;
                    samplerRange.OffsetInDescriptorsFromTableStart = DescriptorTableSizeSamplers++;
                    samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                    break;
                }
                case ERHIBindingType::StorageBuffer:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
					break;
                }
                case ERHIBindingType::StorageTexture:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

					// We combine samplers and textures into a single binding
					// So we need to create a sampler range if this is a texture binding
                    D3D12_DESCRIPTOR_RANGE1& samplerRange = DescriptorRangesSamplers.EmplaceBack();
                    samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    samplerRange.NumDescriptors = 1;
                    samplerRange.BaseShaderRegister = binding.Slot;
                    samplerRange.RegisterSpace = m_Desc.RegisterSpace;
                    samplerRange.OffsetInDescriptorsFromTableStart = DescriptorTableSizeSamplers++;
                    samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

                    break;
                }
                // CBV
                case ERHIBindingType::ConstantBuffer:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                    break;
                }
                case ERHIBindingType::BindlessTextureArray:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

                    // For bindless, use unbounded descriptor arrays (UINT_MAX descriptors)
                    // This requires Shader Model 6.6+ and Resource Binding Tier 3
                    range.NumDescriptors = UINT_MAX;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

                    // We combine samplers and textures into a single binding
                    // For bindless arrays, we also need an unbounded sampler array
                    D3D12_DESCRIPTOR_RANGE1& samplerRange = DescriptorRangesSamplers.EmplaceBack();
                    samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    samplerRange.NumDescriptors = UINT_MAX;
                    samplerRange.BaseShaderRegister = binding.Slot;
                    samplerRange.RegisterSpace = m_Desc.RegisterSpace;
                    samplerRange.OffsetInDescriptorsFromTableStart = 0;
                    samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
                    DescriptorTableSizeSamplers = UINT_MAX;
                    break;
                }
                default:
                {
                    RHI_DEV_CHECK( false, "Invalid binding type '{}'", ToString( binding.Type() ) );
                    continue;
                }
                }

                // For bindless arrays, NumDescriptors is already set to UINT_MAX above
                if ( binding.Type() != ERHIBindingType::BindlessTextureArray )
                {
                    range.NumDescriptors = 1;
                    range.BaseShaderRegister = binding.Slot;
                    range.RegisterSpace = m_Desc.RegisterSpace;
                    range.OffsetInDescriptorsFromTableStart = DescriptorTableSizeRenderResources++;
                    range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                }
                else
                {
                    // Bindless arrays already have their properties set
                    range.BaseShaderRegister = binding.Slot;
                    range.RegisterSpace = m_Desc.RegisterSpace;
                    range.OffsetInDescriptorsFromTableStart = 0;
                    DescriptorTableSizeRenderResources = UINT_MAX;
                }

                RenderResourceBindingLayouts.PushBack( binding );

                currentSlot = binding.Slot;
                currentType = binding.Type();
            }
            // Extend the current range
            else
            {
				// If the binding is a texture, we also need to extend the sampler range
                if ( binding.Type() == ERHIBindingType::Texture
                    || binding.Type() == ERHIBindingType::StorageTexture )
                {
                    ASSERT( !DescriptorRangesSamplers.Empty() );
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesSamplers.Back();
                    range.NumDescriptors += 1;
                    DescriptorTableSizeSamplers++;
                }

                ASSERT( !DescriptorRangesRenderResources.Empty() );
                D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.Back();
                range.NumDescriptors += 1;
                DescriptorTableSizeRenderResources++;
                RenderResourceBindingLayouts.PushBack( binding );

                currentSlot = binding.Slot;
            }
        }

        RootParams.Clear();

        if ( rootConstants.Num32BitValues )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            param.ShaderVisibility = Translate( m_Desc.Visibility );
            param.Constants = rootConstants;
            RootParamInlinedConstants = RootParameterIndex( RootParams.Size() - 1 );
        }

        if ( DescriptorTableSizeSamplers )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = Translate( m_Desc.Visibility );
            param.DescriptorTable.NumDescriptorRanges = UINT( DescriptorRangesSamplers.Size() );
            param.DescriptorTable.pDescriptorRanges = &DescriptorRangesSamplers[0];
            RootParamSamplers = RootParameterIndex( RootParams.Size() - 1 );
        }

        if ( DescriptorTableSizeRenderResources > 0 )
        {
            D3D12_ROOT_PARAMETER1& param = RootParams.EmplaceBack();
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = Translate( m_Desc.Visibility );
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
		RHI_DEV_CHECK( a_Desc.Layout, "Binding set must have a valid layout" );

		// Create Descriptor Heaps for Samplers and Render Resources

		auto* const layout = a_Desc.Layout->As<RHIBindingLayout_D3D12Impl>();
        if ( layout->DescriptorTableSizeSamplers > 0 && layout->DescriptorTableSizeSamplers != UINT_MAX )
        {
			SamplerHeap = Device()->GetDescriptorHeapManager().AllocateHeap(
                ERHIDescriptorHeapType::Sampler,
				layout->DescriptorTableSizeSamplers,
                EDescriptorHeapFlags::Poolable
            );

            for ( const auto& range : layout->DescriptorRangesSamplers )
            {
                // Skip bindless arrays - they don't need pre-initialization
                if ( range.NumDescriptors == UINT_MAX )
                    continue;

                for ( uint32_t i = 0; i < range.NumDescriptors; ++i )
                {
                    const uint32_t slot = range.BaseShaderRegister + i;
					D3D12_CPU_DESCRIPTOR_HANDLE handle = SamplerHeap->GetCPUHandle( range.OffsetInDescriptorsFromTableStart + i );
					bool found = false;

					for ( const auto& binding : m_Desc.Bindings )
                    {
						if ( (binding.Type == ERHIBindingType::Texture || binding.Type == ERHIBindingType::StorageTexture)
                            && binding.Slot == slot )
                        {
                            if ( binding.Resource || binding.Sampler.Valid() )
                            {
                                RHISampler sampler = binding.Sampler.Valid()
                                    ? binding.Sampler.Unpack()
									: binding.Resource->As<IRHITexture>()->Desc().DefaultSampler;

								D3D12_SAMPLER_DESC d3d12Sampler = Translate<D3D12_SAMPLER_DESC, RHISampler>( sampler );
                                Device()->GetD3D12Device()->CreateSampler(
                                    &d3d12Sampler,
                                    handle
                                );
                            }

                            found = true;
                            break;
                        }
                    }

                    if ( !found )
                    {
                        // Create a default sampler if no binding was found for this slot
						TODO( "Create a Default RHI Sampler to use here instead of D3D12 default sampler" );
						const D3D12_SAMPLER_DESC samplerDesc = Translate<D3D12_SAMPLER_DESC, RHISampler>( RHISampler{} );
                        Device()->GetD3D12Device()->CreateSampler(
                            &samplerDesc,
                            handle
						);
					}
                }
            }
		}

        if ( layout->DescriptorTableSizeRenderResources <= 0 || layout->DescriptorTableSizeRenderResources == UINT_MAX )
        {
			// For bindless arrays (UINT_MAX), we'll use a GPU-visible heap managed differently
			// For now, skip the traditional heap allocation
			return;
        }

        RenderResourceHeap = Device()->GetDescriptorHeapManager().AllocateHeap(
            ERHIDescriptorHeapType::RenderResource,
            layout->DescriptorTableSizeRenderResources,
            EDescriptorHeapFlags::Poolable
        );

        for ( const auto& range : layout->DescriptorRangesRenderResources )
        {
            // Skip bindless arrays - they don't need pre-initialization
            if ( range.NumDescriptors == UINT_MAX )
                continue;

            for ( uint32_t i = 0; i < range.NumDescriptors; ++i )
            {
                const uint32_t slot = range.BaseShaderRegister + i;
                D3D12_CPU_DESCRIPTOR_HANDLE handle = RenderResourceHeap->GetCPUHandle( range.OffsetInDescriptorsFromTableStart + i );
                bool found = false;

                IRHIResource* resource = nullptr;

                for ( const auto& binding : a_Desc.Bindings )
                {
					if ( binding.Slot != slot || !binding.Resource )
                    {
						 // Only consider bindings for this slot that have a resource
                        continue;
                    }

                    // Structured buffer SRV
                    if ( binding.Type == ERHIBindingType::StructuredBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV )
                    {
                        auto* buffer = binding.Resource->As<RHIBuffer_D3D12Impl>();
                        resource = buffer;

                        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = buffer->CreateSRVDesc( binding.BufferType, binding.Range, binding.Format );
                        Device()->GetD3D12Device()->CreateShaderResourceView(
                            buffer->ManagedBuffer.Resource(),
                            &srvDesc,
                            handle
                        );

                        found = true;
                        break;
                    }

                    // Texture SRV
                    if ( binding.Type == ERHIBindingType::Texture && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV )
                    {
                        auto* texture = binding.Resource->As<RHITexture_D3D12Impl>();
                        resource = texture;
                        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = texture->CreateSRVDesc( binding.Format, binding.TextureDimension, binding.Subresources.Resolve( texture->Desc(), false ) );
                        Device()->GetD3D12Device()->CreateShaderResourceView(
                            texture->Texture.Resource(),
                            &srvDesc,
                            handle
                        );

                        found = true;
                        break;
                    }

                    // Constant buffer CBV
                    if ( binding.Type == ERHIBindingType::ConstantBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV )
                    {
                        auto* buffer = binding.Resource->As<RHIBuffer_D3D12Impl>();
                        resource = buffer;
                        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = buffer->CreateCBVDesc( binding.Range );
                        Device()->GetD3D12Device()->CreateConstantBufferView(
                            &cbvDesc,
                            handle
                        );

                        found = true;
                        break;
                    }

                    // Storage buffer UAV
                    if ( binding.Type == ERHIBindingType::StorageBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV )
                    {
                        NOT_IMPLEMENTED;

                        found = true;
                        break;
                    }

                    // Storage texture UAV
                    if ( binding.Type == ERHIBindingType::StorageTexture && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV )
                    {
                        NOT_IMPLEMENTED;

                        found = true;
                        break;
                    }
                }

                if ( resource )
                {
                    Resources.EmplaceBack( resource->Shared() );
                }

                if ( !found )
                {
                    // Create appropriate NULL view based on range.RangeType (SRV/UAV/CBV)
                    if ( range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV )
                    {
                        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                        srvDesc.Format = DXGI_FORMAT_R32_UINT;
                        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

						TODO( "Bit sketchy here, revisit the -1" );
                        const size_t bindingIndex = layout->SRVBindingOffset + slot;
						if ( layout->Desc().Bindings.IsValidIndex( bindingIndex ) )
                        {
                            const RHIShaderBinding& binding = layout->Desc().Bindings[bindingIndex];
                            if ( binding.Type() == ERHIBindingType::StructuredBuffer )
                            {
                                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                            }
                            else
                            {
                                srvDesc.ViewDimension = To<D3D12_SRV_DIMENSION>::From( binding.TextureDimension() );
                            }
                        }
                        else
                        {
							//RHI_DEV_CHECK( false, "Invalid binding index for null SRV creation" );
							srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                        }

                        Device()->GetD3D12Device()->CreateShaderResourceView( nullptr, &srvDesc, handle );
                    }
                    else if ( range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV )
                    {
                        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
                        uavDesc.Format = DXGI_FORMAT_R32_UINT;
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                        Device()->GetD3D12Device()->CreateUnorderedAccessView( nullptr, nullptr, &uavDesc, handle );
                    }
                    else if ( range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV )
                    {
                        D3D12_CONSTANT_BUFFER_VIEW_DESC nullCbv = {};
                        nullCbv.BufferLocation = 0;
                        nullCbv.SizeInBytes = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
                        Device()->GetD3D12Device()->CreateConstantBufferView( &nullCbv, handle );
                    }
                }
            }
        }
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