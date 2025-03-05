#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12Texture final : public RHITexture
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) override;
		bool IsWritable() const override;
		size_t GetSizeInBytes() const override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<D3D12::Resource> Texture{};
		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle{};
	};


	constexpr DXGI_FORMAT RHITextureFormatToDXGI( ERHITextureFormat a_Format )
	{
		switch ( a_Format )
		{
			using enum ERHITextureFormat;
			case R8:    return DXGI_FORMAT_R8_UNORM;
			case RG8:   return DXGI_FORMAT_R8G8_UNORM;
			case RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;

			case SRGB8:  return DXGI_FORMAT_UNKNOWN; // DXGI doesn't support RGB8 sRGB directly
			case SRGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case R16:     return DXGI_FORMAT_R16_UNORM;
			case RG16:    return DXGI_FORMAT_R16G16_UNORM;
			case RGBA16:  return DXGI_FORMAT_R16G16B16A16_UNORM;
			case R16F:    return DXGI_FORMAT_R16_FLOAT;
			case RG16F:   return DXGI_FORMAT_R16G16_FLOAT;
			case RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case R32F:    return DXGI_FORMAT_R32_FLOAT;
			case RG32F:   return DXGI_FORMAT_R32G32_FLOAT;
			case RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case R16I:    return DXGI_FORMAT_R16_SINT;
			case RG16I:   return DXGI_FORMAT_R16G16_SINT;
			case RGBA16I: return DXGI_FORMAT_R16G16B16A16_SINT;
			case R32I:    return DXGI_FORMAT_R32_SINT;
			case RG32I:   return DXGI_FORMAT_R32G32_SINT;
			case RGBA32I: return DXGI_FORMAT_R32G32B32A32_SINT;

			case D16:   return DXGI_FORMAT_D16_UNORM;
			case D32:   return DXGI_FORMAT_D32_FLOAT;
			case D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;

			case BC1: return DXGI_FORMAT_BC1_UNORM;
			case BC3: return DXGI_FORMAT_BC3_UNORM;
			case BC4: return DXGI_FORMAT_BC4_UNORM;
			case BC5: return DXGI_FORMAT_BC5_UNORM;
			case BC7: return DXGI_FORMAT_BC7_UNORM;

			default: return DXGI_FORMAT_UNKNOWN;
		}
	}


}