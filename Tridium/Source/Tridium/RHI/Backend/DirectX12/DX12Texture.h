#pragma once
#include <Tridium/RHI/RHITexture.h>
#include "DirectX12.h"

namespace Tridium::DX12 {

	constexpr DXGI_FORMAT RHITextureFormatToDXGI( ERHITextureFormat format );

	class DX12Texture final : public RHITexture
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override
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

			// Create the texture
			D3D12_HEAP_PROPERTIES heapProperties = {};
			heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CreationNodeMask = 1;
			heapProperties.VisibleNodeMask = 1;

			return false;
		}

		bool Release() override
		{
			return false;
		}

		bool IsWritable() const override
		{
			return false;
		}

		size_t GetSizeInBytes() const override
		{
			return size_t();
		}

		bool IsValid() const override
		{
			return false;
		}

		const void* NativePtr() const override
		{
			return nullptr;
		}
	};

}