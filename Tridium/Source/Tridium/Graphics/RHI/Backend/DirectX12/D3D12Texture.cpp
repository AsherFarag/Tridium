#include "tripch.h"
#include "D3D12Texture.h"
#include "D3D12DynamicRHI.h"
#include "../../RHI.h"

namespace Tridium {

	bool D3D12Texture::Commit( const RHITextureDescriptor& a_Desc )
	{
		m_Descriptor = a_Desc;
		if ( a_Desc.Dimension != ERHITextureDimension::Texture2D )
		{
			return false;
		}
		const RHIFormatInfo& formatInfo = GetRHIFormatInfo( a_Desc.Format );
		const bool isDepthTarget = formatInfo.HasDepth || formatInfo.HasStencil;

		const auto& device = GetD3D12RHI()->GetDevice();

		const size_t width = a_Desc.Width;
		const size_t height = a_Desc.Height;
		const size_t depth = a_Desc.Depth;
		const size_t stride = width * formatInfo.BytesPerBlock;
		const size_t imgSize = height * stride;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12::To<D3D12_RESOURCE_DIMENSION>::From( a_Desc.Dimension );
		resourceDesc.Alignment = 0;
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.DepthOrArraySize = depth;
		resourceDesc.MipLevels = a_Desc.Mips;
		resourceDesc.Format = D3D12::Translate( a_Desc.Format );
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = isDepthTarget ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAG_NONE;

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
		if ( !Texture.Commit( resourceDesc, D3D12_RESOURCE_STATE_COMMON, clearValuePtr ) )
		{
			return false;
		}

		D3D12_SET_DEBUG_NAME( Texture.Resource, a_Desc.Name );

		return true;
	}

	bool D3D12Texture::Release()
	{
		Texture.Release();
		return true;
	}

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
		copyCmdList->Reset( copyCmdAllocator.Get(), nullptr);

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
		if ( FAILED( copyQueue->Signal( rhi->GetCopyFence()->NativePtrAs<ID3D12Fence>(), m_CopyFenceValue)) ) return false;

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

	size_t D3D12Texture::GetSizeInBytes() const
	{
		if ( !IsValid() )
			return 0;

		D3D12_RESOURCE_DESC desc = Texture.Resource->GetDesc();
		const uint32_t bytesPerPixel = GetRHIFormatInfo( Descriptor().Format ).BytesPerBlock;
		return Descriptor().Width * Descriptor().Height * bytesPerPixel;
	}

	bool D3D12Texture::Resize( uint32_t a_Width, uint32_t a_Height, uint32_t a_Depth )
	{
		if ( !IsValid() )
		{
			return false;
		}

		if ( a_Width == m_Descriptor.Width && a_Height == m_Descriptor.Height && a_Depth == m_Descriptor.Depth )
		{
			return true;
		}

		m_Descriptor.Width = a_Width;
		m_Descriptor.Height = a_Height;
		m_Descriptor.Depth = a_Depth;

		Release();

		return Commit( m_Descriptor );
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

} // namespace Tridium::D3D12