#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	struct ImageLevel
	{
		Array<uint8_t> Data;
		size_t Width = 0;
		size_t Height = 0;
		size_t Depth = 0;
		size_t BytesPerPixel = 0;
	};
	
	TODO( "Mip generation should be moved to the GPU" );
	static ImageLevel GenerateNextMip(
		Span<const uint8_t> a_Data,
		size_t a_Width, size_t a_Height, size_t a_Depth,
		size_t a_BytesPerPixel )
	{
			// Next mip dimensions
		const size_t dstWidth = std::max<size_t>( 1, a_Width / 2 );
		const size_t dstHeight = std::max<size_t>( 1, a_Height / 2 );
		const size_t dstDepth = std::max<size_t>( 1, a_Depth / 2 );

		ImageLevel dst{};
		dst.Width = dstWidth;
		dst.Height = dstHeight;
		dst.Depth = dstDepth;
		dst.BytesPerPixel = a_BytesPerPixel;
		dst.Data.Resize( dstWidth * dstHeight * dstDepth * a_BytesPerPixel );

		const size_t srcRowStride = a_Width * a_BytesPerPixel;
		const size_t srcSliceStride = srcRowStride * a_Height;

		const size_t dstRowStride = dstWidth * a_BytesPerPixel;
		const size_t dstSliceStride = dstRowStride * dstHeight;

		for ( size_t z = 0; z < dstDepth; ++z )
		{
			for ( size_t y = 0; y < dstHeight; ++y )
			{
				for ( size_t x = 0; x < dstWidth; ++x )
				{
					uint64_t accum[4] = {}; // up to 4 channels, extend if needed
					const size_t samples = 8; // 2x2x2 box filter for 3D, 2D if depth=1

					size_t actualSamples = 0;

					for ( size_t dz = 0; dz < 2 && ( z * 2 + dz ) < a_Depth; ++dz )
					{
						for ( size_t yy = 0; yy < 2 && ( y * 2 + yy ) < a_Height; ++yy )
						{
							for ( size_t xx = 0; xx < 2 && ( x * 2 + xx ) < a_Width; ++xx )
							{
								const uint8_t* src = a_Data.data() +
									( ( z * 2 + dz ) * srcSliceStride ) +
									( ( y * 2 + yy ) * srcRowStride ) +
									( ( x * 2 + xx ) * a_BytesPerPixel );

								for ( size_t c = 0; c < a_BytesPerPixel; ++c )
									accum[c] += src[c];

								actualSamples++;
							}
						}
					}

					// Write averaged pixel
					uint8_t* dstPixel = dst.Data.Data() +
						( z * dstSliceStride + y * dstRowStride + x * a_BytesPerPixel );

					for ( size_t c = 0; c < a_BytesPerPixel; ++c )
						dstPixel[c] = static_cast<uint8_t>( accum[c] / actualSamples );
				}
			}
		}

		return dst;
	}

	RHITexture_D3D12Impl::RHITexture_D3D12Impl( IDynamicRHI* a_Device, const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: IRHITexture( a_Device, a_Desc )
	{
		RHI_DEV_CHECK( m_Desc.HeapType != ERHIHeapType::Immutable || !a_SubResourcesData.empty(),
					   "Immutable textures must be created with initial data" );

		// Staging textures are a special case as they are not actually textures but buffers.
		// This is because D3D12 does not allow textures on readback heaps.
		if ( m_Desc.HeapType == ERHIHeapType::Staging )
		{
			RHI_DEV_WARN( a_SubResourcesData.empty(),
						  "Staging texture '{}' cannot be created with initial data, ignoring provided data",
						  a_Desc.Name );

			if ( !CommitAsStagingTexture() )
			{
				ASSERT( false, "Failed to create staging texture" );
			}

			return;
		}

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
			clearValue.Format = GetDXGIFormatMap( a_Desc.Format ).RTVFormat;
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

		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		switch ( m_Desc.HeapType )
		{
			case ERHIHeapType::Dynamic: // We just treat dynamic as default for now
			case ERHIHeapType::Immutable:
			case ERHIHeapType::Default:
				allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				initialState = Translate( m_Desc.InitialState );
				break;
			case ERHIHeapType::Staging:
				allocDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
				initialState = D3D12_RESOURCE_STATE_COPY_DEST;
				break;
			default:
				RHI_DEV_CHECK( false, "Unsupported heap type for texture '{}'", m_Desc.Name );
				return;
		}

		// Create the texture
		if ( !Texture.Commit( d3d12Desc, allocDesc, initialState, useClearValue ? &clearValue : nullptr ) )
		{
			ASSERT( false, "Failed to create D3D12 texture" );
			return;
		}

		D3D12_SET_DEBUG_NAME( Texture.Resource(), m_Desc.Name, L"Unnamed Texture");

		SetState( m_Desc.InitialState );

		if ( a_SubResourcesData.empty() )
		{
			return; // No data to upload
		}

		const UINT numSubresources = m_Desc.Mips * ( m_Desc.IsArray() ? m_Desc.DepthOrArraySize : 1 );
		UINT64 uploadBufferSize = 0;

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
		{
			D3D12MA::ALLOCATION_DESC uploadBufferAllocDesc = {};
			uploadBufferAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC uploadBufferDesc = {};
			uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			uploadBufferDesc.Width = uploadBufferSize;
			uploadBufferDesc.Height = 1;
			uploadBufferDesc.DepthOrArraySize = 1;
			uploadBufferDesc.MipLevels = 1;
			uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			uploadBufferDesc.SampleDesc.Count = 1;
			uploadBufferDesc.SampleDesc.Quality = 0;
			uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			if ( !uploadBuffer.Commit( uploadBufferDesc, uploadBufferAllocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			{
				ASSERT( false, "Failed to create D3D12 upload buffer" );
				return;
			}
		}

		// Generate the full mip chain if only one mip level was provided and mips are requested
		Array<RHITextureSubresourceData> mipChain;
		Array<ImageLevel> imageMips;
		if ( a_SubResourcesData.size() != numSubresources )
		{
			mipChain.Reserve( m_Desc.Mips );
			mipChain.EmplaceBack( a_SubResourcesData[0] );
			auto lastMip = a_SubResourcesData[0];
			for ( uint32_t mip = 1; mip < m_Desc.Mips; ++mip )
			{
				// Generate next mip level and add to chain
				const ImageLevel& nextMip = imageMips.EmplaceBack( 
					GenerateNextMip(
						Span{ Cast<const uint8_t*>( lastMip.Data ), lastMip.DepthStride },
						std::max<size_t>( 1, width >> ( mip - 1 ) ),
						std::max<size_t>( 1, height >> ( mip - 1 ) ),
						std::max<size_t>( 1, depth >> ( mip - 1 ) ),
						formatInfo.BytesPerBlock 
					) 
				);

				RHITextureSubresourceData nextMipData{};
				nextMipData.Data = nextMip.Data.Data();
				nextMipData.RowStride = nextMip.Width * formatInfo.BytesPerBlock;
				nextMipData.DepthStride = nextMipData.RowStride * nextMip.Height;
				mipChain.EmplaceBack( nextMipData );
				lastMip = nextMipData;
			}

			a_SubResourcesData = mipChain;
		}

		// Copy the subresource data into the upload buffer
		// Suppose a_SubResourcesData[0] points to either:
		//  - a single 2D face image (and you want the same for all faces), or
		//  - a buffer that already contains 6 faces concatenated (face0, face1, ... face5).
		// You need to produce numSubresources D3D12_SUBRESOURCE_DATA entries.

		const UINT arraySize = static_cast<UINT>( m_Desc.IsArray() ? m_Desc.DepthOrArraySize : 1 ); // 6 for cube
		const UINT mipCount = static_cast<UINT>( m_Desc.Mips );
		const UINT totalSubresources = mipCount * arraySize;

		Array<D3D12_SUBRESOURCE_DATA> d3d12SubResData;
		d3d12SubResData.Resize( totalSubresources );

		if ( a_SubResourcesData.size() == 1 )
		{
			// Compute bytes per face for base mip (width*height*bytesPerPixel).
			const uint8_t* basePtr = static_cast<const uint8_t*>( a_SubResourcesData[0].Data );
			const size_t baseRow = a_SubResourcesData[0].RowStride;
			const size_t baseSlice = a_SubResourcesData[0].DepthStride;
			// If user provided a single face and wants same for all faces:
			for ( UINT mip = 0; mip < mipCount; ++mip )
			{
				size_t mipWidth = std::max<size_t>( 1, width >> mip );
				size_t mipHeight = std::max<size_t>( 1, height >> mip );
				size_t mipRow = mipWidth * formatInfo.BytesPerBlock;
				size_t mipSlice = mipRow * mipHeight;
				for ( UINT slice = 0; slice < arraySize; ++slice )
				{
					UINT index = mip * arraySize + slice;

					// Option A: if the single buffer contains all 6 faces sequentially for each mip,
					// you need to compute the correct offset into basePtr:
					// offset = (slice * baseSlice_for_mip0) + (some offset for mip levels)
					// But simplest: if the provided base is per-face (same for all faces), just point to same memory:
					d3d12SubResData[index].pData = basePtr;
					d3d12SubResData[index].RowPitch = Cast<LONG_PTR>( mipRow );
					d3d12SubResData[index].SlicePitch = Cast<LONG_PTR>( mipSlice );
				}
			}
		}
		else
		{
			// If a_SubResourcesData already contains one entry per (mip * slice) in the correct order,
			// copy pointers directly:
			for ( UINT i = 0; i < totalSubresources; ++i )
			{
				d3d12SubResData[i].pData = a_SubResourcesData[i].Data;
				d3d12SubResData[i].RowPitch = Cast<LONG_PTR>( a_SubResourcesData[i].RowStride );
				d3d12SubResData[i].SlicePitch = Cast<LONG_PTR>( a_SubResourcesData[i].DepthStride );
			}
		}


		D3D12_RESOURCE_BARRIER beforeBarrier{};
		beforeBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		beforeBarrier.Transition.pResource = Texture.Resource();
		beforeBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		beforeBarrier.Transition.StateBefore = initialState;
		beforeBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

		D3D12_RESOURCE_BARRIER afterBarrier{};
		afterBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		afterBarrier.Transition.pResource = Texture.Resource();
		afterBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		afterBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		afterBarrier.Transition.StateAfter = initialState;

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
		const RHIFenceValue fence = Device()->ExecuteCommandLists( Span{ &cmdListPtr, 1 }, cmdListPtr->Desc().QueueType );
		Device()->WaitForFence( cmdListPtr->Desc().QueueType, fence );
	}

	bool RHITexture_D3D12Impl::Release()
	{
		Texture.Release();
		return true;
	}

	RHITextureSubresourceData RHITexture_D3D12Impl::MapSubresource( const RHITextureSlice& a_Slice )
	{
		const auto slice = a_Slice.Resolve( m_Desc );
		const uint32_t subresourceIndex = CalcSubresource(
			slice.MipLevel,
			slice.ArraySlice,
			0,
			m_Desc.Mips,
			m_Desc.IsArray() ? m_Desc.DepthOrArraySize : 1
		);

		if ( m_Desc.HeapType != ERHIHeapType::Staging )
			return {}; // only staging supported here

		void* mappedPtr = nullptr;
		D3D12_RANGE range = { 0, 0 }; // no invalidate
		HRESULT hr = Texture.Resource()->Map( 0, &range, &mappedPtr );
		if ( FAILED( hr ) )
			return {};

		const D3D12_RESOURCE_DESC d3dDesc = GetD3D12ResourceDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		UINT numRows;
		UINT64 rowSizeInBytes, totalBytes;
		Device()->GetD3D12Device()->GetCopyableFootprints(
			&d3dDesc,
			subresourceIndex,
			1,
			m_SubresourceOffsets[subresourceIndex],
			&footprint,
			&numRows,
			&rowSizeInBytes,
			&totalBytes
		);

		RHITextureSubresourceData result{};
		result.Data = static_cast<byte_t*>( mappedPtr ) + m_SubresourceOffsets[subresourceIndex];
		result.RowStride = footprint.Footprint.RowPitch;
		result.DepthStride = static_cast<uint32_t>( totalBytes );

		return result;
	}

	void RHITexture_D3D12Impl::UnmapSubresource( const RHITextureSlice& a_Slice )
	{
		RHI_DEV_CHECK( m_Desc.HeapType == ERHIHeapType::Staging, "Only staging textures can be unmapped" );

		Texture.Resource()->Unmap( 0, nullptr );

		m_MappedFootprint = {};
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

		desc.Format = GetDXGIFormatMap( m_Desc.Format ).RTVFormat;
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
		srvDesc.Format = GetDXGIFormatMap( a_Format ).SRVFormat;
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

	D3D12_TEXTURE_COPY_LOCATION RHITexture_D3D12Impl::CreateCopyLocation( const RHITextureSlice& a_Slice ) const
	{
		const auto slice = a_Slice.Resolve( m_Desc );
		D3D12_TEXTURE_COPY_LOCATION result;
		result.pResource = Texture.Resource();
		result.SubresourceIndex = CalcSubresource(
			slice.MipLevel,
			slice.ArraySlice,
			0,
			m_Desc.Mips,
			m_Desc.IsArray() ? m_Desc.DepthOrArraySize : 1
		);

		if ( m_Desc.HeapType == ERHIHeapType::Staging )
		{
			// Staging textures are actually buffers in D3D12, as textures cannot be created on readback heaps.
			result.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			const auto d3d12Desc = GetD3D12ResourceDesc();

			Device()->GetD3D12Device()->GetCopyableFootprints( 
				&d3d12Desc, result.SubresourceIndex, 1,
				m_SubresourceOffsets[result.SubresourceIndex],
				&result.PlacedFootprint, nullptr, nullptr, nullptr
			);
		}
		else
		{
			result.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		}

		return result;
	}

	bool RHITexture_D3D12Impl::CommitAsStagingTexture()
	{
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_READBACK;

		const D3D12_RESOURCE_DESC texDesc = GetD3D12ResourceDesc();
		const UINT numSubresources = m_Desc.Mips * ( m_Desc.IsArray() ? m_Desc.DepthOrArraySize : 1 );

		// Compute total size using D3D12's layout rules
		UINT64 totalSize = 0;
		Device()->GetD3D12Device()->GetCopyableFootprints(
			&texDesc, 0, numSubresources, 0,
			nullptr, nullptr, nullptr, &totalSize
		);

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = totalSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !Texture.Commit( bufferDesc, allocDesc, D3D12_RESOURCE_STATE_COPY_DEST ) )
		{
			ASSERT( false, "Failed to create staging texture buffer" );
			return false;
		}

		D3D12_SET_DEBUG_NAME( Texture.Resource(), m_Desc.Name, L"Unnamed Staging Texture" );

		// Precompute subresource offsets
		m_SubresourceOffsets.Resize( numSubresources );

		UINT64 baseOffset = 0;
		for ( UINT i = 0; i < numSubresources; ++i )
		{
			UINT64 subresourceSize = 0;
			Device()->GetD3D12Device()->GetCopyableFootprints(
				&texDesc, i, 1, 0,
				nullptr, nullptr, nullptr, &subresourceSize
			);

			m_SubresourceOffsets[i] = baseOffset;
			baseOffset = AlignUp<UINT64>( baseOffset + subresourceSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT );
		}

		return true;
	}


} // namespace Tridium::D3D12