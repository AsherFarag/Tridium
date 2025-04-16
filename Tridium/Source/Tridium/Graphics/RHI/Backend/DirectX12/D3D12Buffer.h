#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12Buffer, RHIBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12Buffer, ERHInterfaceType::DirectX12 );

		D3D12Buffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data = {} );
		virtual bool Commit( const RHIBufferDescriptor & a_Desc ) override
		{
			TODO( "Implement D3D12Buffer::Commit" );
			m_Desc = a_Desc;
			return true;
		}
		virtual bool Release() override { ManagedBuffer.Release(); return true; }
		virtual bool IsValid() const override { return ManagedBuffer.Valid(); }
		virtual size_t GetSizeInBytes() const override { return ManagedBuffer.Allocation->GetSize(); }
		virtual const void* NativePtr() const override { return ManagedBuffer.Resource.Get(); }

		// D3D12 specific functions
		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

		D3D12::ManagedResource ManagedBuffer{};
	};

}