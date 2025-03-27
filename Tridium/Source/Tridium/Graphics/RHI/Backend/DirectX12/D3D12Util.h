#pragma once
#include <mutex>
#include "Utils/D3D12RootSig.h"
#include "Utils/ThirdParty/D3D12MemAlloc.h"

namespace Tridium {

	namespace D3D12 {

		// Common Heap Properties
		constexpr struct {

			const D3D12_HEAP_PROPERTIES Default = {
				.Type                 = D3D12_HEAP_TYPE_DEFAULT,
				.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask     = 0u, // 0 means all nodes
				.VisibleNodeMask      = 0u  // 0 means all nodes
			};

			const D3D12_HEAP_PROPERTIES Upload = {
				.Type                 = D3D12_HEAP_TYPE_UPLOAD,
				.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask     = 0u, // 0 means all nodes
				.VisibleNodeMask      = 0u  // 0 means all nodes
			};

			const D3D12_HEAP_PROPERTIES Readback = {
				.Type                 = D3D12_HEAP_TYPE_READBACK,
				.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask     = 0u, // 0 means all nodes
				.VisibleNodeMask      = 0u  // 0 means all nodes
			};

		} HeapProperties;	

		struct ManagedResource
		{
			ComPtr<D3D12MA::Allocation> Allocation{};
			ComPtr<ID3D12Resource> Resource{};

			bool Valid() const { return Allocation && Resource; }

			void Release()
			{
				Allocation.Release();
				Resource.Release();
			}

			bool Commit( const D3D12_RESOURCE_DESC& a_ResourceDesc,
				D3D12_RESOURCE_STATES a_InitialState,
				const D3D12_CLEAR_VALUE* a_ClearValue = nullptr,
				D3D12MA::Allocator* a_Allocator = nullptr )
			{
				D3D12MA::ALLOCATION_DESC allocDesc{};
				allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				return Commit( a_ResourceDesc, allocDesc, a_InitialState, a_ClearValue, a_Allocator );
			}

			bool Commit( const D3D12_RESOURCE_DESC& a_ResourceDesc,
				const D3D12MA::ALLOCATION_DESC& a_AllocDesc,
				D3D12_RESOURCE_STATES a_InitialState,
				const D3D12_CLEAR_VALUE* a_ClearValue = nullptr,
				D3D12MA::Allocator* a_Allocator = nullptr );
		};

	} // namespace D3D12

} // namespace Tridium