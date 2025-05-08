#include "tripch.h"
#include "D3D12Texture.h"
#include "D3D12DynamicRHI.h"
#include "../../RHI.h"

namespace Tridium {

	D3D12Texture::D3D12Texture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
		: RHITexture( a_Desc )
	{
		const bool initData = a_SubResourcesData.size() > 0;
		ERHIResourceStates initialState = initData ? ERHIResourceStates::CopyDest : ERHIResourceStates::Common;
		SetState( initialState );

		const RHIFormatInfo& formatInfo = GetRHIFormatInfo( a_Desc.Format );
		const bool isDepthTarget = formatInfo.HasDepth || formatInfo.HasStencil;

		const size_t width = a_Desc.Width;
		const size_t height = a_Desc.Height;
		const size_t depth = a_Desc.Depth;
		const size_t stride = width * formatInfo.BytesPerBlock;
		const size_t imgSize = height * stride;

		D3D12_RESOURCE_DESC d3d12Desc = GetD3D12ResourceDesc();

		// Configure the clear value
		D3D12_CLEAR_VALUE clearValue = {};
		D3D12_CLEAR_VALUE* clearValuePtr = nullptr;
		if ( a_Desc.ClearValue.has_value() )
		{
			clearValuePtr = &clearValue;
			clearValue.Format = D3D12::Translate( a_Desc.Format );
			if ( isDepthTarget )
			{
				clearValue.DepthStencil.Depth = a_Desc.ClearValue->Depth;
				clearValue.DepthStencil.Stencil = a_Desc.ClearValue->Stencil;
			}
			else
			{
				clearValue.Color[0] = a_Desc.ClearValue->Color[0];
				clearValue.Color[1] = a_Desc.ClearValue->Color[1];
				clearValue.Color[2] = a_Desc.ClearValue->Color[2];
				clearValue.Color[3] = a_Desc.ClearValue->Color[3];
			}
		}

		// Create the texture
		if ( !Texture.Commit( d3d12Desc, D3D12::Translate( initialState ), clearValuePtr ) )
		{
			ASSERT( false, "Failed to create D3D12 texture" );
			return;
		}

		D3D12_SET_DEBUG_NAME( Texture.Resource, a_Desc.Name );

		if ( initData )
		{
			D3D12CommandContext& copyCmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Copy );

			UINT64 uploadBufferSize = 0;
			UINT numSubresources = static_cast<UINT>( a_SubResourcesData.size() );

			Array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts;
			Array<UINT> numRows;
			Array<UINT64> rowSizeInBytes;

			layouts.Resize( numSubresources );
			numRows.Resize( numSubresources );
			rowSizeInBytes.Resize( numSubresources );

			GetD3D12RHI()->GetD3D12Device()->GetCopyableFootprints(
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
				d3d12SubResData[i].pData = a_SubResourcesData[i].Data.data();
				d3d12SubResData[i].RowPitch = static_cast<LONG_PTR>( a_SubResourcesData[i].RowStride );
				d3d12SubResData[i].SlicePitch = static_cast<LONG_PTR>( a_SubResourcesData[i].DepthStride );
			}

			// Ensure the copy command list is reset
			TODO( "Should we be doing this, this way?" );
			if ( FAILED( copyCmdCtx.CmdAllocator->Reset() ) )
			{
				ASSERT( false, "Failed to reset command allocator" );
				return;
			}
			if ( FAILED( copyCmdCtx.CmdList->Reset( copyCmdCtx.CmdAllocator.Get(), nullptr ) ) )
			{
				ASSERT( false, "Failed to reset command list" );
				return;
			}

			UpdateSubresources(
				copyCmdCtx.CmdList.Get(),
				Texture.Resource.Get(),
				uploadBuffer.Resource.Get(),
				0, 0,
				numSubresources,
				d3d12SubResData.Data()
			);

			// Execute the copy command list
			if ( FAILED( copyCmdCtx.CmdList->Close() ) )
			{
				ASSERT( false, "Failed to close command list" );
				return;
			}

			ID3D12CommandList* cmdLists[] = { copyCmdCtx.CmdList.Get() };
			copyCmdCtx.CmdQueue->ExecuteCommandLists( 1, cmdLists );
			copyCmdCtx.Wait( copyCmdCtx.Signal() );
		}
	}

	bool D3D12Texture::Release()
	{
		Texture.Release();
		return true;
	}

	size_t D3D12Texture::GetSizeInBytes() const
	{
		if ( !IsValid() )
			return 0;

		D3D12_RESOURCE_DESC desc = Texture.Resource->GetDesc();
		const uint32_t bytesPerPixel = GetRHIFormatInfo( Descriptor().Format ).BytesPerBlock;
		return Descriptor().Width * Descriptor().Height * bytesPerPixel;
	}

	D3D12_RESOURCE_DESC D3D12Texture::GetD3D12ResourceDesc() const
	{
		D3D12_RESOURCE_DESC desc{};

		desc.Alignment = 0;
		desc.Width = UINT{ m_Desc.Width };
		desc.Height = UINT{ m_Desc.Height };
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = static_cast<UINT16>( m_Desc.Mips );
		desc.SampleDesc.Count = UINT{ m_Desc.Samples };
		desc.SampleDesc.Quality = 0;

		if ( m_Desc.IsArray() || m_Desc.Is3D() )
			desc.DepthOrArraySize = m_Desc.Depth;
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


#if 0

	bool D3D12Texture::Write( const Span<const Byte>& a_Data )
	{
		if ( !IsValid() ) return false;

		auto* rhi = GetD3D12RHI();
		const auto& device = rhi->GetDevice();
		const auto& copyCmdAllocator = rhi->GetCopyCmdAllocator();
		const auto& copyCmdList = rhi->GetCopyCmdList();
		const auto& copyQueue = rhi->GetCopyQueue();

		// Reset the dedicated copy allocator and command list
		copyCmdAllocator->Reset();
		copyCmdList->Reset( copyCmdAllocator.Get(), nullptr );

		const size_t width = Descriptor().Width;
		const size_t height = Descriptor().Height;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		uint64_t uploadSize = 0;
		auto texDesc = Texture.Resource->GetDesc();
		device->GetCopyableFootprints( &texDesc, 0, 1, 0, &footprint, nullptr, nullptr, &uploadSize );

		TODO( "Reusing upload buffers" );
		// Create an upload buffer
		thread_local D3D12::ManagedResource uploadBuffer{};
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = a_Data.size();
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			return false;

		// Copy data to upload buffer
		char* uploadBufferAddress;
		D3D12_RANGE uploadRange = { 0, uploadSize };
		uploadBuffer.Resource->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		memcpy( uploadBufferAddress, a_Data.data(), a_Data.size() );
		uploadBuffer.Resource->Unmap( 0, &uploadRange );

		// Setup copy locations
		D3D12_TEXTURE_COPY_LOCATION dstLocation = { Texture.Resource.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
		D3D12_TEXTURE_COPY_LOCATION srcLocation = { uploadBuffer.Resource.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, footprint };

		// Copy texture data
		copyCmdList->CopyTextureRegion( &dstLocation, 0, 0, 0, &srcLocation, nullptr );

		// Transition the texture to a shader resource
		//D3D12_RESOURCE_BARRIER barrier = {};
		//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//barrier.Transition.pResource = Texture.Resource.Get();
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		//copyCmdList->ResourceBarrier( 1, &barrier );

		// Close and execute copy commands
		if ( FAILED( copyCmdList->Close() ) ) return false;

		ID3D12CommandList* cmdLists[] = { copyCmdList.Get() };
		copyQueue->ExecuteCommandLists( 1, cmdLists );

		// Signal a fence that can be waited on ( outside of this function )
		m_CopyFenceValue = rhi->IncrementCopyFence();
		if ( FAILED( copyQueue->Signal( rhi->GetCopyFence()->NativePtrAs<ID3D12Fence>(), m_CopyFenceValue ) ) ) return false;

		return true;
	}

	bool D3D12Texture::IsWritable() const
	{
		if ( !IsValid() )
		{
			return false;
		}

		return true;
	}

	bool D3D12Texture::Resize( uint32_t a_Width, uint32_t a_Height, uint32_t a_Depth )
	{
		if ( !IsValid() )
		{
			return false;
		}

		if ( a_Width == m_Desc.Width && a_Height == m_Desc.Height && a_Depth == m_Desc.Depth )
		{
			return true;
		}

		m_Desc.Width = a_Width;
		m_Desc.Height = a_Height;
		m_Desc.Depth = a_Depth;

		Release();

		return Commit( m_Desc );
	}

	bool D3D12Texture::IsReady() const
	{
		if ( !IsValid() 
			|| GetD3D12RHI()->GetCopyFence()->GetCompletedValue() < m_CopyFenceValue )
		{
			return false;
		}

		return true;
	}

	void D3D12Texture::Wait()
	{
		if ( !IsValid() )
		{
			return;
		}

		auto* rhi = GetD3D12RHI();
		const auto& copyFence = rhi->GetCopyFence();
		copyFence->Wait( m_CopyFenceValue );
	}

#endif

	bool D3D12Texture::CopyTexture(
		ID3D12GraphicsCommandList& a_CmdList, D3D12Texture& a_SrcTexture,
		uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
		uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion )
	{
		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = this->Texture.Resource.Get();
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = D3D12CalcSubresource(
			a_DstRegion.MinZ, a_DstRegion.MinY, a_DstRegion.MinX,
			m_Desc.Mips, m_Desc.Depth
		);

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = a_SrcTexture.Texture.Resource.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex = D3D12CalcSubresource(
			a_SrcRegion.MinZ, a_SrcRegion.MinY, a_SrcRegion.MinX,
			a_SrcTexture.Descriptor().Mips, a_SrcTexture.Descriptor().Depth 
		);

		// Valid if the regions are valid and can fit in their respective textures
		if ( a_SrcRegion.MinX < 0 || a_SrcRegion.MinY < 0 || a_SrcRegion.MinZ < 0 ||
			a_SrcRegion.MaxX > a_SrcTexture.Descriptor().Width ||
			a_SrcRegion.MaxY > a_SrcTexture.Descriptor().Height ||
			a_SrcRegion.MaxZ > a_SrcTexture.Descriptor().Depth )
		{
			RHI_DEV_CHECK( false, "Source region is invalid!" );
			return false;
		}

		if ( a_DstRegion.MinX < 0 || a_DstRegion.MinY < 0 || a_DstRegion.MinZ < 0 ||
			a_DstRegion.MaxX > m_Desc.Width ||
			a_DstRegion.MaxY > m_Desc.Height ||
			a_DstRegion.MaxZ > m_Desc.Depth )
		{
			RHI_DEV_CHECK( false, "Destination region is invalid!" );
			return false;
		}

		a_CmdList.CopyTextureRegion(
			&dstLocation,
			a_DstRegion.MinX,
			a_DstRegion.MinY,
			a_DstRegion.MinZ,
			&srcLocation,
			nullptr // use full source footprint
		);

		return true;
	}

} // namespace Tridium::D3D12