#pragma once
#include "D3D12Common.h"
#include "D3D12ToRHI.h"
#include "Utils/D3D12DeviceChild.h"
#include <mutex>

namespace Tridium { 
	class D3D12RHI; 

	//=====================================================================
	// Descriptor Heap Flags
	//  Bitmask flags describing the properties of a descriptor heap.
	enum class ED3D12DescriptorHeapFlags : uint8_t
	{
		None = 0,
		// The heap is visible to the GPU.
		GPUVisible = 1 << 0,
		// The heap is poolable.
		Poolable = 1 << 1,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ED3D12DescriptorHeapFlags );
}

namespace Tridium::D3D12 {

	// Forward declarations
	class DescriptorHeap;
	class DescriptorManager;
	class DescriptorHeapManager;
	// ====================

	inline D3D12_DESCRIPTOR_HEAP_FLAGS Translate( ED3D12DescriptorHeapFlags a_Flags )
	{
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if ( EnumFlags( a_Flags ).HasFlag( ED3D12DescriptorHeapFlags::GPUVisible ) )
		{
			flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}
		return flags;
	}



	//=====================================================================
	// Descriptor Heap
	//  Wrapper around a D3D12 descriptor heap.
	class DescriptorHeap : public DeviceChild
	{
	public:
		DescriptorHeap() = delete;
		// Create a new descriptor heap with its own D3D12 descriptor heap.
		DescriptorHeap( ID3D12Device* a_Device, ComPtr<ID3D12DescriptorHeap>&& a_Heap,
			uint32_t a_NumDescriptors, ERHIDescriptorHeapType a_Type,
			ED3D12DescriptorHeapFlags a_Flags, bool a_IsGlobal = false );
		// Create a new descriptor heap that is a suballocation of a larger heap.
		DescriptorHeap( DescriptorHeap& a_ParentHeap, uint32_t a_Offset, uint32_t a_NumDescriptors );
		~DescriptorHeap();

		ID3D12DescriptorHeap* Heap() const { return m_Heap.Get(); }
		ERHIDescriptorHeapType Type() const { return m_Type; }
		ED3D12DescriptorHeapFlags Flags() const { return m_Flags; }

		uint32_t Offset() const { return m_Offet; }
		uint32_t NumDescriptors() const { return m_NumDescriptors; }
		uint32_t DescriptorSize() const { return m_DescriptorSize; }
		bool IsGlobal() const { return m_IsGlobal; }
		bool IsSuballocation() const { return m_IsSuballocation; }
		bool IsPoolable() const { return m_Flags.HasFlag( ED3D12DescriptorHeapFlags::Poolable ); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle( uint32_t a_Index ) const { return { m_CPUBase.ptr + a_Index * m_DescriptorSize }; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle( uint32_t a_Index ) const { return { m_GPUBase.ptr + a_Index * m_DescriptorSize }; }

		// Get the total size of the heap in bytes.
		size_t SizeInBytes() const { return m_NumDescriptors * m_DescriptorSize; }

	private:
		ComPtr<ID3D12DescriptorHeap> m_Heap;
		const ERHIDescriptorHeapType m_Type;
		const EnumFlags<ED3D12DescriptorHeapFlags> m_Flags;

		const D3D12_CPU_DESCRIPTOR_HANDLE m_CPUBase;
		const D3D12_GPU_DESCRIPTOR_HANDLE m_GPUBase;

		// True this heap is a global heap.
		const bool m_IsGlobal = false;

		// True if this heap is a suballocation of a larger heap.
		const bool m_IsSuballocation = false;
		// The offset of the first descriptor in the heap. Only used when the heap is a Suballocation.
		const uint32_t m_Offet = 0u;

		// The size of each descriptor in the heap.
		const uint32_t m_DescriptorSize;
		// The total number of descriptors in the heap.
		const uint32_t m_NumDescriptors;
	};
	using DescriptorHeapRef = SharedPtr<DescriptorHeap>;



	//=====================================================================
	// Descriptor Manager
	//  Implementation of RHIHeapDescriptorAllocator.
	class DescriptorManager : public RHIHeapDescriptorAllocator, public DeviceChild
	{
	public:
		DescriptorManager() = delete;
		DescriptorManager( Device* a_Device, DescriptorHeapRef&& a_Heap )
			: RHIHeapDescriptorAllocator( a_Heap->Type(), a_Heap->NumDescriptors() )
			,DeviceChild( a_Device )
			, m_Heap( std::move( a_Heap ) ) {}
		DescriptorManager( DescriptorManager&& other ) noexcept = default;
		DescriptorManager& operator=( DescriptorManager&& other ) noexcept = default;
		~DescriptorManager() = default;

		DescriptorHeap* Heap() { return m_Heap.get(); }
		const DescriptorHeap* Heap() const { return m_Heap.get(); }

		bool HandlesAllocation( ERHIDescriptorHeapType a_Type, ED3D12DescriptorHeapFlags a_Flags ) const
		{
			return RHIHeapDescriptorAllocator::HandlesAllocation( a_Type ) && m_Heap->Flags() == a_Flags;
		}

		// Returns true if the given heap is a child of this manager.
		bool IsHeapAChild( const DescriptorHeap& a_Heap ) const { return m_Heap->Heap() == a_Heap.Heap(); }

	private:
		DescriptorHeapRef m_Heap;
	};



	//=====================================================================
	// Descriptor Heap Manager
	//  The primary descriptor heap manager for the device.
	class DescriptorHeapManager final
	{
	public:
		// Allocate a descriptor heap.
		DescriptorHeapRef AllocateIndependentHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags, StringView a_DebugName = StringView{} );
		// Allocate a descriptor heap from the global heap pool.
		DescriptorHeapRef AllocateHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags, StringView a_DebugName = StringView{} );

		// Frees the given heap after a frame/s has completed.
		void DeferredFreeHeap( DescriptorHeapRef&& a_Heap );
		// Frees the given heap immediately.
		void ImmediateFreeHeap( DescriptorHeapRef&& a_Heap );

		// Add a heap to the pool so that it can be reused.
		void AddHeapToPool( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags );

	private:
		Device* m_Device = nullptr;
		Array<DescriptorManager> m_GlobalHeaps{};

		// A heap that is pooled for reuse.
		// Useful for descriptor heaps that are frequently created and destroyed.
		struct PooledHeap
		{
			ComPtr<ID3D12DescriptorHeap> Heap;
			uint32_t NumDescriptors;
			ERHIDescriptorHeapType Type;
			ED3D12DescriptorHeapFlags Flags;

			PooledHeap( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags )
				: Heap( std::move( a_Heap ) ), NumDescriptors( a_NumDescriptors ), Type( a_Type ), Flags( a_Flags ) {}
		};

		Array<PooledHeap> m_PooledHeaps{};
		std::mutex m_PooledHeapsMutex{};

	private:
		friend class D3D12RHI;

		NON_COPYABLE( DescriptorHeapManager );
		DescriptorHeapManager() = default;
		~DescriptorHeapManager() { Shutdown(); }

		// Initialize the descriptor heap manager
		void Init( Device* a_Device, uint32_t a_NumGlobalResourceDescriptors, uint32_t a_NumGlobalSamplerDescriptors );
		void Shutdown();

		// Try to retieve a pooled heap, can return null
		ComPtr<ID3D12DescriptorHeap> AcquirePooledHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags );
	};

} // namespace Tridium::D3D12