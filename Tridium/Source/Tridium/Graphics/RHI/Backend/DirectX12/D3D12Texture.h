#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHITexture_D3D12Impl, RHITexture )
	{
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHITexture_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHITexture_D3D12Impl( const RHITextureDescriptor & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		~RHITexture_D3D12Impl() override = default;

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
			ID3D12GraphicsCommandList& a_CmdList, RHITexture_D3D12Impl& a_SrcTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion
		);

		D3D12::ManagedResource Texture{};
	};

}