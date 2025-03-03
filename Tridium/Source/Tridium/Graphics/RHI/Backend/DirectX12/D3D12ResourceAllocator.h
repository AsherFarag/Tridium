#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12ResourceAllocator : public RHIResourceAllocator
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		// Inherited via RHIResourceAllocator
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<D3D12::DescriptorHeap> DescriptorHeap{};
		uint32_t DescriptorSize = 0; // Size of each descriptor in the heap
	};

} // namespace Tridium