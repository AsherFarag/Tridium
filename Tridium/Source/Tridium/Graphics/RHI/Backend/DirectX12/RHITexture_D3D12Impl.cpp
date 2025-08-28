#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	RHITexture_D3D12Impl::RHITexture_D3D12Impl( IDynamicRHI* a_Device, const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: IRHITexture( a_Device, a_Desc )
	{
		const RHIFormatInfo formatInfo = GetRHIFormatInfo( a_Desc.Format );

		const size_t width = a_Desc.Width;
		const size_t height = a_Desc.Height;
		const size_t depth = a_Desc.DepthOrArraySize;
		const size_t stride = width * formatInfo.BytesPerBlock;
		const size_t imgSize = height * stride;

		D3D12_RESOURCE_DESC d3d12Desc = GetD3D12ResourceDesc();

		// Configure the clear value
		D3D12_CLEAR_VALUE clearValue{};
		bool useClearValue = a_Desc.UseClearValue;
		if ( useClearValue )
		{
			clearValue.Format = Translate( a_Desc.Format );
			if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::RenderTarget ) )
			{
				clearValue.Color[0] = a_Desc.ClearValue.Color[0];
				clearValue.Color[1] = a_Desc.ClearValue.Color[1];
				clearValue.Color[2] = a_Desc.ClearValue.Color[2];
				clearValue.Color[3] = a_Desc.ClearValue.Color[3];
			}
			else if ( EnumFlags( a_Desc.BindFlags ).HasFlag( ERHIBindFlags::DepthStencil ) )
			{
				clearValue.DepthStencil.Depth = a_Desc.ClearValue.Depth;
				clearValue.DepthStencil.Stencil = a_Desc.ClearValue.Stencil;
			}
			else
			{
				useClearValue = false; // Clear value is not applicable for this texture type
			}
		}

		// Create the texture
		if ( !Texture.Commit( d3d12Desc, Translate( a_Desc.InitialState ), useClearValue ? &clearValue : nullptr ) )
		{
			ASSERT( false, "Failed to create D3D12 texture" );
			return;
		}

		D3D12_SET_DEBUG_NAME( Texture.Resource(), m_Desc.Name, L"Unnamed Texture");

		if ( a_SubResourcesData.empty() )
			return; // No data to upload

		UINT64 uploadBufferSize = 0;
		UINT numSubresources = Cast<UINT>( a_SubResourcesData.size() );

		Array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
		Array<UINT> numRows;
		Array<UINT64> rowSizeInBytes;

		layouts.Resize( numSubresources );
		numRows.Resize( numSubresources );
		rowSizeInBytes.Resize( numSubresources );

		Device()->GetD3D12Device()->GetCopyableFootprints(
			&d3d12Desc, 0, numSubresources, 0,
			layouts.Data(), numRows.Data(), rowSizeInBytes.Data(), &uploadBufferSize
		);

		// Create the upload buffer
		D3D12::ManagedResource uploadBuffer{};
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = imgSize * depth;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
		{
			ASSERT( false, "Failed to create D3D12 upload buffer" );
			return;
		}

		Array<D3D12_SUBRESOURCE_DATA> d3d12SubResData{};
		d3d12SubResData.Resize( numSubresources );
		for ( UINT i = 0; i < numSubresources; ++i )
		{
			d3d12SubResData[i].pData = a_SubResourcesData[i].Data;
			d3d12SubResData[i].RowPitch = Cast<LONG_PTR>( a_SubResourcesData[i].RowStride );
			d3d12SubResData[i].SlicePitch = Cast<LONG_PTR>( a_SubResourcesData[i].DepthStride );
		}

		const auto beforeBarrier = Translate( RHIResourceBarrier{ this, a_Desc.InitialState, ERHIResourceStates::CopyDest } );
		const auto afterBarrier = Translate( RHIResourceBarrier{ this, ERHIResourceStates::CopyDest, a_Desc.InitialState } );

		auto* cmdList = Device()->GetResourceInitCommandList();
		cmdList->Open();
		cmdList->GetD3D12CmdList()->ResourceBarrier( 1, &beforeBarrier );
		UpdateSubresources(
			cmdList->GetD3D12CmdList(),
			Texture.Resource(),
			uploadBuffer.Resource(),
			0, 0,
			numSubresources,
			d3d12SubResData.Data()
		);
		cmdList->GetD3D12CmdList()->ResourceBarrier( 1, &afterBarrier );
		cmdList->Close();

		IRHICommandList* cmdListPtr = cmdList;
		const RHIFenceValue fence = Device()->ExecuteCommandLists( Span{ &cmdListPtr, 1 }, ERHICommandQueueType::Copy );
		Device()->WaitForFence( ERHICommandQueueType::Copy, fence );
	}

	bool RHITexture_D3D12Impl::Release()
	{
		Texture.Release();
		return true;
	}

	D3D12_RESOURCE_DESC RHITexture_D3D12Impl::GetD3D12ResourceDesc() const
	{
		D3D12_RESOURCE_DESC desc{};

		desc.Alignment = 0;
		desc.Width = UINT{ m_Desc.Width };
		desc.Height = UINT{ m_Desc.Height };
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = Cast<UINT16>( m_Desc.Mips );
		desc.SampleDesc.Count = UINT{ m_Desc.Samples };
		desc.SampleDesc.Quality = 0;

		if ( m_Desc.IsArray() || m_Desc.Is3D() )
			desc.DepthOrArraySize = m_Desc.DepthOrArraySize;
		else
			desc.DepthOrArraySize = 1;

		if ( m_Desc.Is1D() )
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		else if ( m_Desc.Is2D() )
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		else if ( m_Desc.Is3D() )
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		else
			ASSERT( false, "Invalid texture dimension" );

		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		EnumFlags bindFlags = m_Desc.BindFlags;
		if ( bindFlags.HasFlag( ERHIBindFlags::RenderTarget ) )
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if ( bindFlags.HasFlag( ERHIBindFlags::DepthStencil ) )
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if ( bindFlags.HasFlag( ERHIBindFlags::UnorderedAccess ) )
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		if ( !bindFlags.HasFlag( ERHIBindFlags::ShaderResource ) && bindFlags.HasFlag( ERHIBindFlags::DepthStencil ) )
			desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		desc.Format = D3D12::Translate( m_Desc.Format );
		if ( desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB && EnumFlags( desc.Flags ).HasFlag( D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS ) )
			desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;

		return desc;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC RHITexture_D3D12Impl::CreateSRVDesc( ERHIFormat a_Format, ERHITextureDimension a_Dimension, RHITextureSubresourceSet a_Subresources )
	{
		if ( a_Dimension == ERHITextureDimension::Unknown )
			a_Dimension = m_Desc.Dimension;
		if ( a_Format == ERHIFormat::Unknown )
			a_Format = m_Desc.Format;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Translate( a_Format );
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		const uint32_t planeSlice = (srvDesc.Format == DXGI_FORMAT_X24_TYPELESS_G8_UINT) ? 1 : 0;
		a_Subresources = a_Subresources.Resolve( m_Desc, false );

		switch ( a_Dimension )
		{
			case ERHITextureDimension::Texture1D:
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				srvDesc.Texture1D.MostDetailedMip = a_Subresources.BaseMipLevel;
				srvDesc.Texture1D.MipLevels = a_Subresources.NumMipLevels;
				break;
			case ERHITextureDimension::Texture2D:
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = a_Subresources.BaseMipLevel;
				srvDesc.Texture2D.MipLevels = a_Subresources.NumMipLevels;
				srvDesc.Texture2D.PlaneSlice = planeSlice; // For planar formats
				break;
			case ERHITextureDimension::Texture3D:
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
				srvDesc.Texture3D.MostDetailedMip = a_Subresources.BaseMipLevel;
				srvDesc.Texture3D.MipLevels = a_Subresources.NumMipLevels;
				break;
			case ERHITextureDimension::TextureCube:
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MostDetailedMip = a_Subresources.BaseMipLevel;
				srvDesc.TextureCube.MipLevels = a_Subresources.NumMipLevels;
				break;
			default:
				ASSERT( false, "Unsupported texture dimension for SRV" );
				return {};
		}

		TODO( "Add support for array textures and other dimensions" );

		return srvDesc;
	}

} // namespace Tridium::D3D12