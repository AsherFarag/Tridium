#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12Texture, RHITexture )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12Texture, ERHInterfaceType::DirectX12 );

		virtual bool Commit( const RHITextureDescriptor & a_Desc ) override;
		virtual bool Release() override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return Texture.Resource.Get(); }
		virtual bool IsValid() const override { return Texture.Valid(); }
		virtual bool Read( Array<Byte>& o_Data, size_t a_SrcOffset = 0 ) { return false; }
		virtual bool IsReadable() const { return false; }
		virtual bool Write( const Span<const Byte>& a_Data );
		virtual bool IsWritable() const;
		virtual bool IsReady() const;
		virtual void Wait();
		virtual bool Resize( uint32_t a_Width, uint32_t a_Height, uint32_t a_Depth = 1 );

		D3D12::ManagedResource Texture{};

	private:
		uint64_t m_CopyFenceValue = 0;
	};

}