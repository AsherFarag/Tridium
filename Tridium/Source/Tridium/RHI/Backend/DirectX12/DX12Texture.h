#pragma once
#include <Tridium/RHI/RHITexture.h>
#include "DirectX12.h"

namespace Tridium {

	class DX12Texture final : public RHITexture
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

	private:
		ComPtr<ID3D12Resource2> m_Texture;
	};


	constexpr DXGI_FORMAT RHITextureFormatToDXGI( ERHITextureFormat a_Format )
	{
		switch ( a_Format )
		{
			case ERHITextureFormat::R8: return DXGI_FORMAT_R8_UNORM;
			case ERHITextureFormat::RG8: return DXGI_FORMAT_R8G8_UNORM;
			case ERHITextureFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;

			case ERHITextureFormat::SRGB8: return DXGI_FORMAT_UNKNOWN; // DXGI doesn't support RGB8 sRGB directly
			case ERHITextureFormat::SRGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case ERHITextureFormat::R16: return DXGI_FORMAT_R16_UNORM;
			case ERHITextureFormat::RG16: return DXGI_FORMAT_R16G16_UNORM;
			case ERHITextureFormat::RGBA16: return DXGI_FORMAT_R16G16B16A16_UNORM;
			case ERHITextureFormat::R16F: return DXGI_FORMAT_R16_FLOAT;
			case ERHITextureFormat::RG16F: return DXGI_FORMAT_R16G16_FLOAT;
			case ERHITextureFormat::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;

			case ERHITextureFormat::R32F: return DXGI_FORMAT_R32_FLOAT;
			case ERHITextureFormat::RG32F: return DXGI_FORMAT_R32G32_FLOAT;
			case ERHITextureFormat::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case ERHITextureFormat::R16I: return DXGI_FORMAT_R16_SINT;
			case ERHITextureFormat::RG16I: return DXGI_FORMAT_R16G16_SINT;
			case ERHITextureFormat::RGBA16I: return DXGI_FORMAT_R16G16B16A16_SINT;
			case ERHITextureFormat::R32I: return DXGI_FORMAT_R32_SINT;
			case ERHITextureFormat::RG32I: return DXGI_FORMAT_R32G32_SINT;
			case ERHITextureFormat::RGBA32I: return DXGI_FORMAT_R32G32B32A32_SINT;

			case ERHITextureFormat::D16: return DXGI_FORMAT_D16_UNORM;
			case ERHITextureFormat::D32: return DXGI_FORMAT_D32_FLOAT;
			case ERHITextureFormat::D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;

			case ERHITextureFormat::BC1: return DXGI_FORMAT_BC1_UNORM;
			case ERHITextureFormat::BC3: return DXGI_FORMAT_BC3_UNORM;
			case ERHITextureFormat::BC4: return DXGI_FORMAT_BC4_UNORM;
			case ERHITextureFormat::BC5: return DXGI_FORMAT_BC5_UNORM;
			case ERHITextureFormat::BC7: return DXGI_FORMAT_BC7_UNORM;

			default: return DXGI_FORMAT_UNKNOWN;
		}
	}


}