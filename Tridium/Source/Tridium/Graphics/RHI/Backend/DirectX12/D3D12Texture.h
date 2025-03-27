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
		bool Resize( uint32_t a_Width, uint32_t a_Height, uint32_t a_Depth = 1 ) override;

		D3D12::ManagedResource Texture{};
	};

}