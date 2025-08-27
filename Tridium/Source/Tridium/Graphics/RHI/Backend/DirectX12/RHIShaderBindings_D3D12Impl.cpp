#include "tripch.h"
#include "RHI_D3D12Impl.h"

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
                D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.EmplaceBack();
                switch ( binding.Type() )
                {
                    // SRV
                case ERHIBindingType::StructuredBuffer:
                case ERHIBindingType::Texture:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

                    // We combine samplers and textures into a single binding
					// So we need to create a sampler range if this is a texture binding
                    D3D12_DESCRIPTOR_RANGE1& samplerRange = DescriptorRangesSamplers.EmplaceBack();
                    samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    samplerRange.NumDescriptors = 1;
                    samplerRange.BaseShaderRegister = binding.Slot;
                    samplerRange.RegisterSpace = a_Desc.RegisterSpace;
                    samplerRange.OffsetInDescriptorsFromTableStart = DescriptorTableSizeSamplers++;
                    samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                    break;
                }
                // UAV
                case ERHIBindingType::StorageBuffer:
                case ERHIBindingType::StorageTexture:
                {
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

					// We combine samplers and textures into a single binding
					// So we need to create a sampler range if this is a texture binding
                    D3D12_DESCRIPTOR_RANGE1& samplerRange = DescriptorRangesSamplers.EmplaceBack();
                    samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                    samplerRange.NumDescriptors = 1;
                    samplerRange.BaseShaderRegister = binding.Slot;
                    samplerRange.RegisterSpace = a_Desc.RegisterSpace;
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
                    D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesSamplers.EmplaceBack();
                    range.NumDescriptors += 1;
                    DescriptorTableSizeSamplers++;
                }

                ASSERT( !DescriptorRangesRenderResources.Empty() );
                D3D12_DESCRIPTOR_RANGE1& range = DescriptorRangesRenderResources.EmplaceBack();
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
		RHI_DEV_CHECK( a_Desc.Layout, "Binding set must have a valid layout" );

    #if 0
        // Validate the binding layout
        for ( uint32_t i = 0; i < a_Desc.Bindings.Size(); ++i )
        {
            const auto& bindingSetItem = a_Desc.Bindings[i];
			const auto& bindingLayoutItem = a_Desc.Layout->Desc().Bindings[i];
            if ( bindingLayoutItem.Type() == ERHIBindingType::InlinedConstants )
				continue; // Inlined constants are not stored in the binding set

            RHI_DEV_CHECK( bindingSetItem.Type == bindingLayoutItem.Type(),
                "Binding type mismatch at index {}: expected {}, got {}",
                i, ToString( bindingLayoutItem.Type() ), ToString( bindingSetItem.Type ) );

            RHI_DEV_CHECK( bindingSetItem.Slot == bindingLayoutItem.Slot,
                "Binding slot mismatch at index {}: expected {}, got {}",
                i, bindingLayoutItem.Slot, bindingSetItem.Slot );
		}
    #endif

		// Create Descriptor Heaps for Samplers and Render Resources

		auto* const layout = a_Desc.Layout->As<RHIBindingLayout_D3D12Impl>();
        if ( layout->DescriptorTableSizeSamplers > 0 )
        {
			SamplerHeap = Device()->GetDescriptorHeapManager().AllocateHeap(
                ERHIDescriptorHeapType::Sampler,
				layout->DescriptorTableSizeSamplers,
                EDescriptorHeapFlags::GPUVisible
            );

            for ( const auto& range : layout->DescriptorRangesSamplers )
            {
                for ( uint32_t i = 0; i < range.NumDescriptors; ++i )
                {
                    const uint32_t slot = range.BaseShaderRegister + i;
					D3D12_CPU_DESCRIPTOR_HANDLE handle = SamplerHeap->GetCPUHandle( range.OffsetInDescriptorsFromTableStart + i );
					bool found = false;

                    for ( const auto& binding : a_Desc.Bindings )
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

        if ( layout->DescriptorTableSizeRenderResources > 0 )
        {
            RenderResourceHeap = Device()->GetDescriptorHeapManager().AllocateHeap(
                ERHIDescriptorHeapType::RenderResource,
                layout->DescriptorTableSizeRenderResources,
                EDescriptorHeapFlags::GPUVisible
            );

            for ( const auto& range : layout->DescriptorRangesRenderResources )
            {
                for ( uint32_t i = 0; i < range.NumDescriptors; ++i )
                {
                    const uint32_t slot = range.BaseShaderRegister + i;
                    D3D12_CPU_DESCRIPTOR_HANDLE handle = RenderResourceHeap->GetCPUHandle( range.OffsetInDescriptorsFromTableStart + i );
                    bool found = false;

					IRHIResource* resource = nullptr;

                    for ( const auto& binding : a_Desc.Bindings )
                    {
                        if ( binding.Type == ERHIBindingType::StructuredBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV )
                        {
                            if ( binding.Resource )
                            {
								auto* buffer = binding.Resource->As<RHIBuffer_D3D12Impl>();
								resource = buffer;

								D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = buffer->CreateSRVDesc( binding.BufferType, binding.Range, binding.Format );
                                Device()->GetD3D12Device()->CreateShaderResourceView(
									buffer->ManagedBuffer.Resource(),
                                    &srvDesc,
                                    handle
                                );
                            }
                            else
                            {
								// Create a Null SRV for this slot
								D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
								srvDesc.Format = Translate( binding.Format );
								srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
								srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                                Device()->GetD3D12Device()->CreateShaderResourceView(
                                    nullptr, // Null resource for empty SRV
                                    &srvDesc,
                                    handle
								);
                            }

							found = true;
                            break;
                        }
                        else if ( binding.Type == ERHIBindingType::StorageBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV )
                        {
                            NOT_IMPLEMENTED;
                        }
                        else if ( binding.Type == ERHIBindingType::Texture && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV )
                        {
							auto* texture = binding.Resource->As<RHITexture_D3D12Impl>();
                            resource = texture;
                            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = texture->CreateSRVDesc( binding.Format, binding.TextureDimension, binding.Subresources );
                            Device()->GetD3D12Device()->CreateShaderResourceView(
                                texture->Texture.Resource(),
                                &srvDesc,
                                handle
							);
                            found = true;
                        }
                        else if ( binding.Type == ERHIBindingType::StorageTexture && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV )
                        {
							NOT_IMPLEMENTED;
                        }
                        else if ( binding.Type == ERHIBindingType::ConstantBuffer && range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV )
                        {
							auto* buffer = binding.Resource->As<RHIBuffer_D3D12Impl>();
                            resource = buffer;
                            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = buffer->CreateCBVDesc( binding.Range );
                            Device()->GetD3D12Device()->CreateConstantBufferView(
                                &cbvDesc,
                                handle
                            );
                            found = true;
                        }
                    }

                    if ( resource )
						Resources.EmplaceBack( resource->Shared() );

                    if ( !found )
                    {
						// Create a Null SRV, UAV or CBV for this slot
                        switch ( range.RangeType )
                        {
                        case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                        {
                            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                            srvDesc.Format = DXGI_FORMAT_R32_UINT;
                            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                            Device()->GetD3D12Device()->CreateShaderResourceView(
                                nullptr, // Null resource for empty SRV
                                &srvDesc,
                                handle
                            );
                            break;
                        }
                        case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        {
                            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
                            uavDesc.Format = DXGI_FORMAT_R32_UINT;
                            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                            Device()->GetD3D12Device()->CreateUnorderedAccessView(
                                nullptr, // Null resource for empty UAV
                                nullptr,
                                &uavDesc,
                                handle
                            );
							break;
                        }
						case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                        {
                            Device()->GetD3D12Device()->CreateConstantBufferView(
                                nullptr, // Null resource for empty CBV
                                handle
                            );
							break;
                        }
                        }
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