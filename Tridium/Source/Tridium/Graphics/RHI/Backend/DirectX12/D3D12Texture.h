#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12Texture, RHITexture )
	{
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12Texture, ERHInterfaceType::DirectX12 );

		D3D12Texture( const RHITextureDescriptor & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		~D3D12Texture() override = default;

		virtual bool Release() override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return Texture.Resource.Get(); }
		virtual bool IsValid() const override { return Texture.Valid(); }

		// D3D12 specific functions
		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

		D3D12::ManagedResource Texture{};

	private:
		uint64_t m_CopyFenceValue = 0;
	};

}