#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIFence_D3D12Impl, RHIFence )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIFence_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIFence_D3D12Impl( const DescriptorType& a_Desc );
		virtual ~RHIFence_D3D12Impl();

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