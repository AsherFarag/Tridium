#include "tripch.h"
#include "D3D12Texture.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ResourceAllocator.h"

namespace Tridium {

	bool D3D12Texture::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );

		if ( desc->Dimension != ERHITextureDimension::Texture2D )
		{
			return false;
		}

		if ( m_Allocator )
		{
			// We were given a render resource allocator, so use that
			const ERHIResourceAllocatorType allocatorType = m_Allocator->GetDescriptor()->AllocatorType;
			switch ( allocatorType )
			{
				using enum ERHIResourceAllocatorType;
				case RenderResource:
				case RenderTarget:
				{
					auto* allocator = m_Allocator->As<D3D12ResourceAllocator>();
					uint32_t offset = 0;
					if ( !allocator->Allocate( 1, &offset ) )
					{
						LOG( LogCategory::RHI, Error, "Failed to allocate texture - '{0}' in resource allocator - '{1}'", desc->Name, m_Allocator->GetDescriptor()->Name );
						return false;
					}

					DescriptorHandle = allocator->DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
					DescriptorHandle.ptr += offset * allocator->DescriptorSize;

					if ( desc->IsRenderTarget )
					{
						if ( allocatorType == RenderTarget ) [[likely]]
						{
							return true;
						}
						else
						{
							ASSERT_LOG( false, "Texture is a render target but the allocator is not a render target allocator!" );
							return false;
						}
					}

					break;
				}
				default:
				{
					ASSERT_LOG( false, "Invalid allocator type!" );
					return false;
				}
			}
		}
		else
		{
			// We were not given an allocator, so create a default one
			RHIResourceAllocatorDescriptor heapDesc = {};
			heapDesc.AllocatorType = ERHIResourceAllocatorType::RenderResource;
			heapDesc.Capacity = 1;
			heapDesc.Flags = ERHIResourceAllocatorFlags::ShaderVisible;
			m_Allocator = RHI::GetD3D12RHI()->CreateResourceAllocator( heapDesc );
			if ( !m_Allocator )
			{
				return ASSERT_LOG( false, "Failed to create resource allocator!" );
			}

			// We are the only texture in the heap, so we can use the first descriptor
			m_Allocator->Allocate( 1 );
			DescriptorHandle = m_Allocator->As<D3D12ResourceAllocator>()->DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		}

		const size_t width = desc->Width;
		const size_t height = desc->Height;
		const size_t depth = desc->Depth;
		const size_t stride = width * GetTextureFormatSize( desc->Format );
		const size_t imgSize = height * stride;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = ToD3D12::GetResourceDimension( desc->Dimension );
		resourceDesc.Alignment = 0;
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.DepthOrArraySize = depth;
		resourceDesc.MipLevels = desc->Mips;
		resourceDesc.Format = RHITextureFormatToDXGI( desc->Format );
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		const auto& device = RHI::GetD3D12RHI()->GetDevice();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		// Create the texture
		HRESULT hr = device->CreateCommittedResource(
				&heapProperties, D3D12_HEAP_FLAG_NONE,
				&resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr, IID_PPV_ARGS( &Texture ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

#if RHI_DEBUG_ENABLED
		// Set the name
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			WString wName = ToD3D12::ToWString( desc->Name.data() );
			Texture->SetName( wName.c_str() );
			D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
		}
#endif

		// Create the SRV
		const bool isArray = ( desc->Depth > 1 ) && ( desc->Dimension != ERHITextureDimension::Texture3D );
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = ToD3D12::GetSRVDimension( desc->Dimension, isArray );
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc->Mips;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		device->CreateShaderResourceView( Texture.Get(), &srvDesc, DescriptorHandle );

		// If we have initial data, write it to the texture
		if ( desc->InitialData.size() > 0 )
		{
			Write( desc->InitialData );
		}

		return true;
	}

	bool D3D12Texture::Release()
	{
		Texture.Release();
		DescriptorHandle = {};
		if ( m_Allocator )
		{
			m_Allocator->Deallocate( *this );
			m_Allocator = nullptr;
		}
		return true;
	}

	bool D3D12Texture::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
	{
		if ( !IsValid() )
		{
			return false;
		}

		// To write to a texture on the GPU, we need to use an upload buffer
		// To do this, we need to:
		//    1. Create an intermediate upload heap
		//    2. Copy data to upload buffer
		//    3. Setup copy locations
		//    4. Copy texture data
		//    5. Execute commands

		TODO( "Writing to a GPU-only texture requires the cmd list. Figure out if theres some cleaner, faster or safer way to create resources." );

		auto* rhi = RHI::GetD3D12RHI();
		auto& device = rhi->GetDevice();
		auto& commandList = rhi->GetCommandList();
		auto& commandAllocator = rhi->GetCommandAllocator();

		// Reset the command allocator
		commandAllocator->Reset();
		commandList->Reset( commandAllocator.Get(), nullptr );

		const RHITextureDescriptor& desc = *GetDescriptor();
		const size_t width = desc.Width;
		const size_t height = desc.Height;
		const size_t bytesPerPixel = GetTextureFormatSize( desc.Format );

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		uint64_t uploadSize = 0;
		auto texDesc = Texture->GetDesc();
		device->GetCopyableFootprints( &texDesc, 0, 1, 0, &footprint, nullptr, nullptr, &uploadSize );

		D3D12_HEAP_PROPERTIES uploadHeapProps = {};
		uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProps.CreationNodeMask = 1;
		uploadHeapProps.VisibleNodeMask = 1;

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


		ComPtr<ID3D12Resource> uploadBuffer;
		HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProps, D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS( &uploadBuffer ) );

		if ( FAILED( hr ) )
			return false;

		char* uploadBufferAddress;
		D3D12_RANGE uploadRange;
		uploadRange.Begin = 0;
		uploadRange.End = a_Data.size();
		uploadBuffer->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		memcpy( &uploadBufferAddress[0], a_Data.data(), a_Data.size() );
		uploadBuffer->Unmap( 0, &uploadRange );

		D3D12_TEXTURE_COPY_LOCATION dstLocation = { Texture.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint = footprint;

		D3D12_BOX textureBox{};
		textureBox.left = 0;
		textureBox.top = 0;
		textureBox.front = 0;
		textureBox.right = desc.Width;
		textureBox.bottom = desc.Height;
		textureBox.back = 1;
		commandList->CopyTextureRegion( &dstLocation, 0, 0, 0, &srcLocation, &textureBox );

		hr = commandList->Close();
		if ( FAILED( hr ) )
			return false;

		auto& commandQueue = RHI::GetD3D12RHI()->GetCommandQueue();
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists( 1, commandLists );

		// Signal and wait for completion
		rhi->FenceSignal( {} );

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

		D3D12_RESOURCE_DESC desc = Texture->GetDesc();
		const uint32_t bytesPerPixel = GetTextureFormatSize( GetDescriptor()->Format );
		return GetDescriptor()->Width * GetDescriptor()->Height * bytesPerPixel;
	}

	bool D3D12Texture::IsValid() const
	{
		return Texture != nullptr
			&& Descriptor != nullptr;
	}

	const void* D3D12Texture::NativePtr() const
	{
		return Texture.Get();
	}

} // namespace Tridium::D3D12