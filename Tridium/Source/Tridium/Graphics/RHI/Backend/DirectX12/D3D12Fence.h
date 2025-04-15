#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12Fence, RHIFence )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12Fence, ERHInterfaceType::DirectX12 );
		virtual ~D3D12Fence();

		bool Commit( const RHIFenceDescriptor& a_Desc ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		uint64_t GetCompletedValue() override;
		void Signal( uint64_t a_Value ) override;
		void Wait( uint64_t a_Value ) override;

	private:
		ComPtr<ID3D12Fence> m_Fence = nullptr;
		HANDLE m_FenceEvent = nullptr;
	};
}