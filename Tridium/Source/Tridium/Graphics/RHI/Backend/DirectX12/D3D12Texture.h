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
		[[nodiscard]] D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

		//bool Write(
		//	ID3D12GraphicsCommandList& a_CmdList, 
		//	uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region, const RHITextureSubresourceData& a_Data );

		bool CopyTexture( 
			ID3D12GraphicsCommandList& a_CmdList, D3D12Texture& a_SrcTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion
		);

		D3D12::ManagedResource Texture{};
	};

}