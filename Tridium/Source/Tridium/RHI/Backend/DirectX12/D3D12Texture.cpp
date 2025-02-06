#include "tripch.h"
#include "D3D12Texture.h"
#include "D3D12RHI.h"

namespace Tridium {

	bool D3D12Texture::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );

		if ( desc->DimensionCount != 2 )
		{
			return false;
		}

		const size_t width = desc->Dimensions[0];
		const size_t height = desc->Dimensions[1];
		const size_t stride = width * GetTextureFormatSize( desc->Format );
		const size_t imgSize = height * stride;

		// Create Dx12 Texture descriptor
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = desc->Mips;
		resourceDesc.Format = RHITextureFormatToDXGI( desc->Format );
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// Create the heap properties
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		const auto& device = RHI::GetDynamicRHI<DirectX12RHI>()->GetDevice();

		HRESULT hr;

		// Create the texture
		hr = device->CreateCommittedResource(
				&heapProperties, D3D12_HEAP_FLAG_NONE,
				&resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, // Initialize to COPY_DEST for safety
				nullptr, IID_PPV_ARGS( &m_Texture ) );

#if RHI_DEBUG_ENABLED
		if ( RHIQuery::IsDebug() && !desc->Name.empty() )
		{
			WString wName = Helpers::ToWString( desc->Name.data() );
			m_Texture->SetName( wName.c_str() );
			D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) );
		}
#endif

		if ( FAILED( hr ) )
		{
			return false;
		}

		// Create the descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &m_DescriptorHeap ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		// Create the SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc->Mips;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		auto handle( m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart() );
		device->CreateShaderResourceView( m_Texture.Get(), &srvDesc, handle );

		// If we have initial data, write it to the texture
		if ( desc->InitialData.size() > 0 )
		{
			Write( desc->InitialData );
		}

		return true;
	}

	bool D3D12Texture::Release()
	{
		m_Texture.Release();
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
		//    3. Transition to COPY_DEST
		//    4. Setup copy locations
		//    5. Copy texture data
		//    6. Transition to SHADER_RESOURCE
		//    7. Execute commands

		auto* rhi = RHI::GetDynamicRHI<DirectX12RHI>();
		auto& device = rhi->GetDevice();
		auto& commandList = rhi->GetCommandList();
		auto& commandAllocator = rhi->GetCommandAllocator();

		// Reset the command allocator
		commandAllocator->Reset();
		commandList->Reset( commandAllocator.Get(), nullptr );

		const RHITextureDescriptor& desc = *GetDescriptor();
		const size_t width = desc.Dimensions[0];
		const size_t height = desc.Dimensions[1];
		const size_t bytesPerPixel = GetTextureFormatSize( desc.Format );

		// Compute proper footprint
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		uint64_t uploadSize = 0;
		auto texDesc = m_Texture->GetDesc();
		device->GetCopyableFootprints( &texDesc, 0, 1, 0, &footprint, nullptr, nullptr, &uploadSize );

		// 1. Create an intermediate upload heap
		D3D12_HEAP_PROPERTIES uploadHeapProps = {};
		uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProps.CreationNodeMask = 1;
		uploadHeapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = a_Data.size();  // Make sure this isn't zero
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0; // Explicitly set to 0
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


		ComPtr<ID3D12Resource> uploadBuffer;
		HRESULT hr = device->CreateCommittedResource(
			&uploadHeapProps, D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS( &uploadBuffer ) );

		if ( FAILED( hr ) )
			return false;

		// 2. Copy data to upload buffer
		char* uploadBufferAddress;
		D3D12_RANGE uploadRange;
		uploadRange.Begin = 0;
		uploadRange.End = a_Data.size();
		uploadBuffer->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		memcpy( &uploadBufferAddress[0], a_Data.data(), a_Data.size() );
		uploadBuffer->Unmap( 0, &uploadRange );

		// 3. Transition to COPY_DEST
		//D3D12_RESOURCE_BARRIER barrier = {};
		//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//barrier.Transition.pResource = m_Texture.Get();
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		//commandList->ResourceBarrier( 1, &barrier );

		// 4. Setup copy locations
		D3D12_TEXTURE_COPY_LOCATION dstLocation = { m_Texture.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint = footprint;

		// 5. Copy texture data
		D3D12_BOX textureBox{};
		textureBox.left = 0;
		textureBox.top = 0;
		textureBox.front = 0;
		textureBox.right = desc.Dimensions[0];
		textureBox.bottom = desc.Dimensions[1];
		textureBox.back = 1;
		commandList->CopyTextureRegion( &dstLocation, 0, 0, 0, &srcLocation, &textureBox );

		// 6. Transition to SHADER_RESOURCE
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		//commandList->ResourceBarrier( 1, &barrier );

		// 7. Execute commands
		hr = commandList->Close();
		if ( FAILED( hr ) )
			return false;

		auto& commandQueue = RHI::GetDynamicRHI<DirectX12RHI>()->GetCommandQueue();
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

		D3D12_RESOURCE_DESC desc = m_Texture->GetDesc();
		const uint32_t bytesPerPixel = GetTextureFormatSize( GetDescriptor()->Format );
		return GetDescriptor()->Dimensions[0] * GetDescriptor()->Dimensions[1] * bytesPerPixel;
	}

	bool D3D12Texture::IsValid() const
	{
		return m_Texture != nullptr
			&& Descriptor != nullptr;
	}

	const void* D3D12Texture::NativePtr() const
	{
		return m_Texture.Get();
	}

} // namespace Tridium::D3D12