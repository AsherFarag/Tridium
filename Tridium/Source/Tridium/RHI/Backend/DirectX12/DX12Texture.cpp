#include "tripch.h"
#include "DX12Texture.h"
#include "DX12RHI.h"

namespace Tridium {

	bool DX12Texture::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHITextureDescriptor>( a_Params );

		if ( desc->DimensionCount != 2 )
		{
			return false;
		}

		size_t width = desc->Dimensions[0];
		size_t height = desc->Dimensions[1];

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

		// Create the texture
		bool success = SUCCEEDED( device->CreateCommittedResource(
			&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS( &m_Texture )
		) );

		if ( !success )
		{
			return false;
		}

		// Create the descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		success = SUCCEEDED( device->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &descriptorHeap ) ) );
		if ( !success )
		{
			return false;
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc->Mips;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		auto handle( descriptorHeap->GetCPUDescriptorHandleForHeapStart() );
		device->CreateShaderResourceView( m_Texture.Get(), &srvDesc, handle );

		return success;
	}

	bool DX12Texture::Release()
	{
		return false;
	}

	bool DX12Texture::Write( const Span<const Byte>& a_Data, size_t a_DstOffset )
	{
		const D3D12_RANGE uploadRange = { 0, a_Data.size() };
		return false;
	}

	bool DX12Texture::IsWritable() const
	{
		return false;
	}

	size_t DX12Texture::GetSizeInBytes() const
	{
		return 0;
	}

	bool DX12Texture::IsValid() const
	{
		return false;
	}

	const void* DX12Texture::NativePtr() const
	{
		return nullptr;
	}

} // namespace Tridium::DX12